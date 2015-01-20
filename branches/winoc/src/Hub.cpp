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
	if (token_ring->currentToken() == local_id)
	{
	    for (int i = 0; i < num_tx_channels; i++) 
	    {
		if (!(init[i]->buffer_tx.IsEmpty()))
		{
		    LOG << "Buffer_tx is not empty for channel " << i << endl;
		    init[i]->start_request_event.notify();

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
	int * r = new int[num_rx_channels];

	for (int i = 0; i < num_rx_channels; i++) 
	{
	    if (!(target[i]->buffer_rx.IsEmpty()))
	    {
		LOG << "Buffer_rx is not empty for channel " << i << endl;
		Flit received_flit = target[i]->buffer_rx.Front();
		r[i] = tile2Port(received_flit.dst_id);
	    }
	}

	for (int i = 0; i < num_rx_channels; i++) 
	{
	    if (!(target[i]->buffer_rx.IsEmpty()))
	    {
		Flit received_flit = target[i]->buffer_rx.Front();

		if ( !buffer[r[i]].IsFull() ) 
		{
		    target[i]->buffer_rx.Pop();
		    LOG << "Moving flit from buffer_rx to buffer port " << r[i] << endl;

		    buffer[r[i]].Push(received_flit);
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

	    if ((req_rx[i]->read() == 1 - current_level_rx[i]) && !buffer[i].IsFull()) 
	    {
		LOG << "Reading flit on port " << i << endl;
		Flit received_flit = flit_rx[i]->read();

		buffer[i].Push(received_flit);

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
	int * r = new int[num_ports];

	if (local_id==0) buffer[0].Print("XXX HUB");
	// 1st phase: Reservation
	for (int j = 0; j < num_ports; j++) 
	{
	    int i = (start_from_port + j) % (num_ports);

	    if (!buffer[i].IsEmpty()) 
	    {
		LOG << "Reservation: buffer not empty on port " << i << endl;

		Flit flit = buffer[i].Front();

		r[i] = route(flit);

		if (flit.flit_type == FLIT_TYPE_HEAD) 
		{
		    if (r[i]==DIRECTION_WIRELESS)
		    {
			    // TODO: use actual channel
			    int channel = 0;
			    if (wireless_reservation_table.isAvailable(channel)) 
			    {
				wireless_reservation_table.reserve(i, channel);
			    }
			    else
			    LOG << "Reservation:  wireless channel " << channel << " not available ..." << endl;
		    }
		    else if (reservation_table.isAvailable(r[i])) 
		    {
			reservation_table.reserve(i, r[i]);
		    }
		    else
			LOG << "Reservation: no available port to route dir " << r[i] << endl;
		}
	    }
	}
	start_from_port++;

	// 2nd phase: Forwarding
	for (int i = 0; i < num_ports; i++) 
	{
	    if (!buffer[i].IsEmpty()) 
	    {     
		Flit flit = buffer[i].Front();

		if (r[i] == DIRECTION_WIRELESS)
		{
			int channel = wireless_reservation_table.getOutputPort(i);

			if (channel != NOT_RESERVED) 
			{
			    if (!(init[channel]->buffer_tx.IsFull()) )
			    {
				LOG << "Flit moved from buffer["<<i<<"] to buffer_tx["<<channel<<"] " << endl;
				buffer[i].Pop();
				init[channel]->buffer_tx.Push(flit);
				if (flit.flit_type == FLIT_TYPE_TAIL) wireless_reservation_table.release(channel);
			    }


			}
			else
			{
			    LOG << "Forwarding: No channel reserved for port direction " << i  << endl;
			}
		}
		else // not wireless
		{
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
			buffer[i].Pop();

			if (flit.flit_type == FLIT_TYPE_TAIL) reservation_table.release(d);

		    }
		    else
		    {
			LOG << "Forwarding: No output port reserved for input port " << i  <<  endl;
		    }
		}

	    } //if buffer not empty
	}// for all the ports
    } 
}



