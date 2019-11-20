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
    struct Flit* my_flit = (struct Flit*)trans.get_data_ptr();

    LOG << "*** [Ch" <<local_id << "] Received: " << *my_flit << endl;

    // only moves received flit to the antenna buffer
    // reservations stuff is done in the hub to avoid 
    // race conditions on shared reservation table
    if (!buffer_rx.IsFull())
    {
        LOG << "*** [Ch" <<local_id << "] Flit " << *my_flit << " moved to buffer_rx " << endl;
        buffer_rx.Push(*my_flit);
        hub->power.antennaBufferPush();
        // Obliged to set response status to indicate successful completion
        trans.set_response_status( tlm::TLM_OK_RESPONSE );
        //buffer_rx.Print();
    }
    else
    {
        // the response status will remain ERRROR
        // signaling to the Initiator that something went wrong
        LOG << "[Ch" <<local_id << "] WARNING: buffer_rx is full cannot store flit " << *my_flit << endl;
    }
}

