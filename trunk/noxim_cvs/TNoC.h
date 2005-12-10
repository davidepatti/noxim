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

  sc_in_clk        clock;        // The input clock for the NoC
  sc_in<bool>      reset;        // The reset signal for the NoC

  // Signals

  // TODO by Fafa - this commented code should be used... (continues)

  /*
  sc_signal<bool>  req_to_east[][], req_to_west[][], req_to_south[][], req_to_north[][];
  sc_signal<bool>  ack_to_east[][], ack_to_west[][], ack_to_south[][], ack_to_north[][];

  sc_signal<TFlit> flit_to_east[][], flit_to_west[][], flit_to_south[][], flit_to_north[][];
  sc_signal<uint>  buffer_level_to_east[][], buffer_level_to_west[][], buffer_level_to_south[][], buffer_level_to_north[][];

  sc_signal<TFlit> flit_to_east[20][20], flit_to_west[20][20], flit_to_south[20][20], flit_to_north[20][20];
  sc_signal<uint>  buffer_level_to_east[20][20], buffer_level_to_west[20][20], buffer_level_to_south[20][20], buffer_level_to_north[20][20];

  // Matrix of tiles

  TTile*            t[][];

  */

  // TODO by Fafa - ...instead of this one
  sc_signal<bool>    req_to_east[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
  sc_signal<bool>    req_to_west[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
  sc_signal<bool>    req_to_south[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
  sc_signal<bool>    req_to_north[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];

  sc_signal<bool>    ack_to_east[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
  sc_signal<bool>    ack_to_west[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
  sc_signal<bool>    ack_to_south[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
  sc_signal<bool>    ack_to_north[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];

  sc_signal<TFlit>   flit_to_east[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
  sc_signal<TFlit>   flit_to_west[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
  sc_signal<TFlit>   flit_to_south[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
  sc_signal<TFlit>   flit_to_north[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];

  sc_signal<uint>    buffer_level_to_east[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
  sc_signal<uint>    buffer_level_to_west[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
  sc_signal<uint>    buffer_level_to_south[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
  sc_signal<uint>    buffer_level_to_north[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];

  // Matrix of tiles

  TTile*             t[MAX_STATIC_DIM][MAX_STATIC_DIM];


  // Constructor

  SC_CTOR(TNoC)
  {

    // TODO by Fafa - this commented code should be used... (continues)

    /*
    // Instance the matrixes of signals
    **req_to_east  = new sc_signal<bool>[TGlobalParams::mesh_dim_x+1][TGlobalParams::mesh_dim_y+1];
    **req_to_west  = new sc_signal<bool>[TGlobalParams::mesh_dim_x+1][TGlobalParams::mesh_dim_y+1];
    **req_to_south = new sc_signal<bool>[TGlobalParams::mesh_dim_x+1][TGlobalParams::mesh_dim_y+1];
    **req_to_north = new sc_signal<bool>[TGlobalParams::mesh_dim_x+1][TGlobalParams::mesh_dim_y+1];

    **ack_to_east  = new sc_signal<bool>[TGlobalParams::mesh_dim_x+1][TGlobalParams::mesh_dim_y+1];
    **ack_to_west  = new sc_signal<bool>[TGlobalParams::mesh_dim_x+1][TGlobalParams::mesh_dim_y+1];
    **ack_to_south = new sc_signal<bool>[TGlobalParams::mesh_dim_x+1][TGlobalParams::mesh_dim_y+1];
    **ack_to_north = new sc_signal<bool>[TGlobalParams::mesh_dim_x+1][TGlobalParams::mesh_dim_y+1];

    **flit_to_east  = new sc_signal<TFlit>[TGlobalParams::mesh_dim_x+1][TGlobalParams::mesh_dim_y+1];
    **flit_to_west  = new sc_signal<TFlit>[TGlobalParams::mesh_dim_x+1][TGlobalParams::mesh_dim_y+1];
    **flit_to_south = new sc_signal<TFlit>[TGlobalParams::mesh_dim_x+1][TGlobalParams::mesh_dim_y+1];
    **flit_to_north = new sc_signal<TFlit>[TGlobalParams::mesh_dim_x+1][TGlobalParams::mesh_dim_y+1];

    **buffer_level_to_east  = new sc_signal<uint>[TGlobalParams::mesh_dim_x+1][TGlobalParams::mesh_dim_y+1];
    **buffer_level_to_west  = new sc_signal<uint>[TGlobalParams::mesh_dim_x+1][TGlobalParams::mesh_dim_y+1];
    **buffer_level_to_south = new sc_signal<uint>[TGlobalParams::mesh_dim_x+1][TGlobalParams::mesh_dim_y+1];
    **buffer_level_to_north = new sc_signal<uint>[TGlobalParams::mesh_dim_x+1][TGlobalParams::mesh_dim_y+1];

    t = new TTile()[TGlobalParams::mesh_dim_x][TGlobalParams::mesh_dim_y];
    */

    // TODO by Fafa - ...instead of this one


    // Build the Mesh
    buildMesh();
  }

  // Support methods
  TTile* searchNode(const int id) const;

 private:
  void buildMesh();
};

//---------------------------------------------------------------------------

#endif

