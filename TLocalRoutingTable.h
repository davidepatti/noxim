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
#ifndef __TLOCALROUTINGTABLE_H__
#define __TLOCALROUTINGTABLE_H__

//---------------------------------------------------------------------------

#include "TGlobalRoutingTable.h"

//---------------------------------------------------------------------------

class TLocalRoutingTable
{

public:

  TLocalRoutingTable();

  // Extracts the routing table of node _node_id from the global
  // routing table rtable
  void configure(TGlobalRoutingTable& rtable, const int _node_id);

  // Returns the set of admissible output channels for destination
  // destination_id and input channel in_link
  TAdmissibleOutputs getAdmissibleOutputs(const TLinkId& in_link,
					  const int      destination_id);

  // Returns the set of admissible output channels for a destination
  // destination_id and a given input direction
  TAdmissibleOutputs getAdmissibleOutputs(const int in_direction,
					  const int destination_id);


private:

  TRoutingTableNode rt_node;
  int               node_id;
};

//---------------------------------------------------------------------------

#endif
