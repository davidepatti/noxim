/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the global params needed by Noxim
 * to forward configuration to every sub-block
 */
#include "Hub.h"

int Hub::tile2Port(int id)
{
	// TODO: check all [..] map access to replace with at()
	return tile2port_mapping.at(id);
}

int Hub::route(Flit& f)
{
	// check if it is a local delivery
	for (vector<int>::size_type i=0; i< GlobalParams::hub_configuration[local_id].attachedNodes.size();i++)
	{
		// ...to a destination which is connected to the Hub
		if (GlobalParams::hub_configuration[local_id].attachedNodes[i]==f.dst_id)
		{
			return tile2Port(f.dst_id);
		}
		// ...or to a relay which is locally connected to the Hub
		if (GlobalParams::hub_configuration[local_id].attachedNodes[i]==f.hub_relay_node)
		{
			assert(GlobalParams::winoc_dst_hops>0);
			return tile2Port(f.hub_relay_node);
		}

	}
	return DIRECTION_WIRELESS;

}


void Hub::rxPowerManager()
{
	// Check wheter accounting or not buffer to tile leakage
	// For each port, two poweroff condition should be checked:
	// - the buffer to tile is empty
	// - it has not been reserved

	// currently only supported without VC
	assert(GlobalParams::n_virtual_channels==1);

	for (int port=0;port<num_ports;port++)
	{
		if (!buffer_to_tile[port][DEFAULT_VC].IsEmpty() ||
			antenna2tile_reservation_table.isNotReserved(port))
			power.leakageBufferToTile();

		else
			buffer_to_tile_poweroff_cycles[port]++;
	}


	for (unsigned int i=0;i<rxChannels.size();i++)
	{
		int ch_id = rxChannels[i];

		if (!target[ch_id]->buffer_rx.IsEmpty())
		{
			power.leakageAntennaBuffer();
		}
		else
			buffer_rx_sleep_cycles[ch_id]++;
	}

	// Check wheter accounting antenna RX buffer
	// check if there is at least one not empty antenna RX buffer
	// To be only applied if the current hub is in RADIO_EVENT_SLEEP_ON mode

	if (power.isSleeping())
		total_sleep_cycles++;

	else // not sleeping
	{
		power.wirelessSnooping();
		power.leakageTransceiverRx();
		power.biasingRx();
	}
}


void Hub::updateRxPower()
{
	if (GlobalParams::use_powermanager)
		rxPowerManager();
	else
	{
		power.wirelessSnooping();
		power.leakageTransceiverRx();
		power.biasingRx();

		for (unsigned int i=0;i<rxChannels.size();i++)
			for (int vc=0;vc<GlobalParams::n_virtual_channels;vc++)
				power.leakageAntennaBuffer();

		for (int i = 0; i < num_ports; i++)
			for (int vc=0;vc<GlobalParams::n_virtual_channels;vc++)
				power.leakageBufferToTile();
	}
}

void Hub::txPowerManager()
{
	for (unsigned int i=0;i<txChannels.size();i++)
	{
		// check if not empty or reserved
		if (!init[i]->buffer_tx.IsEmpty() ||
			tile2antenna_reservation_table.isNotReserved(i) )
		{
			power.leakageAntennaBuffer();
			// check the second condition for turning off analog tx
			if (power.isSleeping())
			{
				analogtxoff_cycles[i]++;
			}
			else
			{
				power.leakageTransceiverTx();
				power.biasingTx();
			}
		}
		else
		{   // abtx is empty and not reserved - turn off
			// note that this also applies to analog tx and serializer
			abtxoff_cycles[i]++;
			analogtxoff_cycles[i]++;
			total_ttxoff_cycles++;
		}
	}
}

void Hub::updateTxPower()
{
	if (GlobalParams::use_powermanager)
		txPowerManager();
	else
	{
		for (unsigned int i=0;i<txChannels.size();i++)
			for (int vc=0;vc<GlobalParams::n_virtual_channels;vc++)
				power.leakageAntennaBuffer();

		power.leakageTransceiverTx();
		power.biasingTx();
	}

	// mandatory
	power.leakageLinkRouter2Hub();
	for (int i = 0; i < num_ports; i++)
		for (int vc=0;vc<GlobalParams::n_virtual_channels;vc++)
			power.leakageBufferFromTile();
}


