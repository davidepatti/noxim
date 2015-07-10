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


void Hub::wirxPowerManager()
{
    /*
    //LOG << " buffer_to_tile STATUS: " << endl;
    for (int i = 0; i < num_ports; i++) 
    {
	//if (!buffer_to_tile[i].IsEmpty()) cout << "*"; else cout << ".";

    }
    //cout << endl;
    */

    if (power.isSleeping())
    {
	total_sleep_cycles++;

	bool account_buffer_to_tile_leakage = false;
	// check if there is at least one not empty antenna RX buffer
	for (int i=0;i<rxChannels.size();i++)
	{
	    int ch_id = rxChannels[i];

	    if (!target[ch_id]->buffer_rx.IsEmpty())
	    {
		account_buffer_to_tile_leakage = true;
		power.leakageAntennaBuffer();
	    }
	    else
		buffer_rx_sleep_cycles[ch_id]++;
	}


	for (int i = 0; i < num_ports; i++) 
	{
	    if (account_buffer_to_tile_leakage || !buffer_to_tile[i].IsEmpty())
		power.leakageBufferToTile();
	    else
		buffer_to_tile_sleep_cycles[i]++;
	}
	
    }
    else
    {
	power.wirelessSnooping();
	
	for (int i=0;i<rxChannels.size();i++)
	    power.leakageAntennaBuffer();

	for (int i = 0; i < num_ports; i++) 
	{
		power.leakageBufferToTile();
	}
	power.leakageTransceiverRx();
	power.biasingRx();
    }

}


void Hub::perCycleUpdate()
{
    // Mandatory leakage contributions **************************
    //
    // Initiators buffer_tx
    for (int i=0;i<txChannels.size();i++)
    {
	power.leakageAntennaBuffer();
    }

    // buffer from tile connections
    for (int i = 0; i < num_ports; i++) 
    {
	power.leakageBufferFromTile();
	power.leakageLinkRouter2Hub();
    }

    power.leakageTransceiverTx();
    power.biasingTx();
    //***********************************************************


}

void Hub::updateRxPower()
{

    if (GlobalParams::use_wirxsleep)
	wirxPowerManager();
    else
    {
	power.wirelessSnooping();
	for (int i=0;i<rxChannels.size();i++)
	{
	    power.leakageAntennaBuffer();
	}

	for (int i = 0; i < num_ports; i++) 
	{
	    power.leakageBufferToTile();
	}
	power.leakageTransceiverRx();
	power.biasingRx();
    }    
}

