/*****************************************************************************

  TNoC.h -- Network-on-Chip (NoC) definition

 *****************************************************************************/

#ifndef __TNOC_H__
#define __TNOC_H__

//---------------------------------------------------------------------------

#include <systemc.h>
#include "TTile.h"

SC_MODULE(TNoC)
{

  // I/O Ports

  sc_in_clk          clock;        // The input clock for the NoC
  sc_in<bool>        reset;        // The reset signal for the NoC

  // Signals

  sc_signal<bool>    req_to_east[MESH_DIM_X+1][MESH_DIM_Y+1];
  sc_signal<bool>    req_to_west[MESH_DIM_X+1][MESH_DIM_Y+1];
  sc_signal<bool>    req_to_south[MESH_DIM_X+1][MESH_DIM_Y+1];
  sc_signal<bool>    req_to_north[MESH_DIM_X+1][MESH_DIM_Y+1];

  sc_signal<bool>    ack_to_east[MESH_DIM_X+1][MESH_DIM_Y+1];
  sc_signal<bool>    ack_to_west[MESH_DIM_X+1][MESH_DIM_Y+1];
  sc_signal<bool>    ack_to_south[MESH_DIM_X+1][MESH_DIM_Y+1];
  sc_signal<bool>    ack_to_north[MESH_DIM_X+1][MESH_DIM_Y+1];

  sc_signal<TFlit>   flit_to_east[MESH_DIM_X+1][MESH_DIM_Y+1];
  sc_signal<TFlit>   flit_to_west[MESH_DIM_X+1][MESH_DIM_Y+1];
  sc_signal<TFlit>   flit_to_south[MESH_DIM_X+1][MESH_DIM_Y+1];
  sc_signal<TFlit>   flit_to_north[MESH_DIM_X+1][MESH_DIM_Y+1];

  sc_signal<uint>    buffer_level_to_east[MESH_DIM_X+1][MESH_DIM_Y+1];
  sc_signal<uint>    buffer_level_to_west[MESH_DIM_X+1][MESH_DIM_Y+1];
  sc_signal<uint>    buffer_level_to_south[MESH_DIM_X+1][MESH_DIM_Y+1];
  sc_signal<uint>    buffer_level_to_north[MESH_DIM_X+1][MESH_DIM_Y+1];

  // Matrix of tiles

  TTile*             t[MESH_DIM_X][MESH_DIM_Y];

  // Constructor

  SC_CTOR(TNoC)
  {
    buildMesh();
  }

  // Support methods
  TTile* searchNode(const int id) const;

 private:
  void buildMesh();
};

//---------------------------------------------------------------------------

#endif
