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


typedef struct Reservation
{
    int input;
    int vc;
} TReservation;

typedef struct RTEntry
{
    vector<TReservation> reservations;
    int index;
} TRTEntry;

class ReservationTable {
  public:

    ReservationTable();

    inline string name() const {return "ReservationTable";};

    // check if port_out is reservable
    bool isAvailable(const int port_in, const int vc, const int port_out);

    // Connects port_in with port_out. Asserts if port_out is reserved
    void reserve(const int port_in, const int vc, const int port_out);

    // Releases port_out connection. 
    // Asserts if port_out is not reserved or not valid
    void release(const int port_in, const int vc, const int port_out);

    // Returns the output port and virtual channel connected to port_in.
    void getReservation(const int port_int, int & port_out, int & vc);

    // update the index of the reservation having highest priority in the current cycle
    void updateIndex(const int port_out);

    // check whether port_out has no reservations
    bool isNotReserved(const int port_out);

  private:

     TRTEntry rtable[DIRECTIONS+2];	// reservation vector: rtable[i] gives a RTEntry containing the set of input/VC 
                                        // which reserved output port i
};

#endif
