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
int TGlobalParams::trace_mode              = DEFAULT_TRACE_MODE;
int TGlobalParams::mesh_dim_x              = DEFAULT_MESH_DIM_X;
int TGlobalParams::mesh_dim_y              = DEFAULT_MESH_DIM_Y;
int TGlobalParams::buffer_depth            = DEFAULT_BUFFER_DEPTH;
int TGlobalParams::max_packet_size         = DEFAULT_MAX_PACKET_SIZE;
int TGlobalParams::routing_algorithm       = DEFAULT_ROUTING_ALGORITHM;
int TGlobalParams::selection_strategy      = DEFAULT_SELECTION_STRATEGY;
float TGlobalParams::packet_injection_rate = DEFAULT_PACKET_INJECTION_RATE;
int TGlobalParams::simulation_time         = DEFAULT_SIMULATION_TIME;

//---------------------------------------------------------------------------

void showHelp(char selfname[])
{
  cout << "Usage: " << selfname << " [options]\nwhere [options] is one or more of the following ones:" << endl;
  cout << "\t-help\t\tShow this help and exit" << endl;
  cout << "\t-verbose\tVerbose output (default off)" << endl;
  cout << "\t-trace\t\tTrace signals to VCD file named 'trace.vcd' (default off)" << endl;
  cout << "\t-dimx N\t\tSet the mesh X dimension to the specified integer value (default " << DEFAULT_MESH_DIM_X << ")" << endl;
  cout << "\t-dimy N\t\tSet the mesh Y dimension to the specified integer value (default " << DEFAULT_MESH_DIM_Y << ")" << endl;
  cout << "\t-buffer N\tSet the buffer depth of each channel of the router to the specified integer value [flits] (default " << DEFAULT_BUFFER_DEPTH << ")" << endl;
  cout << "\t-size N\t\tSet the maximum packet size to the specified integer value [flits] (default " << DEFAULT_MAX_PACKET_SIZE << ")" << endl;
  cout << "\t-routing {xy|westfirst|northlast|negativefirst|oddeven|dyad|lookahead|nopcar|fullyadaptive}\tSet routing algorithm (default " << DEFAULT_ROUTING_ALGORITHM << ")" << endl;
  cout << "\t-sel {random|bufferlevel|nopcar}\tSet selection strategy (default " << DEFAULT_SELECTION_STRATEGY << ")" << endl;
  cout << "\t-pir R\t\tSet the packet injection rate to the specified real value [%] (default " << DEFAULT_PACKET_INJECTION_RATE << ")" << endl;
  cout << "\t-run N\t\tRun the simulation for the specified time [ns] (default " << DEFAULT_SIMULATION_TIME << ")" << endl << endl;
  cout << "If you find this program useful please don't forget to mention in your paper Maurizio Palesi <mpalesi@diit.unict.it>" << endl;
  cout << "If you find this program useless please feel free to complain with Davide Patti <dpatti@diit.unict.it>" << endl;
  cout << "And if you want to send money please feel free to PayPal to Fabrizio Fazzino <fabrizio@fazzino.it>" << endl;
  exit(0);
}

//---------------------------------------------------------------------------

void showConfig()
{
  cout << "Using the following configuration: " << endl;
  cout << "verbose_mode = " << TGlobalParams::verbose_mode << endl;
  cout << "trace_mode = " << TGlobalParams::trace_mode << endl;
  cout << "mesh_dim_x = " << TGlobalParams::mesh_dim_x << endl;
  cout << "mesh_dim_y = " << TGlobalParams::mesh_dim_y << endl;
  cout << "buffer_depth = " << TGlobalParams::buffer_depth << endl;
  cout << "max_packet_size = " << TGlobalParams::max_packet_size << endl;
  cout << "routing_algorithm = " << TGlobalParams::routing_algorithm << endl;
  cout << "selection_strategy = " << TGlobalParams::selection_strategy << endl;
  cout << "packet_injection_rate = " << TGlobalParams::packet_injection_rate << endl;
  cout << "simulation_time = " << TGlobalParams::simulation_time << endl;
}

//---------------------------------------------------------------------------

void badOption(char option[])
{
  fprintf(stderr, "Error: Unknown option %s (use '-help' to see available options)\n", option);
  exit(1);
}

//---------------------------------------------------------------------------

