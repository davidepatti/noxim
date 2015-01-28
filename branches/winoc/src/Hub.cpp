#include "Hub.h"



int Hub::tile2Port(int id)
{
    LOG << " tileID = " << id << " => portID = " << tile2port_mapping[id] << endl;
    return tile2port_mapping[id];
}




int Hub::route(Flit& f)
{
    for (vector<int>::size_type i=0; i< GlobalParams::hub_configuration[local_id].attachedNodes.size();i++)
    {
	if (GlobalParams::hub_configuration[local_id].attachedNodes[i]==f.dst_id)
	{
	    LOG << "Destination tile " << f.dst_id << " is connected to this hub " << endl;
	    return tile2Port(f.dst_id);
	}
    }

    LOG << "Destination tile " << f.dst_id << " is not connected to this hub " << endl;
    return DIRECTION_WIRELESS;

}

void Hub::txRadioProcess()
{
    if (reset.read()) 
    {
    } 
    else 
    {
	for (unsigned int i =0 ;i<txChannels.size();i++)
	{
	    int channel = txChannels[i];

	    if (token_ring->currentTokenHolder(channel) == local_id)
	    {
		if (!init[channel]->buffer_tx.IsEmpty())
		{
		    LOG << "Buffer_tx is not empty for channel " << channel << endl;
		    init[channel]->start_request_event.notify();
		}
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
		r[i] = tile2Port(received_flit.dst_id);
	    }
	}

	for (unsigned int i = 0; i < rxChannels.size(); i++) 
	{
	    int channel = rxChannels[i];

	    if (!(target[channel]->buffer_rx.IsEmpty()))
	    {
		Flit received_flit = target[channel]->buffer_rx.Front();

		if ( !buffer_to_tile[r[i]].IsFull() ) 
		{
		    target[channel]->buffer_rx.Pop();
		    LOG << "Moving flit from buffer_rx to buffer port " << r[i] << endl;

		    buffer_to_tile[r[i]].Push(received_flit);
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

	    if ((req_rx[i]->read() == 1 - current_level_rx[i]) && !buffer_from_tile[i].IsFull()) 
	    {
		LOG << "Reading flit on port " << i << endl;
		Flit received_flit = flit_rx[i]->read();

		buffer_from_tile[i].Push(received_flit);

		current_level_rx[i] = 1 - current_level_rx[i];

		/* TODO: re-enable
		// Incoming flit
		//stats.power.Buffering();

		if (received_flit.src_id == local_id)
		stats.power.EndToEnd();
		 */
	    }
	    ack_rx[i]->write(current_level_rx[i]);
	}
	// TODO: re-enable
	//stats.power.Leakage();
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
		LOG << "Reservation: buffer_from_tile not empty on port " << i << endl;

		Flit flit = buffer_from_tile[i].Front();
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
			LOG << "Reservation:  wireless channel " << channel << " not available ..." << endl;

		}
	    }

	    if (!buffer_to_tile[i].IsEmpty()) 
	    {
		LOG << "Reservation: buffer_to_tile not empty on port " << i << endl;

		Flit flit = buffer_to_tile[i].Front();
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

		assert(r_from_tile[i] == DIRECTION_WIRELESS);

		int channel = wireless_reservation_table.getOutputPort(i);

		if (channel != NOT_RESERVED) 
		{
		    if (!(init[channel]->buffer_tx.IsFull()) )
		    {
			LOG << "Flit moved from buffer["<<i<<"] to buffer_tx["<<channel<<"] " << endl;
			buffer_from_tile[i].Pop();
			init[channel]->buffer_tx.Push(flit);
			if (flit.flit_type == FLIT_TYPE_TAIL) wireless_reservation_table.release(channel);
		    }


		}
		else
		{
		    LOG << "Forwarding: No channel reserved for port direction " << i  << endl;
		}
	    }


	    if (!buffer_to_tile[i].IsEmpty()) 
	    {     
		Flit flit = buffer_to_tile[i].Front();

		assert(r_to_tile[i] != DIRECTION_WIRELESS);

		int d = reservation_table.getOutputPort(i);

		if (d != NOT_RESERVED) 
		{
		    LOG << "Inject to RH: Hub ID " << local_id << ", Type " << flit.flit_type << ", " << flit.src_id << "-->" << flit.dst_id << endl;
		    LOG << "port[" << i << "] forward to direction [" << d << "], flit: " << flit << endl;

		    // TODO: put code that writes signals to the
		    // tile
		    LOG << "Forwarding to port " << d << endl;

		    flit_tx[d].write(flit);
		    current_level_tx[d] = 1 - current_level_tx[d];
		    req_tx[d].write(current_level_tx[d]);
		    buffer_to_tile[i].Pop();

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



