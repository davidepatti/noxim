/*****************************************************************************

  main.cpp -- The testbench

 *****************************************************************************/

#include <systemc.h>
#include "TNoC.h"
#include "TGlobalStats.h"

int sc_main(int arg_num, char* arg_vet[])
{
  // Signals
  sc_clock            clock("clock", 1);
  sc_signal<bool>     reset;

  // NoC instance
  TNoC* n = new TNoC("NoC");
  n->clock(clock);
  n->reset(reset);

  // Print the banner
  printf("#############################################\n");
  printf("#                                           #\n");
  printf("#  #     #  #######  #     #  ###  #     #  #\n");
  printf("#  ##    #  #     #   #   #    #   ##   ##  #\n");
  printf("#  # #   #  #     #    # #     #   # # # #  #\n");
  printf("#  #  #  #  #     #     #      #   #  #  #  #\n");
  printf("#  #   # #  #     #    # #     #   #     #  #\n");
  printf("#  #    ##  #     #   #   #    #   #     #  #\n");
  printf("#  #     #  #######  #     #  ###  #     #  #\n");
  printf("#                                           #\n");
  printf("#              the NoC Simulator            #\n");
  printf("#                                           #\n");
  printf("#     (C) 2005 University of Catania        #\n");
  printf("#                                           #\n");
  printf("#############################################\n");

  // Trace signals
  sc_trace_file* tf = sc_create_vcd_trace_file("trace");
  sc_trace(tf, reset, "reset");
  sc_trace(tf, clock, "clock");

  for(int i=0; i<MESH_DIM_X; i++)
  {
    for(int j=0; j<MESH_DIM_Y; j++)
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
  printf("Reset asserted... Cleaning up registers.\n");
  srand(time(NULL));
  sc_start(1000, SC_NS);                                 // 1000 ns reset time
  reset.write(0);
  printf("Reset deasserted... Fiat Lux!!!\n");
  sc_start(10000, SC_NS);                                // 10'000 ns total simulation time

  // Close the simulation and exit
  sc_close_vcd_trace_file(tf);
  printf("Simulation completed successfully!\n");


  TGlobalStats gs(n);

  gs.showStats();

  /*
  for (int y=0; y<MESH_DIM_Y; y++)
    for (int x=0; x<MESH_DIM_X; x++)
      n->t[x][y]->r->stats.showStats();
  */
  
  return 0;
}

