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

#include <systemc.h>
#include "NoximMain.h"
#include "NoximBuffer.h"
#include "NoximReservationTable.h"
using namespace std;

SC_MODULE(NoximHub)
{

    // I/O Ports
    sc_in_clk clock;		                // The input clock for the tile
    sc_in <bool> reset;	                        // The reset signal for the tile

    sc_in <NoximFlit> flit_rx[MAX_HUB_PORTS];	// The input channels
    sc_in <bool> req_rx[MAX_HUB_PORTS];	        // The requests associated with the input channels
    sc_out <bool> ack_rx[MAX_HUB_PORTS];	        // The outgoing ack signals associated with the input channels

    sc_out <NoximFlit> flit_tx[MAX_HUB_PORTS];	// The output channels
    sc_out <bool> req_tx[MAX_HUB_PORTS];	        // The requests associated with the output channels
    sc_in <bool> ack_tx[MAX_HUB_PORTS];	        // The outgoing ack signals associated with the output channels


    int local_id;		                // Unique ID
    NoximBuffer buffer[MAX_HUB_PORTS];	        // Buffer for each port
    bool current_level_rx[MAX_HUB_PORTS];	// Current level for ABP
    bool current_level_tx[MAX_HUB_PORTS];	// Current level for ABP

    int start_from_port;	                // Port from which to start the reservation cycle

    // TODO: use different table or extend in order to support also
    // Hubs
    NoximReservationTable reservation_table;	// Switch reservation table

    void rxProcess();		// The receiving process
    void txProcess();		// The transmitting process



    void setup();


    // Constructor

    SC_CTOR(NoximHub) {
	SC_METHOD(rxProcess);
	sensitive << reset;
	sensitive << clock.pos();

	SC_METHOD(txProcess);
	sensitive << reset;
	sensitive << clock.pos();

    }

};

#endif