void Hub::txRadioProcessTokenPacket(int channel)
{
    int current_holder = current_token_holder[channel]->read();
    int current_channel_flag =flag[channel]->read();

	if ( current_holder == local_id && current_channel_flag !=RELEASE_CHANNEL)
	{
		if (!init[channel]->buffer_tx.IsEmpty())
		{
			Flit flit = init[channel]->buffer_tx.Front();

			// TODO: check whether it would make sense to use transmission_in_progress to
			// avoid multiple notify()
			LOG << "*** [Ch"<<channel<<"] Requesting transmission event of flit " << flit << endl;
			init[channel]->start_request_event.notify();
		}
		else
		{
			if (!transmission_in_progress[channel])
			{
				LOG << "*** [Ch"<<channel<<"] Buffer_tx empty and no trasmission in progress, releasing token" << endl;
				flag[channel]->write(RELEASE_CHANNEL);
			}
			else
				LOG << "*** [Ch"<<channel<<"] Buffer_tx empty, but trasmission in progress, holding token" << endl;
		}
	}
}

void Hub::txRadioProcessTokenHold(int channel)
{
	if (flag[channel]->read()==RELEASE_CHANNEL)
		flag[channel]->write(HOLD_CHANNEL);

	if (current_token_holder[channel]->read() == local_id)
	{
		if (!init[channel]->buffer_tx.IsEmpty())
		{
			//LOG << "Token holder for channel " << channel << " with not empty buffer_tx" << endl;
			if (current_token_expiration[channel]->read() < flit_transmission_cycles[channel])
			{
				//LOG << "TOKEN_HOLD policy: Not enough token expiration time for sending channel " << channel << endl;
			}
			else
			{
				flag[channel]->write(HOLD_CHANNEL);
				LOG << "*** [Ch" << channel << "] Starting transmission event" << endl;
				init[channel]->start_request_event.notify();
			}
		}
		else
		{
			//LOG << "TOKEN_HOLD policy: nothing to transmit, holding token for channel " << channel << endl;
		}
	}
}

void Hub::txRadioProcessTokenMaxHold(int channel)
{
	if (flag[channel]->read()==RELEASE_CHANNEL)
		flag[channel]->write(HOLD_CHANNEL);

	if (current_token_holder[channel]->read() == local_id)
	{
		if (!init[channel]->buffer_tx.IsEmpty())
		{
			//LOG << "Token holder for channel " << channel << " with not empty buffer_tx" << endl;

			if (current_token_expiration[channel]->read() < flit_transmission_cycles[channel])
			{
				//LOG << "TOKEN_MAX_HOLD: Not enough token expiration time, releasing token for channel " << channel << endl;
				flag[channel]->write(RELEASE_CHANNEL);
			}
			else
			{
				flag[channel]->write(HOLD_CHANNEL);
				LOG << "Starting transmission on channel " << channel << endl;
				init[channel]->start_request_event.notify();
			}
		}
		else
		{
			//LOG << "TOKEN_MAX_HOLD: Buffer_tx empty, releasing token for channel " << channel << endl;
			flag[channel]->write(RELEASE_CHANNEL);
		}
	}
}

