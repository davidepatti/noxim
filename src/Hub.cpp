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
    return tile2port_mapping[id];
}

int Hub::route(Flit& f)
{
    for (vector<int>::size_type i=0; i< GlobalParams::hub_configuration[local_id].attachedNodes.size();i++)
    {
	if (GlobalParams::hub_configuration[local_id].attachedNodes[i]==f.dst_id)
	{
	    return tile2Port(f.dst_id);
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
	{
	    power.leakageAntennaBuffer();
	}

	for (int i = 0; i < num_ports; i++) 
	{
	    power.leakageBufferToTile();
	}
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
	    power.leakageAntennaBuffer();

	power.leakageTransceiverTx();
	power.biasingTx();
    }    

    // mandatory
    power.leakageLinkRouter2Hub();
    for (int i = 0; i < num_ports; i++) 
	power.leakageBufferFromTile();
}


void Hub::txRadioProcessTokenPacket(int channel)
{
    /*
    if (flag[channel]->read()==RELEASE_CHANNEL)
	flag[channel]->write(HOLD_CHANNEL);
	*/
    if (current_token_holder[channel]->read() == local_id
	&& flag[channel]->read()!=RELEASE_CHANNEL)
    {
	if (!init[channel]->buffer_tx.IsEmpty())
	{
	    Flit flit = init[channel]->buffer_tx.Front();
	    bool is_tail = (flit.flit_type == FLIT_TYPE_TAIL);

	    if (flit.flit_type == FLIT_TYPE_HEAD) 
		transmission_in_progress = true;

	    // TODO: check whether it would make sense to use transmission_in_progress to
	    // avoid multiple notify()
	    LOG << "Requesting transmission of flit " << flit << " on channel " << channel << endl;
	    init[channel]->start_request_event.notify();

	    if (is_tail)
	    {
		LOG << "TOKEN_PACKET: tail sent " << flit << ", releasing token for channel " << channel << endl;
		flag[channel]->write(RELEASE_CHANNEL);
		// TODO: vector for multiple channel
		transmission_in_progress = false;
	    }
	}
	else
	{
	    if (!transmission_in_progress)
	    {
		LOG << "TOKEN_PACKET: Buffer_tx empty and no trasmission in progress: releasing token for channel " << channel << endl;
		flag[channel]->write(RELEASE_CHANNEL);
	    }
	    else
		LOG << "TOKEN_PACKET: Buffer_tx empty, but trasmission in progress: holding token for channel " << channel << endl;
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
		LOG << "Starting transmission on channel " << channel << endl;
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
    
    // Moves a flit from buffer_to_tile to the appropriate signal_tx, depending on the destination node
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

    // Move a flit from antenna buffer_rx to the appropriate buffer_to_tile.
    //
    // Two different phases:
    // 1) stores routing decision about the incoming flit (e.g., to which output port)
    // 2) Moves the flits removing from antenna buffer_rx
    int * r = new int[rxChannels.size()];

    for (unsigned int i=0;i<rxChannels.size();i++)
    {
	int channel = rxChannels[i];
	if (!(target[channel]->buffer_rx.IsEmpty()))
	{
	    LOG << "Buffer_rx is not empty for channel " << channel << endl;
	    Flit received_flit = target[channel]->buffer_rx.Front();
	    power.antennaBufferFront();
	    r[i] = tile2Port(received_flit.dst_id);
	}
    }

    for (unsigned int i = 0; i < rxChannels.size(); i++) 
    {
	int channel = rxChannels[i];

	if (!(target[channel]->buffer_rx.IsEmpty()))
	{
	    Flit received_flit = target[channel]->buffer_rx.Front();
	    int vc = received_flit.vc_id;
	    power.antennaBufferFront();

	    if ( !buffer_to_tile[r[i]][vc].IsFull() ) 
	    {
		target[channel]->buffer_rx.Pop();
		power.antennaBufferPop();
		LOG << "Moving flit  " << received_flit << " from buffer_rx to buffer_to_tile, port [" << r[i] <<"][" << vc << "]" << endl;

		buffer_to_tile[r[i]][vc].Push(received_flit);
		power.bufferToTilePush();
	    }
	    else 
		LOG << "WARNING, buffer full for port " << r[i] << " VC " << vc << endl;

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
		    int channel = selectChannel(local_id,tile2Hub(flit.dst_id));

		    assert(channel!=NOT_VALID && "hubs are not connected by any channel");

		    LOG << " checking reservation availability of Channel " << channel << " by Input[" << i << "][" << vc << "] for flit " << flit << endl;

		    int rt_status = tile2antenna_reservation_table.checkReservation(r,channel);

		    if (rt_status == RT_AVAILABLE) 
		    {
			LOG << "Reservation of channel " << channel << " from Hub Input port["<< i << "]["<<vc<<"] by flit " << flit << endl;
			tile2antenna_reservation_table.reserve(r, channel);
		    }
		    else if (rt_status == RT_ALREADY)
		    {
			LOG << " RT_ALREADY reserved channel " << channel << " for flit " << flit << endl;
		    }
		    else if (rt_status == RT_OUTVC_BUSY)
		    {
			LOG << " RT_OUTVC_BUSY reservation for channel " << channel << " for flit " << flit << endl;
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
//	if (!buffer_from_tile[i][TODO_VC].deadlockFree())
//	{
//	    LOG << " deadlock on buffer " << i << endl;
//	    buffer_from_tile[i][TODO_VC].Print("deadlock");
//	}

	if (req_rx[i]->read() == 1 - current_level_rx[i]) 
	{
	    Flit received_flit = flit_rx[i]->read();
	    int vc = received_flit.vc_id;
	    LOG << "Reading " << received_flit << " from signal flit_rx[" << i << "]" << endl;

	    if (!buffer_from_tile[i][vc].IsFull())
	    {
		LOG << "Storing " << received_flit << " on buffer_from_tile[" << i << "][" << vc << "]" << endl;

		buffer_from_tile[i][vc].Push(received_flit);
		power.bufferFromTilePush();

		current_level_rx[i] = 1 - current_level_rx[i];
	    }
	    else
	    {
		LOG << "Cannot store " << received_flit << " on buffer_from_tile[" << i << "][" << vc << "] (IsFull)" << endl;
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

