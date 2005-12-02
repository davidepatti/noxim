#include "NoximDefs.h"
#include "TNoC.h"

//---------------------------------------------------------------------------

void TNoC::buildMesh()
{
  // Create the mesh as a matrix of tiles
  for(int i=0; i<MESH_DIM_X; i++)
    {
      for(int j=0; j<MESH_DIM_Y; j++)
	{
	  // Create the single Tile with a proper name
	  char tile_name[20];
	  sprintf(tile_name, "Tile[%02d][%02d]", i, j);
	  t[i][j] = new TTile(tile_name);

	  // Tell to the router its coordinates
	  t[i][j]->r->configure(j * MESH_DIM_X + i,
				ODD_EVEN);

	  // Tell to the PE its coordinates
	  t[i][j]->pe->id = j * MESH_DIM_X + i;

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

	}
    }

  // Clear the inputs on the borders
  for(int i=0; i<=MESH_DIM_X; i++)
    {
      req_to_south[i][0] = 0;
      ack_to_north[i][0] = 0;
      req_to_north[i][MESH_DIM_Y] = 0;
      ack_to_south[i][MESH_DIM_Y] = 0;
    }
  for(int j=0; j<=MESH_DIM_Y; j++)
    {
      req_to_east[0][j] = 0;
      ack_to_west[0][j] = 0;
      req_to_west[MESH_DIM_X][j] = 0;
      ack_to_east[MESH_DIM_X][j] = 0;
    }
}

//---------------------------------------------------------------------------

TTile* TNoC::searchNode(const int id) const
{
  for (int i=0; i<MESH_DIM_X; i++)
    for (int j=0; j<MESH_DIM_Y; j++)
      if (t[i][j]->r->id == id)
	return t[i][j];

  return false;
}

//---------------------------------------------------------------------------
