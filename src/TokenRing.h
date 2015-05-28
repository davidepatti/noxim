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


    // I/O Ports
    sc_in_clk clock;	
    sc_in < bool > reset;

    map<int, sc_out<int>* > current_token_holder;
    map<int, sc_out<int>* > current_token_expiration;
    map<int, map<int,sc_in<int>* > > flag;

    map<int, sc_signal<int>* > token_holder_signals;
    map<int, sc_signal<int>* > token_expiration_signals;
    map<int, map<int, sc_signal<int>* > > flag_signals;



    void attachHub(int channel, int hub, sc_in<int>* hub_token_holder_port, sc_in<int>* hub_token_expiration_port, sc_out<int>* hub_flag_port);

    void updateTokens();

    TokenRing(sc_module_name nm): sc_module(nm) {
        SC_METHOD(updateTokens);
        sensitive << reset;
        sensitive << clock.pos();
	// TODO TURI: policy hardwired
	token_policy[0] = TOKEN_MAX_HOLD;
	//token_policy[0] = TOKEN_HOLD;
	//token_policy[0] = TOKEN_PACKET;

    }

    int getPolicy(int channel) { return token_policy[channel];}

    private:

    void updateTokenMaxHold(int channel);
    void updateTokenHold(int channel);
    void updateTokenPacket(int channel);

    // ring of a channel -> list of pairs < hubs , hold counts >
    map<int,vector<int> > rings_mapping;

    // ring of a channel -> token position in the ring
    map<int,int> token_position;
    
    map<int,int> token_hold_count;

    map<int,int> token_policy;

};

#endif
