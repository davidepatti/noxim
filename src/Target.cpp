/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the buffer
 */
#include "Hub.h"
#include "Target.h"

void Target::b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
{
    // Obliged to implement read and write commands
    struct Flit* my_flit = (struct Flit*)trans.get_data_ptr();

    LOG << "*** Received: " << *my_flit << endl;

    if (!buffer_rx.IsFull())
    {
	int dst_port = hub->tile2Port(my_flit->dst_id);
	TReservation r;
	r.input = local_id; // i.e., channel id
	r.vc = my_flit->vc_id;

	if (my_flit->flit_type==FLIT_TYPE_HEAD)
	{
	    if (hub->antenna2tile_reservation_table.isNotReserved(dst_port))
	    {
		LOG << "Reserving output port " << dst_port << " by channel " << local_id << " for flit " << *my_flit << endl;
		hub->antenna2tile_reservation_table.reserve(r, dst_port);

		// The number of commucation using the wireless network, accounting also
		// partial wired path
		hub->wireless_communications_counter++;
	    }
	    else
	    {
		LOG << "WARNING: cannot reserve output port " << dst_port << " for channel " << local_id  << ", flit " << *my_flit << endl;
		return;
	    }
	}

	if (my_flit->flit_type == FLIT_TYPE_TAIL) 
	{
	    LOG << "Releasing reservation for output port " << dst_port << ", flit " << *my_flit << endl;
	    hub->antenna2tile_reservation_table.release(r,dst_port);
	}

	LOG << "Flit " << *my_flit << " moved to buffer_rx " << endl;
	buffer_rx.Push(*my_flit);
	hub->power.antennaBufferPush();
	// Obliged to set response status to indicate successful completion
	trans.set_response_status( tlm::TLM_OK_RESPONSE );
    }
    else
    {
	LOG << "WARNING: buffer_rx is full cannot store flit " << *my_flit << endl;
    }
    // FIXME: controlla commento in RadioProcess 

}

