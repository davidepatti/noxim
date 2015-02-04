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
    SC_HAS_PROCESS(TokenRing);

    int max_hold_cycles;
    int hold_count;

    // I/O Ports
    sc_in_clk clock;	
    sc_in < bool > reset;

    int currentTokenHolder(int channel);

    void attachHub(int channel, int hub);

    void updateTokens();

    TokenRing(sc_module_name nm): sc_module(nm) {
        SC_METHOD(updateTokens);
        sensitive << reset;
        sensitive << clock.pos();
    }

    private:

    // ring of a channel -> list of pairs < hubs , hold counts >
    map<int,vector<pair<int,int> > > rings_mapping;

    // ring of a channel -> token position in the ring
    map<int,int> ch_token_position;

};

#endif
