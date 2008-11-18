/*****************************************************************************

  TNoC.cpp -- Network-on-Chip (NoC) implementation

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
#include "TNoC.h"
#include "TGlobalRoutingTable.h"
#include "TGlobalTrafficTable.h"

//---------------------------------------------------------------------------

void TNoC::buildMesh()
{
  // Check for routing table availability
  if (TGlobalParams::routing_algorithm == ROUTING_TABLE_BASED)
    assert(grtable.load(TGlobalParams::routing_table_filename));

  // Check for traffic table availability
  if (TGlobalParams::traffic_distribution == TRAFFIC_TABLE_BASED)
    assert(gttable.load(TGlobalParams::traffic_table_filename));

  // Create the mesh as a matrix of tiles
  for(int i=0; i<TGlobalParams::mesh_dim_x; i++)
    {
      for(int j=0; j<TGlobalParams::mesh_dim_y; j++)
	{
	  // Create the single Tile with a proper name
	  char tile_name[20];
	  sprintf(tile_name, "Tile[%02d][%02d]", i, j);
	  t[i][j] = new TTile(tile_name);

	  // Tell to the router its coordinates
	  t[i][j]->r->configure(j * TGlobalParams::mesh_dim_x + i,
				TGlobalParams::stats_warm_up_time,
				TGlobalParams::buffer_depth,
				grtable);

	  // Tell to the PE its coordinates
	  t[i][j]->pe->local_id = j * TGlobalParams::mesh_dim_x + i;
          t[i][j]->pe->traffic_table = &gttable;  // Needed to choose destination
          t[i][j]->pe->never_transmit = (gttable.occurrencesAsSource(t[i][j]->pe->local_id) == 0);

	  // Map clock and reset
	  t[i][j]->clock(clock);
	  t[i][j]->reset(reset);

	  // Map Rx signals
	  t[i][j]->req_rx[DIRECTION_NORTH](req_to_south[i][j]);
	  t[i][j]->flit_rx[DIRECTION_NORTH](flit_to_south[i][j]);
	  t[i][j]->ack_rx[DIRECTION_NORTH](ack_to_north[i][j]);

	  t[i][j]->req_rx[DIRECTION_EAST](req_to_west[i+1][j]);
	  t[i][j]->flit_rx[DIRECTION_EAST](flit_to_west[i+1][j]);
	  t[i][j]->ack_rx[DIRECTION_EAST](ack_to_east[i+1][j]);

	  t[i][j]->req_rx[DIRECTION_SOUTH](req_to_north[i][j+1]);
	  t[i][j]->flit_rx[DIRECTION_SOUTH](flit_to_north[i][j+1]);
	  t[i][j]->ack_rx[DIRECTION_SOUTH](ack_to_south[i][j+1]);

	  t[i][j]->req_rx[DIRECTION_WEST](req_to_east[i][j]);
	  t[i][j]->flit_rx[DIRECTION_WEST](flit_to_east[i][j]);
	  t[i][j]->ack_rx[DIRECTION_WEST](ack_to_west[i][j]);

	  // Map Tx signals
	  t[i][j]->req_tx[DIRECTION_NORTH](req_to_north[i][j]);
	  t[i][j]->flit_tx[DIRECTION_NORTH](flit_to_north[i][j]);
	  t[i][j]->ack_tx[DIRECTION_NORTH](ack_to_south[i][j]);

	  t[i][j]->req_tx[DIRECTION_EAST](req_to_east[i+1][j]);
	  t[i][j]->flit_tx[DIRECTION_EAST](flit_to_east[i+1][j]);
	  t[i][j]->ack_tx[DIRECTION_EAST](ack_to_west[i+1][j]);

	  t[i][j]->req_tx[DIRECTION_SOUTH](req_to_south[i][j+1]);
	  t[i][j]->flit_tx[DIRECTION_SOUTH](flit_to_south[i][j+1]);
	  t[i][j]->ack_tx[DIRECTION_SOUTH](ack_to_north[i][j+1]);

	  t[i][j]->req_tx[DIRECTION_WEST](req_to_west[i][j]);
	  t[i][j]->flit_tx[DIRECTION_WEST](flit_to_west[i][j]);
	  t[i][j]->ack_tx[DIRECTION_WEST](ack_to_east[i][j]);

	  // Map buffer level signals (analogy with req_tx/rx port mapping)
	  t[i][j]->free_slots[DIRECTION_NORTH](free_slots_to_north[i][j]);
	  t[i][j]->free_slots[DIRECTION_EAST](free_slots_to_east[i+1][j]);
	  t[i][j]->free_slots[DIRECTION_SOUTH](free_slots_to_south[i][j+1]);
	  t[i][j]->free_slots[DIRECTION_WEST](free_slots_to_west[i][j]);

	  t[i][j]->free_slots_neighbor[DIRECTION_NORTH](free_slots_to_south[i][j]);
	  t[i][j]->free_slots_neighbor[DIRECTION_EAST](free_slots_to_west[i+1][j]);
	  t[i][j]->free_slots_neighbor[DIRECTION_SOUTH](free_slots_to_north[i][j+1]);
	  t[i][j]->free_slots_neighbor[DIRECTION_WEST](free_slots_to_east[i][j]);

	  // NoP 

	  t[i][j]->NoP_data_out[DIRECTION_NORTH](NoP_data_to_north[i][j]);
	  t[i][j]->NoP_data_out[DIRECTION_EAST](NoP_data_to_east[i+1][j]);
	  t[i][j]->NoP_data_out[DIRECTION_SOUTH](NoP_data_to_south[i][j+1]);
	  t[i][j]->NoP_data_out[DIRECTION_WEST](NoP_data_to_west[i][j]);

	  t[i][j]->NoP_data_in[DIRECTION_NORTH](NoP_data_to_south[i][j]);
	  t[i][j]->NoP_data_in[DIRECTION_EAST](NoP_data_to_west[i+1][j]);
	  t[i][j]->NoP_data_in[DIRECTION_SOUTH](NoP_data_to_north[i][j+1]);
	  t[i][j]->NoP_data_in[DIRECTION_WEST](NoP_data_to_east[i][j]);
	}
    }


  // dummy TNoP_data structure
  TNoP_data tmp_NoP;

  tmp_NoP.sender_id = NOT_VALID;

  for (int i=0; i<DIRECTIONS; i++)
  {
      tmp_NoP.channel_status_neighbor[i].free_slots = NOT_VALID;
      tmp_NoP.channel_status_neighbor[i].available = false;
  }

  // Clear signals for borderline nodes
  for(int i=0; i<=TGlobalParams::mesh_dim_x; i++)
    {
      req_to_south[i][0] = 0;
      ack_to_north[i][0] = 0;
      req_to_north[i][TGlobalParams::mesh_dim_y] = 0;
      ack_to_south[i][TGlobalParams::mesh_dim_y] = 0;

      free_slots_to_south[i][0].write(NOT_VALID);
      free_slots_to_north[i][TGlobalParams::mesh_dim_y].write(NOT_VALID);

      NoP_data_to_south[i][0].write(tmp_NoP);
      NoP_data_to_north[i][TGlobalParams::mesh_dim_y].write(tmp_NoP);

    }

  for(int j=0; j<=TGlobalParams::mesh_dim_y; j++)
    {
      req_to_east[0][j] = 0;
      ack_to_west[0][j] = 0;
      req_to_west[TGlobalParams::mesh_dim_x][j] = 0;
      ack_to_east[TGlobalParams::mesh_dim_x][j] = 0;

      free_slots_to_east[0][j].write(NOT_VALID);
      free_slots_to_west[TGlobalParams::mesh_dim_x][j].write(NOT_VALID);

      NoP_data_to_east[0][j].write(tmp_NoP);
      NoP_data_to_west[TGlobalParams::mesh_dim_x][j].write(tmp_NoP);

    }

  // invalidate reservation table entries for non-exhistent channels
  for(int i=0; i<TGlobalParams::mesh_dim_x; i++)
    {
      t[i][0]->r->reservation_table.invalidate(DIRECTION_NORTH);
      t[i][TGlobalParams::mesh_dim_y-1]->r->reservation_table.invalidate(DIRECTION_SOUTH);
    }
  for(int j=0; j<TGlobalParams::mesh_dim_y; j++)
    {
      t[0][j]->r->reservation_table.invalidate(DIRECTION_WEST);
      t[TGlobalParams::mesh_dim_x-1][j]->r->reservation_table.invalidate(DIRECTION_EAST);
    }

}

//---------------------------------------------------------------------------

TTile* TNoC::searchNode(const int id) const
{
  for (int i=0; i<TGlobalParams::mesh_dim_x; i++)
    for (int j=0; j<TGlobalParams::mesh_dim_y; j++)
      if (t[i][j]->r->local_id == id)
	return t[i][j];

  return false;
}

//---------------------------------------------------------------------------

