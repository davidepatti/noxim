/*****************************************************************************

  TGlobalTrafficTable.h -- Global Traffic Table definition

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
#ifndef __TGLOBAL_TRAFFIC_TABLE_H__
#define __TGLOBAL_TRAFFIC_TABLE_H__

//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "NoximDefs.h"
using namespace std;

//---------------------------------------------------------------------------

// Structure used to store information into the table
struct TLocalTrafficLink
{
  int src;                    // ID of the source node (PE)
  int dst;                    // ID of the destination node (PE)
  float pir;                  // Packet Injection Rate for the link
  float por;                  // Probability Of Retransmission for the link
  int t_on;                   // Time (in cycles) at which activity begins
  int t_off;                  // Time (in cycles) at which activity ends
  int t_period;               // Period after which activity starts again
};

//---------------------------------------------------------------------------

class TGlobalTrafficTable
{

public:

  // The constructor by default sets valid to false
  TGlobalTrafficTable(); 

  // Load traffic table from file. Returns true if OK (and sets valid to true), false otherwise
  bool load(const char* fname);

  // Tell how many times the PE whose ID is specified is in table as source
  int occurrencesAsSource(const int id);

  // Return a random destination ID from the table given a source ID
  int randomDestinationGivenTheSource(const int src);

  // Return the PIR given a couple src/dst
  float getPirForTheSelectedLink(int src_id, int dst_id);

  // Return the POR given a couple src/dst
  float getPorForTheSelectedLink(int src_id, int dst_id);

  // Return t_on given a couple src/dst
  int getTonForTheSelectedLink(int src_id, int dst_id);

  // Return t_off given a couple src/dst
  int getToffForTheSelectedLink(int src_id, int dst_id);

  // Return t_period given a couple src/dst
  int getTperiodForTheSelectedLink(int src_id, int dst_id);

  // Tell if the current Traffic Table is valid
  bool isValid() { return valid; }

  // Give back the number of lines in the table
  int size() { return numberOfLines; }

private:

  vector<TLocalTrafficLink> traffic_table;
  bool valid;
  int numberOfLines;

};

//---------------------------------------------------------------------------

#endif
