/*****************************************************************************

  main.cpp -- The testbench

 *****************************************************************************/

#include <systemc.h>
#include "NoximDefs.h"
#include "TNoC.h"
#include "TGlobalStats.h"

//---------------------------------------------------------------------------

// Initialize global configuration parameters (can be overridden with command-line arguments)
int TGlobalParams::verbose_mode            = DEFAULT_VERBOSE_MODE;
int TGlobalParams::mesh_dim_x              = DEFAULT_MESH_DIM_X;
int TGlobalParams::mesh_dim_y              = DEFAULT_MESH_DIM_Y;
int TGlobalParams::buffer_depth            = DEFAULT_BUFFER_DEPTH;
int TGlobalParams::max_packet_size         = DEFAULT_MAX_PACKET_SIZE;
int TGlobalParams::routing_algorithm       = DEFAULT_ROUTING_ALGORITHM;
int TGlobalParams::selection_strategy      = DEFAULT_SELECTION_STRATEGY;
float TGlobalParams::packet_injection_rate = DEFAULT_PACKET_INJECTION_RATE;
int TGlobalParams::simulation_time         = DEFAULT_SIMULATION_TIME;

//---------------------------------------------------------------------------

int sc_main(int arg_num, char* arg_vet[])
{
  // Signals
  sc_clock        clock("clock", 1);
  sc_signal<bool> reset;

  // NoC instance
  TNoC* n = new TNoC("NoC");
  n->clock(clock);
  n->reset(reset);

  // Handle command-line arguments
  cout << endl << "\t\tNoxim - the NoC Simulator" << endl;
  cout << "\t\t(C) University of Catania" << endl;
  if(arg_num==1)
  {
    cout << "Running with default parameters (use '-help' option to see how to override them)" << endl;
  }
  else
  {
    int i=1;
    do
    {
      if(!strcmp(arg_vet[i],"-help"))
      {
        cout << "Usage: " << arg_vet[0] << " [options]\nwhere [options] is one or more of the following ones:" << endl;
        cout << "\t\t-help\tShow this help and exit" << endl;
        cout << "\t\t-verbose\tVerbose output" << endl;
        exit(0);
      }
      else if(!strcmp(arg_vet[i],"-verbose"))
      {
        TGlobalParams::verbose_mode = true;
        i++;
      }
      else
      {
        fprintf(stderr, "Error: Unknown option %s (use '-help' to see available options)\n", arg_vet[i]);
        exit(1);
      }
    } while (i<arg_num);
  }
  cout << "Using the following configuration: " << endl;
  cout << "verbose = " << TGlobalParams::verbose_mode << endl;
  cout << "mesh_dim_x = " << TGlobalParams::mesh_dim_x << endl;
  cout << "mesh_dim_y = " << TGlobalParams::mesh_dim_y << endl;
  cout << "buffer_depth = " << TGlobalParams::buffer_depth << endl;
  cout << "max_packet_size = " << TGlobalParams::max_packet_size << endl;
  cout << "routing_algorithm = " << TGlobalParams::routing_algorithm << endl;
  cout << "selection_strategy = " << TGlobalParams::selection_strategy << endl;
  cout << "packet_injection_rate = " << TGlobalParams::packet_injection_rate << endl;
  cout << "simulation_time = " << TGlobalParams::simulation_time << endl;

  // Trace signals
  sc_trace_file* tf = sc_create_vcd_trace_file("trace");
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

  // Reset the chip and run the simulation
  reset.write(1);
  cout << "Reset asserted... cleaning up registers" << endl;
  srand(time(NULL));
  sc_start(1000, SC_NS);                                 // 1000 ns reset time
  reset.write(0);
  cout << "Reset deasserted... Fiat Lux!!!" << endl;
  sc_start(TGlobalParams::simulation_time, SC_NS);       // Run simulation

  // Close the simulation
  sc_close_vcd_trace_file(tf);
  cout << "Simulation completed successfully!" << endl;

  // Show statistics
  TGlobalStats gs(n);
  gs.showStats();

  /*
  for (int y=0; y<MESH_DIM_Y; y++)
    for (int x=0; x<MESH_DIM_X; x++)
      n->t[x][y]->r->stats.showStats();
  */
  
  return 0;
}

