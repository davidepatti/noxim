/*****************************************************************************

  TReservationTable.h -- Switch reservation table definition

 *****************************************************************************/

#ifndef __TRESERVATIONTABLE_H__
#define __TRESERVATIONTABLE_H__

//---------------------------------------------------------------------------

#include <cassert>

using namespace std;

//---------------------------------------------------------------------------

class TReservationTable
{
 public:

  TReservationTable();

  // Clear reservation table
  void clear();

  // Returns true if port_out is reserved (i.e., connected to an
  // input port)
  bool isReserved(const int port_out);

  // Connects port_in with port_out. Asserts if port_out is reserved
  void reserve(const int port_in, const int port_out);

  // Releases port_out connection. Asserts if port_out is not reserved
  void release(const int port_out);

  // Returns the output port connected to port_in. Asserts if port_out
  // is not reserved
  int getOutputPort(const int port_in);


private:
  
  vector<int> rtable; // reservation vector: rtable[i] gives the input
		      // port whose output port 'i' is connected to
};

//---------------------------------------------------------------------------

#endif
