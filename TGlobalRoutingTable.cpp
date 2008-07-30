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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cassert>
#include <fstream>
#include "NoximDefs.h"
#include "TGlobalRoutingTable.h"

using namespace std;

//---------------------------------------------------------------------------

TLinkId direction2ILinkId(const int node_id, const int dir)
{
  int node_src;

  switch (dir)
    {
    case DIRECTION_NORTH:
      node_src = node_id - TGlobalParams::mesh_dim_x;
      break;

    case DIRECTION_SOUTH:
      node_src = node_id + TGlobalParams::mesh_dim_x;
      break;

    case DIRECTION_EAST:
      node_src = node_id + 1;
      break;

    case DIRECTION_WEST:
      node_src = node_id - 1;
      break;

    case DIRECTION_LOCAL:
      node_src = node_id;
      break;

    default:
      assert(false);
    }


  return TLinkId(node_src, node_id);
}

//---------------------------------------------------------------------------

int oLinkId2Direction(const TLinkId& out_link)
{
  int src = out_link.first;
  int dst = out_link.second;

  if (dst == src)
    return DIRECTION_LOCAL;
  else if (dst == src+1)
    return DIRECTION_EAST;
  else if (dst == src-1)
    return DIRECTION_WEST;
  else if (dst == src - TGlobalParams::mesh_dim_x)
    return DIRECTION_NORTH;
  else if (dst == src + TGlobalParams::mesh_dim_x)
    return DIRECTION_SOUTH;
  else
    assert(false);
  return 0;  
}

//---------------------------------------------------------------------------

vector<int> admissibleOutputsSet2Vector(const TAdmissibleOutputs& ao)
{
  vector<int> dirs;

  for (TAdmissibleOutputs::iterator i=ao.begin(); i!=ao.end(); i++)
    dirs.push_back(oLinkId2Direction(*i));

  return dirs;
}

//---------------------------------------------------------------------------

#define COLUMN_AOC 22

//---------------------------------------------------------------------------

TGlobalRoutingTable::TGlobalRoutingTable()
{
  valid = false;
}

//---------------------------------------------------------------------------

bool TGlobalRoutingTable::load(const char* fname)
{
  ifstream fin(fname, ios::in);

  if (!fin)
    return false;
  
  rt_noc.clear();

  bool stop = false;
  while (!fin.eof() && !stop)
    {
      char line[128];
      fin.getline(line, sizeof(line)-1);

      if (line[0] == '\0')
	stop = true;
      else
	{
	  if (line[0] != '%')
	    {
	      int node_id, in_src, in_dst, dst_id, out_src, out_dst;
	  
	      if (sscanf(line+1, "%d %d->%d %d", &node_id, &in_src, &in_dst, &dst_id) == 4)
		{
		  TLinkId lin(in_src, in_dst);
		  
		  char *pstr = line+COLUMN_AOC;
		  while (sscanf(pstr, "%d->%d", &out_src, &out_dst) == 2)
		    {
		      TLinkId lout(out_src, out_dst);

		      rt_noc[node_id][lin][dst_id].insert(lout);
		      
		      pstr = strstr(pstr, ",");
		      pstr++;
		    }
		}
	    }
	}
    }

  valid = true;
  
  return true;
}

//---------------------------------------------------------------------------

// bool TGlobalRoutingTable::seek(const char* fname, const char* rt_label, ifstream& fin)
// {
//   fin.open(fname, ifstream::in);

//   if (!fin.is_open())
//     return false;

//   bool found = false;
//   while (!fin.eof() && !found)
//     {
//       char line[128];
//       fin.getline(line, sizeof(line)-1);

//       if (strstr(line, rt_label) != NULL)
// 	found = true;
//     }
  
//   return found;
// }

//---------------------------------------------------------------------------

TRoutingTableNode TGlobalRoutingTable::getNodeRoutingTable(const int node_id)
{
  return rt_noc[node_id];
}

//---------------------------------------------------------------------------

