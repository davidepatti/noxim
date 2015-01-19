/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the processing element
 */

#include "TokenRing.h"


void TokenRing::updateToken()
{
    if (reset.read()) {
	
    } else {

	hold_count--;
	if (hold_count==0)
	{
	    hold_count = max_hold_cycles;

	    current_token = (current_token+1)%num_hubs;

	    cout << name() << " ****** token assigned to " << current_token << endl;
	}
    }
}


void TokenRing::configure(int start_token,int _max_hold_cycles)
{
    current_token = start_token;
    max_hold_cycles = _max_hold_cycles;
    hold_count = max_hold_cycles;
}


int TokenRing::currentToken()
{
    return current_token;
}

