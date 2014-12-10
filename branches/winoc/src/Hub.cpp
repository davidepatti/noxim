#include "Hub.h"

void Hub::setup()
{

}


int Hub::tile2Port(int id)
{
    cout << name() << " tile2Port " << tile2port_mapping[id] << endl;
    return tile2port_mapping[id];
}


int Hub::tile2Hub(int id)
{
    //TODO add support multiple channels
    map<int, int>::iterator it = GlobalParams::hub_for_tile.find(id); 
    assert( (it != GlobalParams::hub_for_tile.end()) && "Specified Tile is not connected to any Hub");
    return it->second;
}


int Hub::route(Flit& f)
{
    for (int i=0; i< GlobalParams::hub_configuration[local_id].attachedNodes.size();i++)
    {
	if (GlobalParams::hub_configuration[local_id].attachedNodes[i]==f.dst_id)
	{
	    cout << name() << " destination tile " << f.dst_id << " is connected to this hub " << endl;
	    return tile2Port(f.dst_id);
	}
    }

    cout << name() << " destination tile " << f.dst_id << " is not connected to this hub " << endl;
    return DIRECTION_WIRELESS;

}

void Hub::radioProcess()
{
    if (reset.read()) 
    {
    } 
    else 
    {
	int port;
	for (int i = 0; i < num_rx_channels; i++) 
	{
	    if (!(target[i]->buffer_rx.IsEmpty()) && !buffer[port].IsFull() ) 
	    {
		Flit received_flit = target[i]->buffer_rx.Pop();
		port = tile2Port(received_flit.dst_id);
		cout << name() << "::radioProcess() wireless buffer_rx not empty, moving flit to buffer port " << port << endl;

		buffer[port].Push(received_flit);
	    }
	    // TODO: remove
	    else if (buffer[port].IsFull())
		cout << name() << " WARNING, buffer full for port " << port << endl;
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
		cout << name() << "::rxProcess() reading flit on port " << i << endl;
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

	// 1st phase: Reservation
	for (int j = 0; j < num_ports; j++) 
	{
	    int i = (start_from_port + j) % (num_ports);

	    if (!buffer[i].IsEmpty()) 
	    {
		cout << name() << "::txProcess() reservation: buffer not empty on port " << i << endl;

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
			cout << name() << "::txProcess() reservation:  wireless channel " << channel << " not available ..." << endl;
		    }
		    else if (reservation_table.isAvailable(r[i])) 
		    {
			reservation_table.reserve(i, r[i]);
		    }
		    else
			cout << name() << "::txProcess() reservation: no available port to route dir " << r[i] << endl;
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
			cout << name() << " inject to RH: Hub ID " << local_id << ", Type " << flit.flit_type << ", " << flit.src_id << "-->" << flit.dst_id << endl;
			cout << name() << " port[" << i << "] forward to channel [" << channel << "], flit: "
			    << flit << endl;

            int destHub = tile2Hub(flit.dst_id);
			cout << name() << "::txProcess() forwarding to wireless channel " << channel << " to reach HUB_" << destHub <<  endl;
            init[channel]->set_target_address(destHub);
			init[channel]->set_payload(flit);
			init[channel]->start_request_event.notify();

			buffer[i].Pop();

			if (flit.flit_type == FLIT_TYPE_TAIL) wireless_reservation_table.release(channel);

		    }
		    else
		    {
			cout << name() << "::txProcess() forwarding: No channel reserved for port direction " << i  << endl;
		    }
		}
		else // not wireless
		{
		    int d = reservation_table.getOutputPort(i);
		    if (d != NOT_RESERVED) 
		    {
			cout << name() << " inject to RH: Hub ID " << local_id << ", Type " << flit.flit_type << ", " << flit.src_id << "-->" << flit.dst_id << endl;
			cout << name() << " port[" << i << "] forward to direction [" << d << "], flit: "
			    << flit << endl;

			// TODO: put code that writes signals to the
			// tile
			cout << name() << "::txProcess() forwarding to port " << d << endl;

			flit_tx[d].write(flit);
			current_level_tx[d] = 1 - current_level_tx[d];
			req_tx[d].write(current_level_tx[d]);
			buffer[i].Pop();

			if (flit.flit_type == FLIT_TYPE_TAIL) reservation_table.release(d);

		    }
		    else
		    {
			cout << name() << "::txProcess() forwarding: No output port reserved for input port " << i  <<  endl;
		    }
		}

	    } //if buffer not empty
	}// for all the ports
    } 
}



