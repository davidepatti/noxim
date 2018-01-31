/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the switch reservation table
 */

#include "ReservationTable.h"

ReservationTable::ReservationTable()
{
    for (int i=0;i<DIRECTIONS+2;i++)
    {
	rtable[i].index = 0;
	rtable[i].reservations.clear();
    }
}


bool ReservationTable::isAvailable(const int port_in, const int vc, const int port_out)
{
    for (int i=0;i<DIRECTIONS+2;i++)
    {
	if (i!=port_out)
	{
	    for (int j=0;j<rtable[i].reservations.size();j++)
		if (rtable[i].reservations[j].input==port_it) 
		    return false;
	}
    }

    for (int j=0;j<rtable[port_out].reservations.size();j++)
	if (rtable[port_out].reservations[j].vc == vc)
	    return false;
    
    return true;
}

void ReservationTable::reserve(const int port_in, const int vc, const int port_out)
{
    // reservation of reserved/not valid ports is illegal. Correctness
    // should be assured by ReservationTable users
    assert(isAvailable(port_in,vc, port_out));

    TReservation r;
    r.input = port_in;
    r.vc = vc;

    // TODO: a better policy could insert in a specific position as far a possible
    // from the current index
    rtable[port_out].reservations.push_back(r);

}

void ReservationTable::release(const int port_in, const int vc, const int port_out)
{
    for (vector<TReservation>::iterator i=rtable[port_out].reservations.begin(); 
	    i != rtable[port_out].reservations.end(); i++)
    {
	if (i->input == port_in && i->vc == vc)
	{
	    rtable[port_out].reservations.erase(i);
	    if (
	    return;
	}
    }

}

int ReservationTable::getOutputPort(const int port_in)
{

    for (map<int,int>::iterator i=rtable.begin(); i!=rtable.end(); i++)
    {
	if (i->second == port_in)
	    return i->first;		// port_in reserved outport i
    }

    // semantic: port_in currently doesn't reserve any out port
    return NOT_RESERVED;
}

// makes port_out no longer available for reservation/release
void ReservationTable::invalidate(const int port_out)
{
    rtable[port_out] = NOT_VALID;
}
