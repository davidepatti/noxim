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


void Hub::perCycleUpdate()
{
        for (int i = 0; i < num_ports; i++) 
	{
	    // TX
	    power.leakageBuffer();
	    // RX
	    power.leakageBuffer();

	    power.leakageLinkRouter2Hub();
	}

	for (int i=0;i<txChannels.size();i++)
	{
	    power.leakageAntennaBuffer();
	}

	for (int i=0;i<rxChannels.size();i++)
	{
	    power.leakageAntennaBuffer();
	}
	
	if (!(power.isSleeping()))
	{
	    power.leakageTransceiverRx();
	    power.biasingRx();
	}

	power.leakageTransceiverTx();
	power.biasingTx();
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
	    LOG << "Starting transmission on channel " << channel << endl;
	    init[channel]->start_request_event.notify();

	    if (is_tail)
	    {
		//LOG << "TOKEN_PACKET: tail sent, releasing token for channel " << channel << endl;
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

void Hub::txRadioProcess()
{
    if (reset.read()) 
    {
	for (unsigned int i =0 ;i<txChannels.size();i++)
	{
	    int channel = txChannels[i];
	    flag[channel]->write(HOLD_CHANNEL);
	}
	
    } 
    else 
    {
	for (unsigned int i =0 ;i<txChannels.size();i++)
	{
	    int channel = txChannels[i];

	    switch (token_ring->getPolicy(channel))
	    {
		case TOKEN_PACKET:
		    txRadioProcessTokenPacket(channel);
		    break;
		case TOKEN_HOLD:
		    txRadioProcessTokenHold(channel);
		    break;
		case TOKEN_MAX_HOLD:
		    txRadioProcessTokenMaxHold(channel);
		    break;
		default: assert(false);
	    }


	}

    }
}

void Hub::rxRadioProcess()
{
    if (reset.read()) 
    {
    } 
    else 
    {
	if (!power.isSleeping())
	    power.wirelessSnooping();

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
		    //LOG << "Moving flit from buffer_rx to buffer port " << r[i] << endl;

		    buffer_to_tile[r[i]].Push(received_flit);
		    power.bufferPush();
		}
		else 
		    LOG << "WARNING, buffer full for port " << r[i] << endl;

	    }
	}
    }
}

void Hub::rxProcess()
{
    if (reset.read()) 
    {
        for (int i = 0; i < num_ports; i++) 
        {
            ack_rx[i]->write(0);
            current_level_rx[i] = 0;
        }
    } 
    else 
    {
	////////////////////////////////////////////////////////////////
	// For each port decide if a new flit can be accepted

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
		power.bufferPush();

		current_level_rx[i] = 1 - current_level_rx[i];

	    }
	    ack_rx[i]->write(current_level_rx[i]);
	}
    }
}



void Hub::txProcess()
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
	// stores routing decision
	// need a vector to use this info to choose between the two
	// tables
	int * r_from_tile = new int[num_ports];
	int * r_to_tile = new int[num_ports];

	// 1st phase: Reservation
	for (int j = 0; j < num_ports; j++) 
	{
	    int i = (start_from_port + j) % (num_ports);

	    if (!buffer_from_tile[i].IsEmpty()) 
	    {
		//LOG << "Reservation: buffer_from_tile not empty on port " << i << endl;

		Flit flit = buffer_from_tile[i].Front();
		power.bufferFront();
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

	    if (!buffer_to_tile[i].IsEmpty()) 
	    {
		//LOG << "Reservation: buffer_to_tile not empty on port " << i << endl;

		Flit flit = buffer_to_tile[i].Front();
		power.bufferFront();
		r_to_tile[i] = route(flit);

		if (flit.flit_type == FLIT_TYPE_HEAD) 
		{
		    assert(r_to_tile[i]!=DIRECTION_WIRELESS);

		    if (reservation_table.isAvailable(r_to_tile[i])) 
		    {
			reservation_table.reserve(i, r_to_tile[i]);
		    }
		    else
			LOG << "Reservation: no available port to route dir " << r_to_tile[i] << endl;
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
			power.bufferPop();
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


	    if (!buffer_to_tile[i].IsEmpty()) 
	    {     
		Flit flit = buffer_to_tile[i].Front();
		// powerFront already accounted in 1st phase

		assert(r_to_tile[i] != DIRECTION_WIRELESS);

		int d = reservation_table.getOutputPort(i);

		if (d != NOT_RESERVED) 
		{

		    flit_tx[d].write(flit);
		    power.r2hLink();
		    current_level_tx[d] = 1 - current_level_tx[d];
		    req_tx[d].write(current_level_tx[d]);
		    buffer_to_tile[i].Pop();
		    power.bufferPop();

		    if (flit.flit_type == FLIT_TYPE_TAIL) reservation_table.release(d);

		}
		else
		{
		    LOG << "Forwarding: No output port reserved for input port " << i  <<  endl;
		}

	    } //if buffer not empty
	}// for all the ports
    } 
}



