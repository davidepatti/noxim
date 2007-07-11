/*****************************************************************************

  TNoC.h -- Network-on-Chip (NoC) definition

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
  sc_signal<uint>  buffer_status_to_east[][], buffer_status_to_west[][], buffer_status_to_south[][], buffer_status_to_north[][];

  sc_signal<TFlit> flit_to_east[20][20], flit_to_west[20][20], flit_to_south[20][20], flit_to_north[20][20];
  sc_signal<uint>  buffer_status_to_east[20][20], buffer_status_to_west[20][20], buffer_status_to_south[20][20], buffer_status_to_north[20][20];

  // Matrix of tiles

  TTile*            t[][];

  */

  // TODO by Fafa - ...instead of this one
  // TODO:Fabrizio quando sostituirai questo codice ricordati anche le
  // strutture per NoP !
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

  sc_signal<int>    free_slots_to_east[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
  sc_signal<int>    free_slots_to_west[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
  sc_signal<int>    free_slots_to_south[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
  sc_signal<int>    free_slots_to_north[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];

  // NoP
  sc_signal<TNoP_data>    NoP_data_to_east[MAX_STATIC_DIM][MAX_STATIC_DIM];
  sc_signal<TNoP_data>    NoP_data_to_west[MAX_STATIC_DIM][MAX_STATIC_DIM];
  sc_signal<TNoP_data>    NoP_data_to_south[MAX_STATIC_DIM][MAX_STATIC_DIM];
  sc_signal<TNoP_data>    NoP_data_to_north[MAX_STATIC_DIM][MAX_STATIC_DIM];

  // Matrix of tiles

  TTile*             t[MAX_STATIC_DIM][MAX_STATIC_DIM];


  // Global tables
  TGlobalRoutingTable grtable;
  TGlobalTrafficTable gttable;

  //---------- Mau experiment <start>
  void flitsMonitor() {

    if (!reset.read())
      {
	//	if ((int)sc_simulation_time() % 5)
	//	  return;

	unsigned int count = 0;
	for(int i=0; i<TGlobalParams::mesh_dim_x; i++)
	  for(int j=0; j<TGlobalParams::mesh_dim_y; j++)
	    count += t[i][j]->r->getFlitsCount();
	cout << count << endl;
      }
  }
  //---------- Mau experiment <stop>

  // Constructor

  SC_CTOR(TNoC)
  {

    //---------- Mau experiment <start>
    /*
    SC_METHOD(flitsMonitor);
    sensitive(reset);
    sensitive_pos(clock);
    */
    //---------- Mau experiment <stop>

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

