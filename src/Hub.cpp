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
    /*
    // Check wheter accounting or not buffer to tile leakage 
    // For each port, two poweroff condition should be checked:
    // - the buffer to tile is empty
    // - it has not been reserved

    for (int port=0;port<num_ports;port++)
    {
	if (!buffer_to_tile[port][TODO_VC].IsEmpty() || 
	    !antenna2tile_reservation_table.isAvailable(port))
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

    */ // LAVORI
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
    /*
    for (unsigned int i=0;i<txChannels.size();i++)
    {
	// check if not empty or reserved
	if (!init[i]->buffer_tx.IsEmpty() || 
	    !tile2antenna_reservation_table.isAvailable(i) )
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
    */ // LAVORI
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

	    //flag[channel]->write(HOLD_CHANNEL);
	    LOG << "Requesting transmission transmission of flit " << flit << " on channel " << channel << endl;
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
    /* LAVORI
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
    
    for (int i = 0; i < num_ports; i++) 
    {
	if (!buffer_to_tile[i][TODO_VC].IsEmpty()) 
	{     
	    Flit flit = buffer_to_tile[i][TODO_VC].Front();

	    if (current_level_tx[i] == ack_tx[i].read())
	    {
		LOG << "Flit " << flit << " moved from buffer_to_tile[" << i <<"][" << TODO_VC << "] to signal flit_tx["<<i<<"] " << endl;

		flit_tx[i].write(flit);
		current_level_tx[i] = 1 - current_level_tx[i];
		req_tx[i].write(current_level_tx[i]);

		buffer_to_tile[i][TODO_VC].Pop();
		power.bufferToTilePop();
		power.r2hLink();
	    } //if buffer not empty
	    else
	    {
		LOG << "Flit " << flit << " cannot move from buffer_to_tile[" << i <<"] [" << TODO_VC << "] to signal flit_tx["<<i<<"] " << endl;
	    }
	}
    }

    // stores routing decision
    // need a vector to use this info to choose between the two
    // tables
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
	    power.antennaBufferFront();

	    if ( !buffer_to_tile[r[i]][TODO_VC].IsFull() ) 
	    {
		target[channel]->buffer_rx.Pop();
		power.antennaBufferPop();
		LOG << "Moving flit  " << received_flit << " from buffer_rx to buffer_to_tile, port [" << r[i] <<"][" << TODO_VC << "]" << endl;

		buffer_to_tile[r[i]][TODO_VC].Push(received_flit);
		power.bufferToTilePush();
	    }
	    else 
		LOG << "WARNING, buffer full for port " << r[i] << endl;

	}
    }
*/
}


void Hub::tileToAntennaProcess()
{
    /*
     
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
	
	for (int i = 0; i < num_ports; i++) 
	{
            ack_rx[i]->write(0);
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


    int * r_from_tile = new int[num_ports];

    // 1st phase: Reservation
    for (int j = 0; j < num_ports; j++) 
    {
	int i = (start_from_port + j) % (num_ports);

	if (!buffer_from_tile[i][TODO_VC].IsEmpty()) 
	{
	    LOG << "Reservation: buffer_from_tile not empty on port " << i << endl;

	    Flit flit = buffer_from_tile[i][TODO_VC].Front();
	    power.bufferFromTileFront();
	    r_from_tile[i] = route(flit);

	    if (flit.flit_type == FLIT_TYPE_HEAD) 
	    {
		assert(r_from_tile[i]==DIRECTION_WIRELESS);
		int channel = selectChannel(local_id,tile2Hub(flit.dst_id));

		assert(channel!=NOT_VALID && "hubs are not connected by any channel");

		if (tile2antenna_reservation_table.isAvailable(channel)) 
		{
		    tile2antenna_reservation_table.reserve(i, channel);
		    last_reserved = i;
		}
		else
		{
		    LOG << "Reservation:  wireless channel " << channel << " not available ..." << endl;
		}

	    }
	}

    }

    if (last_reserved!=NOT_VALID) start_from_port = (last_reserved+1)%num_ports;

    // 2nd phase: Forwarding
    for (int i = 0; i < num_ports; i++) 
    {
	if (!buffer_from_tile[i][TODO_VC].IsEmpty()) 
	{     
	    Flit flit = buffer_from_tile[i][TODO_VC].Front();
	    // powerFront already accounted in 1st phase

	    assert(r_from_tile[i] == DIRECTION_WIRELESS);

	    int channel = tile2antenna_reservation_table.getOutputPort(i);

	    if (channel != NOT_RESERVED) 
	    {
		if (!(init[channel]->buffer_tx.IsFull()) )
		{
		    buffer_from_tile[i][TODO_VC].Pop();
		    power.bufferFromTilePop();
		    init[channel]->buffer_tx.Push(flit);
		    power.antennaBufferPush();
		    if (flit.flit_type == FLIT_TYPE_TAIL) 
			tile2antenna_reservation_table.release(channel);

		    LOG << "Flit " << flit << " moved from buffer_from_tile["<<i<<"] to buffer_tx["<<channel<<"] " << endl;
		}
		else
		{
		    LOG << "Buffer Full: Cannot move flit " << flit << " from buffer_from_tile["<<i<<"] to buffer_tx["<<channel<<"] " << endl;
		    //init[channel]->buffer_tx.Print();
		}

	    }
	    else
	    {
		LOG << "Forwarding: No channel reserved for port direction " << i << ", flit " << flit << endl;
	    }
	}

    }// for all the ports

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
	    if (!buffer_from_tile[i][TODO_VC].IsFull())
	    {

		LOG << "Reading flit " << received_flit << " on port " << i << endl;

		buffer_from_tile[i][TODO_VC].Push(received_flit);
		power.bufferFromTilePush();

		current_level_rx[i] = 1 - current_level_rx[i];
	    }
	    else
	    {
		LOG << "Buffer full: Cannot store " << received_flit << " on port " << i << endl;
		//buffer_from_tile[i][TODO_VC].Print();
	    }


	}
	ack_rx[i]->write(current_level_rx[i]);
    }

    // IMPORTANT: do not move from here
    // The txPowerManager assumes that all flit buffer write have been done
    updateTxPower();
    */ //LAVORI
}

