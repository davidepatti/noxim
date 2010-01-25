/*****************************************************************************

  main.cpp -- The testbench

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

#include <systemc.h>
#include "NoximDefs.h"
#include "TNoC.h"
#include "TGlobalStats.h"
#include "CmdLineParser.h"


using namespace std;

// need to be globally visible to allow "-volume" simulation stop
unsigned int drained_volume;

//---------------------------------------------------------------------------

// Initialize global configuration parameters (can be overridden with command-line arguments)
int   TGlobalParams::verbose_mode                     = DEFAULT_VERBOSE_MODE;
int   TGlobalParams::trace_mode                       = DEFAULT_TRACE_MODE;
char  TGlobalParams::trace_filename[128]              = DEFAULT_TRACE_FILENAME;
int   TGlobalParams::mesh_dim_x                       = DEFAULT_MESH_DIM_X;
int   TGlobalParams::mesh_dim_y                       = DEFAULT_MESH_DIM_Y;
int   TGlobalParams::buffer_depth                     = DEFAULT_BUFFER_DEPTH;
int   TGlobalParams::min_packet_size                  = DEFAULT_MIN_PACKET_SIZE;
int   TGlobalParams::max_packet_size                  = DEFAULT_MAX_PACKET_SIZE;
int   TGlobalParams::min_communication_size           = DEFAULT_MIN_COMM_SIZE;
int   TGlobalParams::max_communication_size           = DEFAULT_MAX_COMM_SIZE;
float TGlobalParams::comm_blocking_probability        = DEFAULT_PB_COMM;
int   TGlobalParams::routing_algorithm                = DEFAULT_ROUTING_ALGORITHM;
char  TGlobalParams::routing_table_filename[128]      = DEFAULT_ROUTING_TABLE_FILENAME;
int   TGlobalParams::selection_strategy               = DEFAULT_SELECTION_STRATEGY;
float TGlobalParams::packet_injection_rate            = DEFAULT_PACKET_INJECTION_RATE;
float TGlobalParams::probability_of_retransmission    = DEFAULT_PROBABILITY_OF_RETRANSMISSION;
int   TGlobalParams::traffic_distribution             = DEFAULT_TRAFFIC_DISTRIBUTION;
char  TGlobalParams::traffic_table_filename[128]      = DEFAULT_TRAFFIC_TABLE_FILENAME;
int   TGlobalParams::simulation_time                  = DEFAULT_SIMULATION_TIME;
bool TGlobalParams::in_order_packets_delivery_cam     = DEFAULT_IN_ORDER_PACKETS_DELIVERY_CAM;
unsigned int TGlobalParams::inorder_CAM_capacity      = DEFAULT_INORDER_CAM_CAPACITY;
bool TGlobalParams::in_order_packets_delivery_blocking= DEFAULT_IN_ORDER_PACKETS_DELIVERY_BLOCKING;
int   TGlobalParams::stats_warm_up_time               = DEFAULT_STATS_WARM_UP_TIME;
int   TGlobalParams::rnd_generator_seed               = time(NULL);
bool  TGlobalParams::detailed                         = DEFAULT_DETAILED;
float TGlobalParams::dyad_threshold                   = DEFAULT_DYAD_THRESHOLD;
unsigned int TGlobalParams::max_volume_to_be_drained  = DEFAULT_MAX_VOLUME_TO_BE_DRAINED;
vector<pair<int,double> > TGlobalParams::hotspots;

// tmp - to be removed
unsigned int TGlobalParams::tmp_cam_accesses = 0;
unsigned int TGlobalParams::tmp_cam_hits = 0;

//---------------------------------------------------------------------------

int sc_main(int arg_num, char* arg_vet[])
{
    // TEMP
    drained_volume = 0;

  // Handle command-line arguments
  cout << endl << "\t\tNoxim - the NoC Simulator" << endl;
  cout << "\t\t(C) University of Catania" << endl << endl;

  parseCmdLine(arg_num, arg_vet);

  // Signals
  sc_clock        clock("clock", 1, SC_NS);
  sc_signal<bool> reset;

  // NoC instance
  TNoC* n = new TNoC("NoC");
  n->clock(clock);
  n->reset(reset);

  // Trace signals
  sc_trace_file* tf = NULL;
  if(TGlobalParams::trace_mode)
  {
    tf = sc_create_vcd_trace_file(TGlobalParams::trace_filename);
    sc_trace(tf, reset, "reset");
    sc_trace(tf, clock, "clock");

    for(int i=0; i<TGlobalParams::mesh_dim_x; i++)
    {
      for(int j=0; j<TGlobalParams::mesh_dim_y; j++)
      {
        char label[30];

        sprintf(label, "req_to_east(%02d)(%02d)", i, j);
        sc_trace(tf, n->req_to_east[i][j], label);
        sprintf(label, "req_to_west(%02d)(%02d)", i, j);
        sc_trace(tf, n->req_to_west[i][j], label);
        sprintf(label, "req_to_south(%02d)(%02d)", i, j);
        sc_trace(tf, n->req_to_south[i][j], label);
        sprintf(label, "req_to_north(%02d)(%02d)", i, j);
        sc_trace(tf, n->req_to_north[i][j], label);

        sprintf(label, "ack_to_east(%02d)(%02d)", i, j);
        sc_trace(tf, n->ack_to_east[i][j], label);
        sprintf(label, "ack_to_west(%02d)(%02d)", i, j);
        sc_trace(tf, n->ack_to_west[i][j], label);
        sprintf(label, "ack_to_south(%02d)(%02d)", i, j);
        sc_trace(tf, n->ack_to_south[i][j], label);
        sprintf(label, "ack_to_north(%02d)(%02d)", i, j);
        sc_trace(tf, n->ack_to_north[i][j], label);
/*
        sprintf(label, "flit_to_east(%02d)(%02d)", i, j);
        sc_trace(tf, n->flit_to_east[i][j], label);
        sprintf(label, "flit_to_west(%02d)(%02d)", i, j);
        sc_trace(tf, n->flit_to_west[i][j], label);
        sprintf(label, "flit_to_south(%02d)(%02d)", i, j);
        sc_trace(tf, n->flit_to_south[i][j], label);
        sprintf(label, "flit_to_north(%02d)(%02d)", i, j);
        sc_trace(tf, n->flit_to_north[i][j], label);
*/
      }
    }
  }


  // Reset the chip and run the simulation
  reset.write(1);
  cout << "Reset...";
  srand(TGlobalParams::rnd_generator_seed); // time(NULL));
  sc_start(DEFAULT_RESET_TIME, SC_NS);
  reset.write(0);
  cout << " done! Now running for " << TGlobalParams::simulation_time << " cycles..." << endl;
  sc_start(TGlobalParams::simulation_time, SC_NS);

  // Close the simulation
  if(TGlobalParams::trace_mode) sc_close_vcd_trace_file(tf);
  cout << "Noxim simulation completed." << endl;
  cout << " ( " << sc_time_stamp().to_double()/1000 << " cycles executed)" << endl;

  // Show statistics
  TGlobalStats gs(n);
  gs.showStats(std::cout, TGlobalParams::detailed);

  if ((TGlobalParams::max_volume_to_be_drained>0) &&
      (sc_time_stamp().to_double()/1000 >= TGlobalParams::simulation_time))
      {
	  cout << "\nWARNING! the number of flits specified with -volume option"<<endl;
	  cout << "has not been reached. ( " << drained_volume << " instead of " << TGlobalParams::max_volume_to_be_drained << " )" <<endl;
	  cout << "You might want to try an higher value of simulation cycles" << endl;
	  cout << "using -sim option." << endl;
#ifdef TESTING
	  cout << "\n Sum of local drained flits: "  << gs.drained_total << endl;
	  cout << "\n Effective drained volume: " << drained_volume;
#endif
      }

  /*
  for (int y=0; y<MESH_DIM_Y; y++)
    for (int x=0; x<MESH_DIM_X; x++)
      n->t[x][y]->r->stats.showStats();
  */

  // tmp - to be removed
  cout << "% CAM miss rate (%): " << 100.0*(TGlobalParams::tmp_cam_accesses-TGlobalParams::tmp_cam_hits)/TGlobalParams::tmp_cam_accesses << endl;

  return 0;
}

