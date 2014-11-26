#include "Hub.h"

void Hub::setup()
{

}


int Hub::route(Flit& f)
{
    // TODO: dummy test code
    // HUB 0 simply send on wireless, other Hubs move to first port 0
    if (local_id == 0)
    {
	cout << name() << " TEST routing, sending to wireless channel 0" << endl;
	return DIRECTION_WIRELESS;
    }

    cout << name() << " TEST routing, sending to port 0" << endl;
    return 0;

}

void Hub::radioProcess()
{
    if (reset.read()) 
    {
    } 
    else 
    {
	for (int i = 0; i < num_rx_channels; i++) 
	{
	    // TODO: fixed to first port
	    int port = 0;

	    if (!(target[i]->buffer_rx.IsEmpty()) ) 
	    {
		cout << name() << " buffer_rx not empty..." << endl;
		Flit received_flit = target[i]->buffer_rx.Pop();

		buffer[port].Push(received_flit);
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
        reservation_table.clear();
    } 
    else 
    {
	////////////////////////////////////////////////////////////////
	// For each port decide if a new flit can be accepted

	for (int i = 0; i < num_ports; i++) 
	{
	    if ((req_rx[i]->read() == 1 - current_level_rx[i]) && !buffer[i].IsFull()) 
	    {
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
	// 1st phase: Reservation
	for (int j = 0; j < num_ports; j++) 
	{
	    int i = (start_from_port + j) % (num_ports);

	    if (!buffer[i].IsEmpty()) 
	    {
		cout << name() << " buffer not empty on port " << i << endl;
		// TODO: put code to handle TLM transmission
		//
		Flit flit = buffer[i].Front();

		int o = route(flit);

		if (flit.flit_type == FLIT_TYPE_HEAD) 
		{
		    if (reservation_table.isAvailable(o)) 
		    {
			reservation_table.reserve(i, o);
		    }
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

		int o = reservation_table.getOutputPort(i);
		if (o != NOT_RESERVED) 
		{
		    cout << name() << " inject to RH: Hub ID " << local_id << ", Type " << flit.flit_type << ", " << flit.src_id << "-->" << flit.dst_id << endl;
		    cout << name() << " port[" << i << "] forward to direction [" << o << "], flit: "
			<< flit << endl;

		    if (o==DIRECTION_WIRELESS)
		    {

			cout << name() << " forwarding to wireless channel 0" << endl;
			init[0]->set_payload(flit);
			init[0]->start_request_event.notify();
		    }
		    else
		    {
			// TODO: put code that writes signals to the
			// tile
			cout << name() << " forwarding to port 0" << endl;
		    }

		    buffer[i].Pop();

		    if (flit.flit_type == FLIT_TYPE_TAIL) reservation_table.release(o);

		}
		else
		{
		    cout << name() << " output port for direction " << i  << " is not reserved ..." << endl;
		}

	    }
	} //if
    }				// for
}



