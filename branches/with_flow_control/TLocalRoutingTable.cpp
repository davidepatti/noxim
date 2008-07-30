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
#include "TLocalRoutingTable.h"
#include "NoximDefs.h"

//---------------------------------------------------------------------------

TLocalRoutingTable::TLocalRoutingTable()
{
}

//---------------------------------------------------------------------------

void TLocalRoutingTable::configure(TGlobalRoutingTable& rtable, 
				   const int _node_id)
{
  rt_node = rtable.getNodeRoutingTable(_node_id);
  node_id = _node_id;
}

//---------------------------------------------------------------------------

TAdmissibleOutputs TLocalRoutingTable::getAdmissibleOutputs(const TLinkId& in_link,
							    const int      destination_id)
{
  return rt_node[in_link][destination_id];
}

//---------------------------------------------------------------------------

TAdmissibleOutputs TLocalRoutingTable::getAdmissibleOutputs(const int in_direction,
							    const int destination_id)
{
  TLinkId lid = direction2ILinkId(node_id, in_direction);

  return getAdmissibleOutputs(lid, destination_id);
}

//---------------------------------------------------------------------------
