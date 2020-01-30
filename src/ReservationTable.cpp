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
}

void ReservationTable::setSize(const int n_outputs)
{
    this->n_outputs = n_outputs;
    rtable = new TRTEntry[this->n_outputs];

    for (int i=0;i<this->n_outputs;i++)
    {
	rtable[i].index = 0;
	rtable[i].reservations.clear();
    }
}

bool ReservationTable::isNotReserved(const int port_out)
{
    assert(port_out<n_outputs);
    return (rtable[port_out].reservations.size()==0);
}

/* For a given input, returns the set of output/vc reserved from that input.
 * An index is required for each output entry, to avoid that multiple invokations
 * with different inputs returns the same output in the same clock cycle. */
vector<pair<int,int> > ReservationTable::getReservations(const int port_in)
{
    vector<pair<int,int> > reservations;

    for (int o = 0;o<n_outputs;o++)
    {
	if (rtable[o].reservations.size()>0)
	{
	    int current_index = rtable[o].index;
	    if (rtable[o].reservations[current_index].input == port_in)
		reservations.push_back(pair<int,int>(o,rtable[o].reservations[current_index].vc));
	}
    }
    return reservations;
}

int ReservationTable::checkReservation(const TReservation r, const int port_out)
{
    /* Sanity Check for forbidden table status:
     * - same input/VC in a different output line */
    for (int o=0;o<n_outputs;o++)
    {
	for (vector<TReservation>::size_type i=0;i<rtable[o].reservations.size(); i++)
	{
	    // In the current implementation this should never happen
	    if (o!=port_out && rtable[o].reservations[i] == r)
	    {
		return RT_ALREADY_OTHER_OUT;
	    }
	}
    }
    
     /* On a given output entry, reservations must differ by VC
     *  Motivation: they will be interleaved cycle-by-cycle as index moves */

     int n_reservations = rtable[port_out].reservations.size();
    for (int i=0;i< n_reservations; i++)
    {
	// the reservation is already present
	if (rtable[port_out].reservations[i] == r)
	    return RT_ALREADY_SAME;

	// the same VC for that output has been reserved by another input
	if (rtable[port_out].reservations[i].input != r.input &&
	    rtable[port_out].reservations[i].vc == r.vc)
	    return RT_OUTVC_BUSY;
    }
    return RT_AVAILABLE;
}

void ReservationTable::print()
{
    for (int o=0;o<n_outputs;o++)
    {
	cout << o << ": ";
	for (vector<TReservation>::size_type i=0;i<rtable[o].reservations.size();i++)
	{
	    cout << "<" << rtable[o].reservations[i].input << "," << rtable[o].reservations[i].vc << ">, ";
	}
	cout << " | " << rtable[o].index;
	cout << endl;
    }
}


void ReservationTable::reserve(const TReservation r, const int port_out)
{
    // IMPORTANT: problem when used by Hub with more connections
    //
    // reservation of reserved/not valid ports is illegal. Correctness
    // should be assured by ReservationTable users
    assert(checkReservation(r, port_out)==RT_AVAILABLE);

    // TODO: a better policy could insert in a specific position as far a possible
    // from the current index
    rtable[port_out].reservations.push_back(r);

}

void ReservationTable::release(const TReservation r, const int port_out)
{
    assert(port_out < n_outputs);

    for (vector<TReservation>::iterator i=rtable[port_out].reservations.begin(); 
	    i != rtable[port_out].reservations.end(); i++)
    {
	if (*i == r)
	{
	    rtable[port_out].reservations.erase(i);
	    vector<TReservation>::size_type removed_index = i - rtable[port_out].reservations.begin();

	    if (removed_index < rtable[port_out].index)
		rtable[port_out].index--;
	    else
		if (rtable[port_out].index >= rtable[port_out].reservations.size())
		    rtable[port_out].index = 0;

	    return;
	}
    }
    assert(false); //trying to release a never made reservation  ?
}

void ReservationTable::updateIndex()
{
    for (int o=0;o<n_outputs;o++)
    {
	if (rtable[o].reservations.size()>0)
	    rtable[o].index = (rtable[o].index+1)%(rtable[o].reservations.size());
    }
}


