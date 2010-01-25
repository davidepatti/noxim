/*****************************************************************************

  TReservationTable.h -- Switch reservation table definition

 *****************************************************************************/
/* Copyright 2005-2007  
    Fabrizio Fazzino <fabrizio.fazzino@diit.unict.it>
    Maurizio Palesi <mpalesi@diit.unict.it>
    Davide Patti <dpatti@diit.unict.it>

 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
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

  // check if port_out is reservable
  bool isAvailable(const int port_out) const;

  // Connects port_in with port_out. Asserts if port_out is reserved
  void reserve(const int port_in, const int port_out);

  // Releases port_out connection. 
  // Asserts if port_out is not reserved or not valid
  void release(const int port_out);

  // Returns the output port connected to port_in.
  int getOutputPort(const int port_in) const;

  // Makes output port no longer available for reservation/release
  void invalidate(const int port_out);

private:
  
  vector<int> rtable; // reservation vector: rtable[i] gives the input
		      // port whose output port 'i' is connected to
};

//---------------------------------------------------------------------------

#endif
