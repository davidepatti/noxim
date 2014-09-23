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
using namespace std;

SC_MODULE(NoximHub)
{

    // I/O Ports
    sc_in_clk clock;		                // The input clock for the tile
    sc_in <bool> reset;	                        // The reset signal for the tile

    sc_out <NoximFlit> flit_rx[MAX_HUB_PORTS];	// The input channels
    sc_out <bool> req_rx[MAX_HUB_PORTS];	        // The requests associated with the input channels
    sc_in <bool> ack_rx[MAX_HUB_PORTS];	        // The outgoing ack signals associated with the input channels

    sc_in <NoximFlit> flit_tx[MAX_HUB_PORTS];	// The output channels
    sc_in <bool> req_tx[MAX_HUB_PORTS];	        // The requests associated with the output channels
    sc_in <bool> ack_tx[MAX_HUB_PORTS];	        // The outgoing ack signals associated with the output channels


    void setup();


    // Constructor

    SC_CTOR(NoximHub) {


    }

};

#endif