void Hub::txRadioProcessTokenPacket(int channel)
{
    if (flag[channel]->read()==RELEASE_CHANNEL)
	flag[channel]->write(HOLD_CHANNEL);

    if (current_token_holder[channel]->read() == local_id)
    {
	if (!init[channel]->buffer_tx.IsEmpty())
	{
	    bool is_tail = init[channel]->buffer_tx.Front().flit_type == FLIT_TYPE_TAIL;

	    flag[channel]->write(HOLD_CHANNEL);
	    //LOG << "Starting transmission on channel " << channel << endl;
	    init[channel]->start_request_event.notify();

	    if (is_tail)
	    {
		LOG << "TOKEN_PACKET: tail sent, releasing token for channel " << channel << endl;
		flag[channel]->write(RELEASE_CHANNEL);
	    }

	}
	else
	{
	    //LOG << "TOKEN_PACKET: Buffer_tx empty, releasing token for channel " << channel << endl;
	    flag[channel]->write(RELEASE_CHANNEL);
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



void Hub::antennaToTile()
{
    if (reset.read()) 
    {
        for (int i = 0; i < num_ports; i++) 
        {
	    req_tx[i]->write(0);
	    current_level_tx[i] = 0;
        }
    } 
    else
    {
	updateRxPower();
	
	for (int i = 0; i < num_ports; i++) 
	{
	    if (!buffer_to_tile[i].IsEmpty()) 
	    {     
		Flit flit = buffer_to_tile[i].Front();

		flit_tx[i].write(flit);
		power.r2hLink();
		current_level_tx[i] = 1 - current_level_tx[i];
		req_tx[i].write(current_level_tx[i]);
		LOG << "Flit moved from buffer_to_tile[" << i <<"] to signal flit_tx["<<i<<"] " << endl;
		buffer_to_tile[i].Pop();
		power.bufferToTilePop();
	    } //if buffer not empty
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

		if ( !buffer_to_tile[r[i]].IsFull() ) 
		{
		    target[channel]->buffer_rx.Pop();
		    power.antennaBufferPop();
		    LOG << "Moving flit from buffer_rx to buffer_to_tile, port " << r[i] << endl;

		    buffer_to_tile[r[i]].Push(received_flit);
		    power.bufferToTilePush();
		}
		else 
		    LOG << "WARNING, buffer full for port " << r[i] << endl;

	    }
	}
    }
}


void Hub::tileToAntenna()
{
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
    } 
    else 
    {
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

    }

    int * r_from_tile = new int[num_ports];

    // 1st phase: Reservation
    for (int j = 0; j < num_ports; j++) 
    {
	int i = (start_from_port + j) % (num_ports);

	if (!buffer_from_tile[i].IsEmpty()) 
	{
	    //LOG << "Reservation: buffer_from_tile not empty on port " << i << endl;

	    Flit flit = buffer_from_tile[i].Front();
	    power.bufferFromTileFront();
	    r_from_tile[i] = route(flit);

	    if (flit.flit_type == FLIT_TYPE_HEAD) 
	    {
		assert(r_from_tile[i]==DIRECTION_WIRELESS);
		int channel = selectChannel(local_id,tile2Hub(flit.dst_id));

		assert(channel!=NOT_VALID && "hubs are connected by any channel");

		if (wireless_reservation_table.isAvailable(channel)) 
		{
		    wireless_reservation_table.reserve(i, channel);
		}
		else
		{
		    //LOG << "Reservation:  wireless channel " << channel << " not available ..." << endl;
		}

	    }
	}

    }

    start_from_port++;

    // 2nd phase: Forwarding
    for (int i = 0; i < num_ports; i++) 
    {
	if (!buffer_from_tile[i].IsEmpty()) 
	{     
	    Flit flit = buffer_from_tile[i].Front();
	    // powerFront already accounted in 1st phase

	    assert(r_from_tile[i] == DIRECTION_WIRELESS);

	    int channel = wireless_reservation_table.getOutputPort(i);

	    if (channel != NOT_RESERVED) 
	    {
		if (!(init[channel]->buffer_tx.IsFull()) )
		{
		    LOG << "Flit moved from buffer_from_tile["<<i<<"] to buffer_tx["<<channel<<"] " << endl;
		    buffer_from_tile[i].Pop();
		    power.bufferFromTilePop();
		    init[channel]->buffer_tx.Push(flit);
		    power.antennaBufferPush();
		    if (flit.flit_type == FLIT_TYPE_TAIL) wireless_reservation_table.release(channel);
		}


	    }
	    else
	    {
		//LOG << "Forwarding: No channel reserved for port direction " << i  << endl;
	    }
	}

    }// for all the ports

    for (int i = 0; i < num_ports; i++) 
    {
	/*
	if (!buffer_from_tile[i].deadlockFree())
	{
	    LOG << " deadlock on buffer " << i << endl;
	    buffer_from_tile[i].Print("deadlock");
	}
	*/

	if ((req_rx[i]->read() == 1 - current_level_rx[i]) && !buffer_from_tile[i].IsFull()) 
	{
	    //LOG << "Reading flit on port " << i << endl;
	    Flit received_flit = flit_rx[i]->read();

	    buffer_from_tile[i].Push(received_flit);
	    power.bufferFromTilePush();

	    current_level_rx[i] = 1 - current_level_rx[i];

	}
	ack_rx[i]->write(current_level_rx[i]);
    }
}
