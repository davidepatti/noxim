/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the switch reservation table
 */

#ifndef __NOXIMRESERVATIONTABLE_H__
#define __NOXIMRESERVATIONTABLE_H__

#include <cassert>
#include "DataStructs.h"
#include "Utils.h"

using namespace std;


struct TReservation
{
    int input;
    int vc;
    inline bool operator ==(const TReservation & r) const
    {
	return (r.input==input && r.vc == vc);
    }
};

typedef struct RTEntry
{
    vector<TReservation> reservations;
    vector<TReservation>::size_type index;
} TRTEntry;

class ReservationTable {
  public:

    ReservationTable();

    inline string name() const {return "ReservationTable";};

    // check if the input/vc/output is a
    int checkReservation(const TReservation r, const int port_out);

    // Connects port_in with port_out. Asserts if port_out is reserved
    void reserve(const TReservation r, const int port_out);

    // Releases port_out connection. 
    // Asserts if port_out is not reserved or not valid
    void release(const TReservation r, const int port_out);

    // Returns the pairs of output port and virtual channel reserved by port_in
    vector<pair<int,int> > getReservations(const int port_int);

    // update the index of the reservation having highest priority in the current cycle
    void updateIndex();

    // check whether port_out has no reservations
    bool isNotReserved(const int port_out);

    void setSize(const int n_outputs);

    void print();

  private:

     TRTEntry *rtable;	// reservation vector: rtable[i] gives a RTEntry containing the set of input/VC 
			// which reserved output port

     int n_outputs;
};

#endif
