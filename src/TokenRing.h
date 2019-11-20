/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the processing element
 */

#ifndef __TOKENRING_H__
#define __TOKENRING_H__

#include <systemc.h>

#include "Utils.h"
#include <stdlib.h>

using namespace std;

SC_MODULE(TokenRing)
{
    SC_HAS_PROCESS(TokenRing);


    // I/O Ports
    sc_in_clk clock;	
    sc_in < bool > reset;

    map<int, sc_out<int>* > current_token_holder;
    map<int, sc_out<int>* > current_token_expiration;
    map<int, map<int,sc_inout<int>* > > flag;

    map<int, sc_signal<int>* > token_holder_signals;
    map<int, sc_signal<int>* > token_expiration_signals;
    map<int, map<int, sc_signal<int>* > > flag_signals;



    void attachHub(int channel, int hub, sc_in<int>* hub_token_holder_port, sc_in<int>* hub_token_expiration_port, sc_inout<int>* hub_flag_port);

    void updateTokens();

    TokenRing(sc_module_name nm): sc_module(nm) {


	if (GlobalParams::use_winoc)
	{
	    SC_METHOD(updateTokens);
	    sensitive << reset;
	    sensitive << clock.pos();
	}

        for (map<int, ChannelConfig>::iterator i = GlobalParams::channel_configuration.begin(); 
                i != GlobalParams::channel_configuration.end();
                ++i) {
            token_policy[i->first] = make_pair(i->second.macPolicy[0], i->second.macPolicy); 
        }
    }

    pair<string, vector<string> > getPolicy(int channel) { return token_policy[channel];}

    private:

    void updateTokenMaxHold(int channel);
    void updateTokenHold(int channel);
    void updateTokenPacket(int channel);

    // ring of a channel -> list of pairs < hubs , hold counts >
    map<int,vector<int> > rings_mapping;

    // ring of a channel -> token position in the ring
    map<int,int> token_position;
    
    map<int,int> token_hold_count;

    map<int,pair<string, vector<string> > >token_policy;

};

#endif
