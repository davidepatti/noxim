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
  
  if (TGlobalParams::routing_algorithm == RTABLE_BASED)
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

	  // NOPCAR 

	  t[i][j]->NOP_data_out[DIRECTION_NORTH](NOP_data_to_north[i][j]);
	  t[i][j]->NOP_data_out[DIRECTION_EAST](NOP_data_to_east[i+1][j]);
	  t[i][j]->NOP_data_out[DIRECTION_SOUTH](NOP_data_to_south[i][j+1]);
	  t[i][j]->NOP_data_out[DIRECTION_WEST](NOP_data_to_west[i][j]);

	  t[i][j]->NOP_data_in[DIRECTION_NORTH](NOP_data_to_south[i][j]);
	  t[i][j]->NOP_data_in[DIRECTION_EAST](NOP_data_to_west[i+1][j]);
	  t[i][j]->NOP_data_in[DIRECTION_SOUTH](NOP_data_to_north[i][j+1]);
	  t[i][j]->NOP_data_in[DIRECTION_WEST](NOP_data_to_east[i][j]);
	}
    }

  // Clear the inputs on the borders

  // dummy empty TNOP_data structure
  TNOP_data NOP_tmp;
  for (int i=0; i<DIRECTIONS; i++)
  {
      NOP_tmp.sender_id = INVALID_ID;
      NOP_tmp.buffer_level_neighbor[i] = 0;
  }

  for(int i=0; i<=TGlobalParams::mesh_dim_x; i++)
    {
      req_to_south[i][0] = 0;
      ack_to_north[i][0] = 0;
      req_to_north[i][TGlobalParams::mesh_dim_y] = 0;
      ack_to_south[i][TGlobalParams::mesh_dim_y] = 0;

      buffer_level_to_south[i][0] = 0;
      buffer_level_to_north[i][TGlobalParams::mesh_dim_y] = 0;

      /*
      // NOPCAR
      NOP_data_to_south[i][0].sender_id = INVALID_ID; 
      NOP_data_to_north[i][TGlobalParams::mesh_dim_y].sender_id = INVALID_ID;

      for(int k=0; k<DIRECTIONS; k++)
      {
	  NOP_data_to_south[i][0].buffer_level_neighbor[k] = 0;
	  NOP_data_to_north[i][TGlobalParams::mesh_dim_y].buffer_level_neighbor[k] = 0;
      }
      */
      NOP_data_to_south[i][0] = NOP_tmp;
      NOP_data_to_north[i][TGlobalParams::mesh_dim_y] = NOP_tmp;
    }
  for(int j=0; j<=TGlobalParams::mesh_dim_y; j++)
    {
      req_to_east[0][j] = 0;
      ack_to_west[0][j] = 0;
      req_to_west[TGlobalParams::mesh_dim_x][j] = 0;
      ack_to_east[TGlobalParams::mesh_dim_x][j] = 0;

      buffer_level_to_east[0][j] = 0;
      buffer_level_to_west[TGlobalParams::mesh_dim_x][j] = 0;

      // NOPCAR
      /*
      NOP_data_to_east[0][j].sender_id = INVALID_ID;
      NOP_data_to_west[TGlobalParams::mesh_dim_x][j].sender_id = INVALID_ID;
      for(int k=0; k<DIRECTIONS; k++)
      {
	  NOP_data_to_east[0][j].buffer_level_neighbor[k] = 0;
	  NOP_data_to_west[TGlobalParams::mesh_dim_x][j].buffer_level_neighbor[k] = 0;
      }
      */
      NOP_data_to_east[0][j] = NOP_tmp;
      NOP_data_to_west[TGlobalParams::mesh_dim_x][j] = NOP_tmp;

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
