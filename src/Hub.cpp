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
	    //cout << "TESTTTT " << ack_rx[i].size() << " i " << endl;
	    ack_rx[i]->write(0);
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
		if ((req_rx[i]->read() == 1 - current_level_rx[i]) && !buffer[i].IsFull()) 
		{
		    Flit received_flit = flit_rx[i]->read();

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
	for (int i = 0; i < MAX_HUB_PORTS; i++) 
	{
	    req_tx[i]->write(0);
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
		// fixed to first channel 
		int o = 0;

		if (flit.flit_type == FLIT_TYPE_HEAD) 
		{
		    if (reservation_table.isAvailable(o)) 
		    {
			reservation_table.reserve(i, o);
			if (GlobalParams::verbose_mode > VERBOSE_OFF) 
			{
			    cout << sc_time_stamp().to_double() / 1000 << ": Hub[" << local_id
				<< "], Port[" << i << "] (" << buffer[i].  Size() << " flits)" << ", WIFI Channel["
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
		    cout << name() << " inject to RH: Hub ID " << local_id << ", Type " << flit.flit_type << ", " << flit.src_id << "-->" << flit.dst_id << endl;
		    cout << name() << " port[" << i << "] forward to WIFI Channel[" << o << "], flit: "
			<< flit << endl;

		    init[0]->set_payload(flit);
		    init[0]->start_request_event.notify();

			//TODO: put here the code that actually
		    /*
		       if (current_level_tx[o] == ack_tx[o].read()) 
		       {

		     * inject to TLM
		     * .........
		     Inject(local_id, flit);



		     flit_tx[o].write(flit);
		     current_level_tx[o] = 1 - current_level_tx[o];
		     req_tx[o].write(current_level_tx[o]);
		     */
		    buffer[i].Pop();

		    if (flit.flit_type == FLIT_TYPE_TAIL) reservation_table.release(o);

		}

	    }
	} //if
    }				// for
}



