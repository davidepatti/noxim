#include "Hub.h"

void Hub::setup()
{

}

void Hub::rxProcess()
{
    if (reset.read()) 
    {
	for (int i = 0; i < MAX_HUB_PORTS; i++) 
	{
	    ack_rx[i].write(0);
	    current_level_rx[i] = 0;
	}
	reservation_table.clear();

	/* TODO: re-enable
	   routed_flits = 0;
	   local_drained = 0;
	 */
    } 
    else 
    {
	    ////////////////////////////////////////////////////////////////
	    // For each port decide if a new flit can be accepted

	    for (int i = 0; i < MAX_HUB_PORTS; i++) 
	    {
		if ((req_rx[i].read() == 1 - current_level_rx[i]) && !buffer[i].IsFull()) 
		{
		    Flit received_flit = flit_rx[i].read();

		    if (GlobalParams::verbose_mode > VERBOSE_OFF) {
			cout << sc_time_stamp().to_double() /
			    1000 << ": Hub[" << local_id << "], Port[" << i
			    << "], Received flit: " << received_flit << endl;
		    }

		    buffer[i].Push(received_flit);

		    current_level_rx[i] = 1 - current_level_rx[i];

		    /* TODO: re-enable
		    // Incoming flit
		    //stats.power.Buffering();

		    if (received_flit.src_id == local_id)
		    stats.power.EndToEnd();
		     */
		}
		ack_rx[i].write(current_level_rx[i]);
	    }
	// TODO: re-enable
	//stats.power.Leakage();
    }
}



void Hub::txProcess()
{
    if (reset.read()) 
    {
	for (int i = 0; i < MAX_HUB_PORTS; i++) 
	{
	    req_tx[i].write(0);
	    current_level_tx[i] = 0;
	}
    } 
    else 
    {
	// 1st phase: Reservation
	for (int j = 0; j < MAX_HUB_PORTS; j++) 
	{
	    int i = (start_from_port + j) % (MAX_HUB_PORTS);

	    if (!buffer[i].IsEmpty()) 
	    {
		// TODO: put code to handle TLM transmission
		//
		Flit flit = buffer[i].Front();

		// TODO: replace
		int o = 0;

		if (flit.flit_type == FLIT_TYPE_HEAD) 
		{
		    if (reservation_table.isAvailable(o)) 
		    {
			reservation_table.reserve(i, o);
			if (GlobalParams::verbose_mode > VERBOSE_OFF) 
			{
			    cout << sc_time_stamp().to_double() / 1000 << ": Hub[" << local_id
				<< "], Port[" << i << "] (" << buffer[i].  Size() << " flits)" << ", reserved Output["
				<< o << "], flit: " << flit << endl;
			}
		    }
		}
	    }
	}
	start_from_port++;

	// 2nd phase: Forwarding
	for (int i = 0; i < MAX_HUB_PORTS; i++) 
	{
	    if (!buffer[i].IsEmpty()) 
	    {
		Flit flit = buffer[i].Front();

		int o = reservation_table.getOutputPort(i);
		if (o != NOT_RESERVED) 
		{
		    if (current_level_tx[o] == ack_tx[o].read()) 
		    {
			assert(false);
			cout << "(FAKE!!) Inject to RH: Router ID " << local_id << ", Type " << flit.flit_type << ", " << flit.src_id << "-->" << flit.dst_id << endl;
			cout << sc_time_stamp().to_double() / 1000 << ": Hub[" << local_id
			<< "], Port[" << i << "] forward to Output[" << o << "], flit: "
			<< flit << endl;

			/* TODO: put here the code that actually
			 * inject to TLM
			 * .........
			Inject(local_id, flit);
			*/


			flit_tx[o].write(flit);
			current_level_tx[o] = 1 - current_level_tx[o];
			req_tx[o].write(current_level_tx[o]);
			buffer[i].Pop();

			if (flit.flit_type == FLIT_TYPE_TAIL) reservation_table.release(o);

		    }
		}
	    }
	}
    }				// else reset read
}



