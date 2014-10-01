/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the tile
 */

#ifndef __NOXIMHUB_H__
#define __NOXIMHUB_H__

// Needed for the simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"

#include <systemc.h>
#include "Main.h"
#include "Buffer.h"
#include "ReservationTable.h"

#include "Initiator.h"
#include "Target.h"


SC_MODULE(Hub)
{

    // I/O Ports
    sc_in_clk clock;		                // The input clock for the tile
    sc_in <bool> reset;	                        // The reset signal for the tile

    sc_in <Flit> flit_rx[MAX_HUB_PORTS];	// The input channels
    sc_in <bool> req_rx[MAX_HUB_PORTS];	        // The requests associated with the input channels
    sc_out <bool> ack_rx[MAX_HUB_PORTS];	        // The outgoing ack signals associated with the input channels

    sc_out <Flit> flit_tx[MAX_HUB_PORTS];	// The output channels
    sc_out <bool> req_tx[MAX_HUB_PORTS];	        // The requests associated with the output channels
    sc_in <bool> ack_tx[MAX_HUB_PORTS];	        // The outgoing ack signals associated with the output channels


    int local_id;		                // Unique ID
    Buffer buffer[MAX_HUB_PORTS];	        // Buffer for each port
    bool current_level_rx[MAX_HUB_PORTS];	// Current level for ABP
    bool current_level_tx[MAX_HUB_PORTS];	// Current level for ABP

    Initiator* init[MAX_HUB_CHANNELS];
    Target* target[MAX_HUB_CHANNELS];

    int start_from_port;	                // Port from which to start the reservation cycle

    // TODO: use different table or extend in order to support also
    // Hubs
    ReservationTable reservation_table;	// Switch reservation table

    void rxProcess();		// The receiving process
    void txProcess();		// The transmitting process

    void setup();

    // Constructor

    SC_CTOR(Hub) 
    {

	SC_METHOD(rxProcess);
	sensitive << reset;
	sensitive << clock.pos();

	SC_METHOD(txProcess);
	sensitive << reset;
	sensitive << clock.pos();

	for (int i = 0; i < MAX_HUB_CHANNELS; i++)
	{
	    char txt[20];
	    sprintf(txt, "init_%d", i);
	    init[i] = new Initiator(txt);
	    // actual bus binding in buildMesh()
	    //init[i]->socket.bind( bus->targ_socket );
	}

	for (int i = 0; i < MAX_HUB_CHANNELS; i++)
	{
	    char txt[20];
	    sprintf(txt, "target_%d", i);
	    target[i] = new Target(txt);
	    // actual bus binding in buildMesh()
	    // bus->init_socket.bind( target[i]->socket );
	}

    }

};

#endif