void Hub::antennaToTileProcess()
{
	if (reset.read())
	{
		for (int i = 0; i < num_ports; i++)
		{
			req_tx[i]->write(0);
			current_level_tx[i] = 0;
		}
		return;
	}
	// IMPORTANT: do not move from here
	// The rxPowerManager must perform its checks before the flits are removed from buffers
	updateRxPower();


	/***********************************************************************************
      data flow from antenna(s) towards the tiles consist of 3 different steps:

      1) data received from a radio channel stored (if possible) to a specific buffer_rx
      2) data found on a buffer_rx moved to a buffer_to_tile
      3) data found on a buffer_to_tile moved to signal_tx 

      From a implementation perspective, they are performed in 3-2-1 order, to simulate
      a kind of pipelined sequence
     ***********************************************************************************/


	//////////////////////////////////////////////////////////////////////////////////////
	// Moves a flit from buffer_to_tile to the appropriate signal_tx
	// No routing required: each port is associated to a prefixed tile
	for (int i = 0; i < num_ports; i++)
	{
		// TODO: check blocking channel (like the blocking single signal ?)
		for (int k = 0;k < GlobalParams::n_virtual_channels; k++)
		{
			int vc = (start_from_vc[i]+k)%(GlobalParams::n_virtual_channels);

			if (!buffer_to_tile[i][vc].IsEmpty())
			{
				Flit flit = buffer_to_tile[i][vc].Front();

				LOG << "Flit " << flit << " found on buffer_to_tile[" << i <<"][" << vc << "] " << endl;
				if (current_level_tx[i] == ack_tx[i].read() &&
					buffer_full_status_tx[i].read().mask[vc] == false)
				{
					LOG << "Flit " << flit << " moved from buffer_to_tile[" << i <<"][" << vc << "] to signal flit_tx["<<i<<"] " << endl;

					flit_tx[i].write(flit);
					current_level_tx[i] = 1 - current_level_tx[i];
					req_tx[i].write(current_level_tx[i]);

					buffer_to_tile[i][vc].Pop();
					power.bufferToTilePop();
					power.r2hLink();
					break; // port flit transmitted, skip remaining VCs
				}
				else
				{
					LOG << "Flit " << flit << " cannot move from buffer_to_tile[" << i <<"] [" << vc << "] to signal flit_tx["<<i<<"] " << endl;
				}
			}//if buffer not empty
		}
		start_from_vc[i] = (start_from_vc[i]+1)%GlobalParams::n_virtual_channels;
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Move a flit from antenna buffer_rx to the appropriate buffer_to_tile.
	//
	// Two different phases:
	// 1) stores routing decision about the incoming flit (e.g., to which output port)
	// 2) Moves the flits removing from antenna buffer_rx

	for (unsigned int i = 0; i < rxChannels.size(); i++)
	{
		int channel = rxChannels[i];

		if (!(target[channel]->buffer_rx.IsEmpty()))
		{
			Flit received_flit = target[channel]->buffer_rx.Front();
			power.antennaBufferFront();

			// Check antenna buffer_rx making appropriate reservations
			if (received_flit.flit_type==FLIT_TYPE_HEAD)
			{
				int dst_port;

				if (received_flit.hub_relay_node!=NOT_VALID)
					dst_port = tile2Port(received_flit.hub_relay_node);
				else
                    dst_port = tile2Port(received_flit.dst_id);

				TReservation r;
				r.input = channel;
				r.vc = received_flit.vc_id;

				LOG << " Checking reservation availability of output port " << dst_port << " by channel " << channel << " for flit " << received_flit << endl;

				int rt_status = antenna2tile_reservation_table.checkReservation(r,dst_port);

				if (rt_status == RT_AVAILABLE)
				{
					LOG << "Reserving output port " << dst_port << " by channel " << channel << " for flit " << received_flit << endl;
					antenna2tile_reservation_table.reserve(r, dst_port);

					// The number of commucation using the wireless network, accounting also
					// partial wired path
					wireless_communications_counter++;
				}
				else if (rt_status == RT_ALREADY_SAME)
				{
					LOG << " RT_ALREADY_SAME reserved direction " << dst_port << " for flit " << received_flit << endl;
				}
				else if (rt_status == RT_OUTVC_BUSY)
				{
					LOG << " RT_OUTVC_BUSY reservation direction " << dst_port << " for flit " << received_flit << endl;
				}
				else assert(false); // no meaningful status here


			}
		}
	}
	// forwarding
	for (unsigned int i = 0; i < rxChannels.size(); i++)
	{
		int channel = rxChannels[i];
		vector<pair<int,int> > reservations = antenna2tile_reservation_table.getReservations(channel);

		if (reservations.size()!=0)
		{
			int rnd_idx = rand()%reservations.size();

			int port = reservations[rnd_idx].first;
			int vc = reservations[rnd_idx].second;

			if (!(target[channel]->buffer_rx.IsEmpty()))
			{
				Flit received_flit = target[channel]->buffer_rx.Front();
				power.antennaBufferFront();

				if ( !buffer_to_tile[port][vc].IsFull() )
				{
					target[channel]->buffer_rx.Pop();
					power.antennaBufferPop();
					LOG << "*** [Ch" << channel << "] Moving flit  " << received_flit << " from buffer_rx to buffer_to_tile[" << port <<"][" << vc << "]" << endl;

					buffer_to_tile[port][vc].Push(received_flit);
					power.bufferToTilePush();

					if (received_flit.flit_type == FLIT_TYPE_TAIL)
					{
						LOG << "Releasing reservation for output port " << port << ", flit " << received_flit << endl;
						TReservation r;
						r.input = channel;
						r.vc = vc;
						antenna2tile_reservation_table.release(r,port);
					}
				}
				else
					LOG << "Full buffer_to_tile[" << port <<"][" << vc << "]" << ", cannot store " << received_flit << endl;
			}
			else
			{
				// should be ok
				/*
                LOG << "WARNING: empty target["<<channel<<"] buffer_rx, but reservation still present, if correct, remove assertion below " << endl;
                assert(false);
                */
			}
		}
	}
}

void Hub::tileToAntennaProcess()
{
	// double cycle = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
	// if (cycle > 0 && cycle < 58428)
	// {
	//     if (local_id == 1)
	//     {
	//         cout << "CYCLES " << cycle << endl;
	//         for (int j = 0; j < num_ports; j++)
	//     	buffer_from_tile[j].Print();;
	//         init[0]->buffer_tx.Print();
	//         cout << endl;
	//     }
	// }

	if (reset.read())
	{
		for (unsigned int i =0 ;i<txChannels.size();i++)
		{
			int channel = txChannels[i];
			flag[channel]->write(HOLD_CHANNEL);
		}

		TBufferFullStatus bfs;
		for (int i = 0; i < num_ports; i++)
		{
			ack_rx[i]->write(0);
			buffer_full_status_rx[i].write(bfs);
			current_level_rx[i] = 0;
		}
		return;
	}

	for (unsigned int i =0 ;i<txChannels.size();i++)
	{
		int channel = txChannels[i];

		string macPolicy = token_ring->getPolicy(channel).first;

		if (macPolicy == TOKEN_PACKET)
			txRadioProcessTokenPacket(channel);
		else if (macPolicy == TOKEN_HOLD)
			txRadioProcessTokenHold(channel);
		else if (macPolicy == TOKEN_MAX_HOLD)
			txRadioProcessTokenMaxHold(channel);
		else
			assert(false);
	}

	int last_reserved = NOT_VALID;

	// used to store routing decisions
	int * r_from_tile[num_ports];
	for (int i=0;i<num_ports;i++)
		r_from_tile[i] = new int[GlobalParams::n_virtual_channels];

	// 1st phase: Reservation
	for (int j = 0; j < num_ports; j++)
	{
		int i = (start_from_port + j) % (num_ports);

		for (int k = 0;k < GlobalParams::n_virtual_channels; k++)
		{
			int vc = (start_from_vc[i]+k)%(GlobalParams::n_virtual_channels);

			if (!buffer_from_tile[i][vc].IsEmpty())
			{
				LOG << "Reservation: buffer_from_tile[" << i <<"][" << vc << "] not empty " << endl;

				Flit flit = buffer_from_tile[i][vc].Front();

				assert(flit.vc_id == vc);

				power.bufferFromTileFront();
				r_from_tile[i][vc] = route(flit);

				if (flit.flit_type == FLIT_TYPE_HEAD)
				{
					TReservation r;
					r.input = i;
					r.vc = vc;

					assert(r_from_tile[i][vc]==DIRECTION_WIRELESS);
					int channel;

					if (flit.hub_relay_node==NOT_VALID)
						channel = selectChannel(local_id, tile2Hub(flit.dst_id));
					else
						channel = selectChannel(local_id, tile2Hub(flit.hub_relay_node));


					assert(channel!=NOT_VALID && "hubs are not connected by any channel");

					LOG << "Checking reservation availability of Channel " << channel << " by Hub port[" << i << "][" << vc << "] for flit " << flit << endl;

					int rt_status = tile2antenna_reservation_table.checkReservation(r,channel);

					if (rt_status == RT_AVAILABLE)
					{
						LOG << "Reservation of channel " << channel << " from Hub port["<< i << "]["<<vc<<"] by flit " << flit << endl;
						tile2antenna_reservation_table.reserve(r, channel);
					}
					else if (rt_status == RT_ALREADY_SAME)
					{
						LOG << "RT_ALREADY_SAME reserved channel " << channel << " for flit " << flit << endl;
					}
					else if (rt_status == RT_OUTVC_BUSY)
					{
						LOG << "RT_OUTVC_BUSY reservation for channel " << channel << " for flit " << flit << endl;
					}
					else if (rt_status == RT_ALREADY_OTHER_OUT)
					{
						LOG << "RT_ALREADY_OTHER_OUT a channel different from " << channel << " already reserved by Hub port["<< i << "]["<<vc<<"]" << endl;
					}
					else assert(false); // no meaningful status here
				}
			}
		}
		start_from_vc[i] = (start_from_vc[i]+1)%GlobalParams::n_virtual_channels;
	} // for num_ports

	if (last_reserved!=NOT_VALID)
		start_from_port = (last_reserved+1)%num_ports;

	// 2nd phase: Forwarding
	for (int i = 0; i < num_ports; i++)
	{
		vector<pair<int,int> > reservations = tile2antenna_reservation_table.getReservations(i);

		if (reservations.size()!=0)
		{
			int rnd_idx = rand()%reservations.size();

			int o = reservations[rnd_idx].first;
			int vc = reservations[rnd_idx].second;

			if (!buffer_from_tile[i][vc].IsEmpty())
			{
				Flit flit = buffer_from_tile[i][vc].Front();
				// powerFront already accounted in 1st phase

				assert(r_from_tile[i][vc] == DIRECTION_WIRELESS);

				int channel =  o;

				if (channel != NOT_RESERVED)
				{
					if (!(init[channel]->buffer_tx.IsFull()) )
					{
						buffer_from_tile[i][vc].Pop();
						power.bufferFromTilePop();
						init[channel]->buffer_tx.Push(flit);
						power.antennaBufferPush();
						if (flit.flit_type == FLIT_TYPE_TAIL)
						{
							TReservation r;
							r.input = i;
							r.vc = vc;
							tile2antenna_reservation_table.release(r,channel);
						}

						LOG << "Flit " << flit << " moved from buffer_from_tile["<<i<<"]["<<vc<<"]  to buffer_tx["<<channel<<"] " << endl;
					}
					else
					{
						LOG << "Buffer Full: Cannot move flit " << flit << " from buffer_from_tile["<<i<<"] to buffer_tx["<<channel<<"] " << endl;
						//init[channel]->buffer_tx.Print();
					}
				}
				else
				{
					LOG << "Forwarding: No channel reserved for input port [" << i << "][" << vc << "], flit " << flit << endl;
				}
			}

		}// for all the ports
	}

	for (int i = 0; i < num_ports; i++)
	{

		if (req_rx[i]->read() == 1 - current_level_rx[i])
		{
			Flit received_flit = flit_rx[i]->read();
			int vc = received_flit.vc_id;
			LOG << "Reading " << received_flit << " from signal flit_rx[" << i << "]" << endl;

			/*
            if (!buffer_from_tile[i][vc].deadlockFree())
            {
            LOG << " deadlock on buffer " << i << endl;
            buffer_from_tile[i][vc].Print();
            }
            */

			if (!buffer_from_tile[i][vc].IsFull())
			{
				LOG << "Storing " << received_flit << " on buffer_from_tile[" << i << "][" << vc << "]" << endl;

				buffer_from_tile[i][vc].Push(received_flit);
				power.bufferFromTilePush();

				current_level_rx[i] = 1 - current_level_rx[i];
			}
			else
			{
				LOG << "Buffer Full: Cannot store " << received_flit << " on buffer_from_tile[" << i << "][" << vc << "]" << endl;
				//buffer_from_tile[i][TODO_VC].Print();
			}
		}
		ack_rx[i]->write(current_level_rx[i]);
		// updates the mask of VCs to prevent incoming data on full buffers
		TBufferFullStatus bfs;
		for (int vc=0;vc<GlobalParams::n_virtual_channels;vc++)
			bfs.mask[vc] = buffer_from_tile[i][vc].IsFull();
		buffer_full_status_rx[i].write(bfs);
	}

	// IMPORTANT: do not move from here
	// The txPowerManager assumes that all flit buffer write have been done
	updateTxPower();
}

int Hub::selectChannel(int src_hub, int dst_hub) const
{
	vector<int> & first = GlobalParams::hub_configuration[src_hub].txChannels;
	vector<int> & second = GlobalParams::hub_configuration[dst_hub].rxChannels;

	vector<int> intersection;

	for (unsigned int i=0;i<first.size();i++)
	{
		for (unsigned int j=0;j<second.size();j++)
		{
			if (first[i] ==second[j])
				intersection.push_back(first[i]);
		}
	}

	if (intersection.size()==0)
	    return NOT_VALID;

	if (GlobalParams::channel_selection==CHSEL_RANDOM)
		return intersection[rand()%intersection.size()];
	else
	if (GlobalParams::channel_selection==CHSEL_FIRST_FREE)
	{
		int start_channel = rand()%intersection.size();
		int k;

		for (vector<int>::size_type i=0;i<intersection.size();i++)
		{
			k = (start_channel+i)%intersection.size();

			if (!transmission_in_progress[intersection[k]])
			{
				cout << "Found free channel " << intersection[k] << " on (src,dest) (" << src_hub << "," << dst_hub << ") " << endl;
				return intersection[k];
			}
		}
		cout << "All channel busy, applying random selection " << endl;
		return intersection[rand()%intersection.size()];
	}

	return NOT_VALID;
}
