/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the switch reservation table
 */

#include "ReservationTable.h"

ReservationTable::ReservationTable()
{
  rtable.clear();
}


void ReservationTable::init(int size)
{
    cout << "Initializing reservation table with size " << size << endl;
    assert(size>0);
    assert(directions()==0);

    rtable.clear();

    for (int i=0;i<size;i++)
	rtable.push_back(NOT_RESERVED); 

}

int ReservationTable::directions() const
{
    return rtable.size();
}

bool ReservationTable::isAvailable(const int port_out) const
{
    cout << " checking availability of out port " << port_out << endl;
    assert(port_out >= 0 && port_out < directions());

    return ((rtable[port_out] == NOT_RESERVED));
}

void ReservationTable::reserve(const int port_in, const int port_out)
{
    // reservation of reserved/not valid ports is illegal. Correctness
    // should be assured by ReservationTable users
    assert(isAvailable(port_out));

    // check for previous reservation to be released
    int port = getOutputPort(port_in);

    if (port != NOT_RESERVED)
	release(port);

    rtable[port_out] = port_in;
}

void ReservationTable::release(const int port_out)
{
    assert(port_out >= 0 && port_out < directions());
    // there is a valid reservation on port_out
    assert(rtable[port_out] >= 0);

    rtable[port_out] = NOT_RESERVED;
}

int ReservationTable::getOutputPort(const int port_in) const
{
//    cout << " getOutputPort invoked with port_in = " << port_in << endl;

    for (int i = 0; i < directions(); i++)
	if (rtable[i] == port_in)
	    return i;		// port_in reserved outport i

    // semantic: port_in currently doesn't reserve any out port
    return NOT_RESERVED;
}

// makes port_out no longer available for reservation/release
void ReservationTable::invalidate(const int port_out)
{
    assert(port_out < directions());
    rtable[port_out] = NOT_VALID;
}