void badArgument(char argument[], char option[])
{
  fprintf(stderr, "Error: Unknown argument %s for option %s (use '-help' to see available options and arguments)\n", argument, option);
  exit(1);
}

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
  cout << "\t\t(C) University of Catania" << endl << endl;
  if(arg_num==1)
  {
    cout << "Running with default parameters (use '-help' option to see how to override them)" << endl;
  }
  else
  {
    int i=1;
    do
    {
      if(!strcmp(arg_vet[i],"-help")) showHelp(arg_vet[0]);
      else if(!strcmp(arg_vet[i],"-verbose"))
      {
        TGlobalParams::verbose_mode = true;
        i++;
      }
      else if(!strcmp(arg_vet[i],"-trace"))
      {
        TGlobalParams::trace_mode = true;
        i++;
      }
      else if(!strcmp(arg_vet[i],"-dimx"))
      {
        int new_x = atoi(arg_vet[i+1]);
        if(new_x>1)
	{
          TGlobalParams::mesh_dim_x = new_x;
          i+=2;
        }
        else badArgument(arg_vet[i+1], arg_vet[i]);
      }
      else if(!strcmp(arg_vet[i],"-dimy"))
      {
        int new_y = atoi(arg_vet[i+1]);
        if(new_y>1)
	{
          TGlobalParams::mesh_dim_y = new_y;
          i+=2;
        }
        else badArgument(arg_vet[i+1], arg_vet[i]);
      }
      else if(!strcmp(arg_vet[i],"-buffer"))
      {
        int new_buffer = atoi(arg_vet[i+1]);
        if(new_buffer>1)
	{
          TGlobalParams::buffer_depth = new_buffer;
          i+=2;
        }
        else badArgument(arg_vet[i+1], arg_vet[i]);
      }
      else if(!strcmp(arg_vet[i],"-size"))
      {
        int new_size = atoi(arg_vet[i+1]);
        if(new_size>1)
	{
          TGlobalParams::max_packet_size = new_size;
          i+=2;
        }
        else badArgument(arg_vet[i+1], arg_vet[i]);
      }
      else if(!strcmp(arg_vet[i],"-routing"))
      {
        if(!strcmp(arg_vet[i+1],"xy")) TGlobalParams::routing_algorithm = XY;
        else if(!strcmp(arg_vet[i+1],"westfirst")) TGlobalParams::routing_algorithm = WEST_FIRST;
        else if(!strcmp(arg_vet[i+1],"northlast")) TGlobalParams::routing_algorithm = NORTH_LAST;
        else if(!strcmp(arg_vet[i+1],"negativefirst")) TGlobalParams::routing_algorithm = NEGATIVE_FIRST;
        else if(!strcmp(arg_vet[i+1],"oddeven")) TGlobalParams::routing_algorithm = ODD_EVEN;
        else if(!strcmp(arg_vet[i+1],"dyad")) TGlobalParams::routing_algorithm = DYAD;
        else if(!strcmp(arg_vet[i+1],"lookahead")) TGlobalParams::routing_algorithm = LOOK_AHEAD;
        else if(!strcmp(arg_vet[i+1],"nopcar")) TGlobalParams::routing_algorithm = NOPCAR;
        else if(!strcmp(arg_vet[i+1],"fullyadaptive")) TGlobalParams::routing_algorithm = FULLY_ADAPTIVE;
        else badArgument(arg_vet[i+1], arg_vet[i]);
        i+=2;
      }
      else if(!strcmp(arg_vet[i],"-sel"))
      {
        if(!strcmp(arg_vet[i+1],"random")) TGlobalParams::selection_strategy = SEL_RANDOM;
        else if(!strcmp(arg_vet[i+1],"bufferlevel")) TGlobalParams::selection_strategy = SEL_BUFFER_LEVEL;
        else if(!strcmp(arg_vet[i+1],"nopcar")) TGlobalParams::selection_strategy = SEL_NOPCAR;
        else badArgument(arg_vet[i+1], arg_vet[i]);
        i+=2;
      }
      else if(!strcmp(arg_vet[i],"-pir"))
      {
        float new_pir = atof(arg_vet[i+1]);
        if(new_pir>0)
	{
          TGlobalParams::packet_injection_rate = new_pir;
          i+=2;
        }
        else badArgument(arg_vet[i+1], arg_vet[i]);
      }
      else if(!strcmp(arg_vet[i],"-sim"))
      {
        int new_sim = atoi(arg_vet[i+1]);
        if(new_sim>1)
	{
          TGlobalParams::simulation_time = new_sim;
          i+=2;
        }
        else badArgument(arg_vet[i+1], arg_vet[i]);
      }
      else badOption(arg_vet[i]);
    } while (i<arg_num);
  }
  showConfig();

  // Trace signals
  sc_trace_file* tf;
  if(TGlobalParams::trace_mode)
  {
    tf = sc_create_vcd_trace_file("trace");
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
  cout << "Reset asserted... cleaning up registers" << endl;
  srand(time(NULL));
  sc_start(1000, SC_NS);                                 // 1000 ns reset time
  reset.write(0);
  cout << "Reset deasserted... Fiat Lux!!!" << endl;
  sc_start(TGlobalParams::simulation_time, SC_NS);       // Run simulation

  // Close the simulation
  if(TGlobalParams::trace_mode) sc_close_vcd_trace_file(tf);
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

