/*****************************************************************************

  TPower.cpp -- Power model

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
#include <cassert>
#include "NoximDefs.h"
#include "TPower.h"

using namespace std;

// ---------------------------------------------------------------------------

TPower::TPower()
{
  pwr = 0.0;

  pwr_standby  = PWR_STANDBY;
  pwr_forward  = PWR_FORWARD_FLIT;

  pwr_incoming = PWR_INCOMING;
  if (TGlobalParams::buffer_depth == 4)
    pwr_incoming = PWR_FIFO4;
  else
    cerr << "Warning: Using power numbers for 4-flits FIFO" << endl;

  if (TGlobalParams::routing_algorithm == ROUTING_XY) pwr_routing = PWR_ROUTING_XY;
  else if (TGlobalParams::routing_algorithm == ROUTING_XY) pwr_routing = PWR_ROUTING_XY;
  else if (TGlobalParams::routing_algorithm == ROUTING_WEST_FIRST) pwr_routing = PWR_ROUTING_WEST_FIRST;
  else if (TGlobalParams::routing_algorithm == ROUTING_NORTH_LAST) pwr_routing = PWR_ROUTING_NORTH_LAST;
  else if (TGlobalParams::routing_algorithm == ROUTING_NEGATIVE_FIRST) pwr_routing = PWR_ROUTING_NEGATIVE_FIRST;
  else if (TGlobalParams::routing_algorithm == ROUTING_ODD_EVEN) pwr_routing = PWR_ROUTING_ODD_EVEN;
  else if (TGlobalParams::routing_algorithm == ROUTING_DYAD) pwr_routing = PWR_ROUTING_DYAD;
  else if (TGlobalParams::routing_algorithm == ROUTING_FULLY_ADAPTIVE) pwr_routing = PWR_ROUTING_FULLY_ADAPTIVE;
  else if (TGlobalParams::routing_algorithm == ROUTING_TABLE_BASED) pwr_routing = PWR_ROUTING_TABLE_BASED;
  else assert(false);

  if (TGlobalParams::selection_strategy == SEL_RANDOM) pwr_selection = PWR_SEL_RANDOM;
  else if (TGlobalParams::selection_strategy == SEL_BUFFER_LEVEL) pwr_selection = PWR_SEL_BUFFER_LEVEL;
  else if (TGlobalParams::selection_strategy == SEL_NOP) pwr_selection = PWR_SEL_NOP;
  else assert(false);

  if (TGlobalParams::in_order_packets_delivery_cam)
  {
    pwr_io = PWR_IO_CAM;
    switch (TGlobalParams::inorder_CAM_capacity)
    {
      case 2: 
	pwr_io = PWR_CAM2;
	break;
      case 4: 
	pwr_io = PWR_CAM4;
	break;
      case 8: 
	pwr_io = PWR_CAM8;
	break;
      case 16: 
	pwr_io = PWR_CAM16;
	break;
      default:
	cerr << "Warning: Using power numbers for 2-entries CAM" << endl;
    }
  }
  else if (TGlobalParams::in_order_packets_delivery_blocking)
    pwr_io = PWR_IO_BLOCKING;
  else
    pwr_io = 0.0;
}

// ---------------------------------------------------------------------------

void TPower::Routing()
{
  pwr += pwr_routing;
}

// ---------------------------------------------------------------------------

void TPower::Selection()
{
  pwr += pwr_selection;
}

// ---------------------------------------------------------------------------

void TPower::Standby()
{
  pwr += pwr_standby;
}

// ---------------------------------------------------------------------------

void TPower::Forward()
{
  pwr += pwr_forward;
}

// ---------------------------------------------------------------------------

void TPower::Incoming()
{
  pwr += pwr_incoming;
}

// ---------------------------------------------------------------------------

void TPower::InOrderStrategy()
{
  pwr += pwr_io;
}

// ---------------------------------------------------------------------------



