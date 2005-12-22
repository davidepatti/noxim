/*****************************************************************************

  TNoC.cpp -- Network-on-Chip (NoC) implementation

 *****************************************************************************/

#include "TNoC.h"
#include "TGlobalRoutingTable.h"

//---------------------------------------------------------------------------


void TNoC::buildMesh()
{
  // Check for routing table availability
  TGlobalRoutingTable grtable;
  
  if (TGlobalParams::routing_algorithm == ROUTING_RTABLE_BASED)
    assert(grtable.load(TGlobalParams::rtable_filename));

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
				grtable);

	  // Tell to the PE its coordinates
	  t[i][j]->pe->id = j * TGlobalParams::mesh_dim_x + i;

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
	  t[i][j]->buffer_level[DIRECTION_NORTH](buffer_level_to_north[i][j]);
	  t[i][j]->buffer_level[DIRECTION_EAST](buffer_level_to_east[i+1][j]);
	  t[i][j]->buffer_level[DIRECTION_SOUTH](buffer_level_to_south[i][j+1]);
	  t[i][j]->buffer_level[DIRECTION_WEST](buffer_level_to_west[i][j]);

	  t[i][j]->buffer_level_neighbor[DIRECTION_NORTH](buffer_level_to_south[i][j]);
	  t[i][j]->buffer_level_neighbor[DIRECTION_EAST](buffer_level_to_west[i+1][j]);
	  t[i][j]->buffer_level_neighbor[DIRECTION_SOUTH](buffer_level_to_north[i][j+1]);
	  t[i][j]->buffer_level_neighbor[DIRECTION_WEST](buffer_level_to_east[i][j]);

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

  // Clear the inputs on the borders

  // dummy empty TNoP_data structure
  TNoP_data NoP_tmp;
  for (int i=0; i<DIRECTIONS; i++)
  {
      NoP_tmp.sender_id = INVALID_ID;
      NoP_tmp.buffer_level_neighbor[i] = 0;
  }

  for(int i=0; i<=TGlobalParams::mesh_dim_x; i++)
    {
      req_to_south[i][0] = 0;
      ack_to_north[i][0] = 0;
      req_to_north[i][TGlobalParams::mesh_dim_y] = 0;
      ack_to_south[i][TGlobalParams::mesh_dim_y] = 0;

      buffer_level_to_south[i][0] = 0;
      buffer_level_to_north[i][TGlobalParams::mesh_dim_y] = 0;

      NoP_data_to_south[i][0] = NoP_tmp;
      NoP_data_to_north[i][TGlobalParams::mesh_dim_y] = NoP_tmp;
    }
  for(int j=0; j<=TGlobalParams::mesh_dim_y; j++)
    {
      req_to_east[0][j] = 0;
      ack_to_west[0][j] = 0;
      req_to_west[TGlobalParams::mesh_dim_x][j] = 0;
      ack_to_east[TGlobalParams::mesh_dim_x][j] = 0;

      buffer_level_to_east[0][j] = 0;
      buffer_level_to_west[TGlobalParams::mesh_dim_x][j] = 0;

      NoP_data_to_east[0][j] = NoP_tmp;
      NoP_data_to_west[TGlobalParams::mesh_dim_x][j] = NoP_tmp;

    }
}

//---------------------------------------------------------------------------

TTile* TNoC::searchNode(const int id) const
{
  for (int i=0; i<TGlobalParams::mesh_dim_x; i++)
    for (int j=0; j<TGlobalParams::mesh_dim_y; j++)
      if (t[i][j]->r->id == id)
	return t[i][j];

  return false;
}

//---------------------------------------------------------------------------
