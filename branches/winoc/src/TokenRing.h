/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the processing element
 */

#ifndef __TOKENRING_H__
#define __TOKENRING_H__

#include <systemc.h>

#include "Utils.h"

using namespace std;

SC_MODULE(TokenRing)
{

    // I/O Ports
    sc_in_clk clock;	
    sc_in < bool > reset;

    int currentToken();

    // TURI FIX, file config
    void configure(int start_token,int _max_hold_cycles);

    void updateToken();

    // Constructor
    SC_CTOR(TokenRing) {
	SC_METHOD(updateToken);
	sensitive << reset;
	sensitive << clock.pos();

	// TURI FIX
	num_hubs = 4;
    }

    private:

    int current_token;
    int hold_count;
    int max_hold_cycles;

    int num_hubs;

   

};

#endif
