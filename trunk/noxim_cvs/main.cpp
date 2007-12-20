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
int   TGlobalParams::routing_algorithm                = DEFAULT_ROUTING_ALGORITHM;
char  TGlobalParams::routing_table_filename[128]      = DEFAULT_ROUTING_TABLE_FILENAME;
int   TGlobalParams::selection_strategy               = DEFAULT_SELECTION_STRATEGY;
float TGlobalParams::packet_injection_rate            = DEFAULT_PACKET_INJECTION_RATE;
float TGlobalParams::probability_of_retransmission    = DEFAULT_PROBABILITY_OF_RETRANSMISSION;
int   TGlobalParams::traffic_distribution             = DEFAULT_TRAFFIC_DISTRIBUTION;
char  TGlobalParams::traffic_table_filename[128]      = DEFAULT_TRAFFIC_TABLE_FILENAME;
int   TGlobalParams::simulation_time                  = DEFAULT_SIMULATION_TIME;
int   TGlobalParams::stats_warm_up_time               = DEFAULT_STATS_WARM_UP_TIME;
int   TGlobalParams::rnd_generator_seed               = time(NULL);
bool  TGlobalParams::detailed                         = DEFAULT_DETAILED;
float TGlobalParams::dyad_threshold                   = DEFAULT_DYAD_THRESHOLD;
unsigned int TGlobalParams::max_volume_to_be_drained  = DEFAULT_MAX_VOLUME_TO_BE_DRAINED;
vector<pair<int,double> > TGlobalParams::hotspots;

//---------------------------------------------------------------------------

void showHelp(char selfname[])
{
  cout << "Usage: " << selfname << " [options]\nwhere [options] is one or more of the following ones:" << endl;
  cout << "\t-help\t\tShow this help and exit" << endl;
  cout << "\t-verbose N\tVerbosity level (1=low, 2=medium, 3=high, default off)" << endl;
  cout << "\t-trace FILENAME\tTrace signals to a VCD file named 'FILENAME.vcd' (default off)" << endl;
  cout << "\t-dimx N\t\tSet the mesh X dimension to the specified integer value (default " << DEFAULT_MESH_DIM_X << ")" << endl;
  cout << "\t-dimy N\t\tSet the mesh Y dimension to the specified integer value (default " << DEFAULT_MESH_DIM_Y << ")" << endl;
  cout << "\t-buffer N\tSet the buffer depth of each channel of the router to the specified integer value [flits] (default " << DEFAULT_BUFFER_DEPTH << ")" << endl;
  cout << "\t-size Nmin Nmax\tSet the minimum and maximum packet size to the specified integer values [flits] (default min=" << DEFAULT_MIN_PACKET_SIZE << ", max=" << DEFAULT_MAX_PACKET_SIZE << ")" << endl;
  cout << "\t-routing TYPE\tSet the routing algorithm to TYPE where TYPE is one of the following (default " << DEFAULT_ROUTING_ALGORITHM << "):" << endl;
  cout << "\t\txy\t\tXY routing algorithm" << endl;
  cout << "\t\twestfirst\tWest-First routing algorithm" << endl;
  cout << "\t\tnorthlast\tNorth-Last routing algorithm" << endl;
  cout << "\t\tnegativefirst\tNegative-First routing algorithm" << endl;
  cout << "\t\toddeven\t\tOdd-Even routing algorithm" << endl;
  cout << "\t\tdyad T\t\tDyAD routing algorithm with threshold T" << endl;
  cout << "\t\tfullyadaptive\tFully-Adaptive routing algorithm" << endl;
  cout << "\t\ttable FILENAME\tRouting Table Based routing algorithm with table in the specified file" << endl;
  cout << "\t-sel TYPE\tSet the selection strategy to TYPE where TYPE is one of the following (default " << DEFAULT_SELECTION_STRATEGY << "):" << endl;
  cout << "\t\trandom\t\tRandom selection strategy" << endl;
  cout << "\t\tbufferlevel\tBuffer-Level Based selection strategy" << endl;
  cout << "\t\tnop\t\tNeighbors-on-Path selection strategy" << endl;
  cout << "\t-pir R\t\tSet the packet injection rate to the specified real value [0..1] (default " << DEFAULT_PACKET_INJECTION_RATE << ")" << endl;
  cout << "\t-dist TYPE\tSet the time distribution of traffic to TYPE where TYPE is one of the following:" << endl;
  cout << "\t\tpoisson\t\tMemory-less Poisson distribution (default)" << endl;
  cout << "\t\tburst R\t\tBurst distribution with given real burstness" << endl;
  cout << "\t\tpareto on off r\tSelf-similar Pareto distribution with given real parameters (alfa-on alfa-off r)" << endl;
  cout << "\t\tcustom R\tCustom distribution with given real probability of retransmission" << endl;
  cout << "\t-traffic TYPE\tSet the spatial distribution of traffic to TYPE where TYPE is one of the following (default " << DEFAULT_TRAFFIC_DISTRIBUTION << "'):" << endl;
  cout << "\t\trandom\t\tRandom traffic distribution" << endl;
  cout << "\t\ttranspose1\tTranspose matrix 1 traffic distribution" << endl;
  cout << "\t\ttranspose2\tTranspose matrix 2 traffic distribution" << endl;
  cout << "\t\tbitreversal\tBit-reversal traffic distribution" << endl;
  cout << "\t\tbutterfly\tButterfly traffic distribution" << endl;
  cout << "\t\tshuffle\t\tShuffle traffic distribution" << endl;
  cout << "\t\ttable FILENAME\tTraffic Table Based traffic distribution with table in the specified file" << endl;
  cout << "\t-hs ID P\tAdd node ID to hotspot nodes, with percentage P (0..1) (Only for 'random' traffic)" << endl;
  cout << "\t-warmup N\tStart to collect statistics after N cycles (default " << DEFAULT_STATS_WARM_UP_TIME << ")" << endl;
  cout << "\t-seed N\t\tSet the seed of the random generator (default time())" << endl;
  cout << "\t-detailed\tShow detailed statistics" << endl;
  cout << "\t-volume N\tStop the simulation when either the maximum number of cycles has been reached or N flits have been delivered" << endl;
  cout << "\t-sim N\t\tRun for the specified simulation time [cycles] (default " << DEFAULT_SIMULATION_TIME << ")" << endl << endl;
  cout << "If you find this program useful please don't forget to mention in your paper Maurizio Palesi <mpalesi@diit.unict.it>" << endl;
  cout << "If you find this program useless please feel free to complain with Davide Patti <dpatti@diit.unict.it>" << endl;
  cout << "And if you want to send money please feel free to PayPal to Fabrizio Fazzino <fabrizio@fazzino.it>" << endl;
  exit(0);
}

//---------------------------------------------------------------------------

void showConfig()
{
  cout << "Using the following configuration: " << endl;
  cout << "- verbose_mode = " << TGlobalParams::verbose_mode << endl;
  cout << "- trace_mode = " << TGlobalParams::trace_mode << endl;
  //  cout << "- trace_filename = " << TGlobalParams::trace_filename << endl;
  cout << "- mesh_dim_x = " << TGlobalParams::mesh_dim_x << endl;
  cout << "- mesh_dim_y = " << TGlobalParams::mesh_dim_y << endl;
  cout << "- buffer_depth = " << TGlobalParams::buffer_depth << endl;
  cout << "- max_packet_size = " << TGlobalParams::max_packet_size << endl;
  cout << "- routing_algorithm = " << TGlobalParams::routing_algorithm << endl;
  //  cout << "- routing_table_filename = " << TGlobalParams::routing_table_filename << endl;
  cout << "- selection_strategy = " << TGlobalParams::selection_strategy << endl;
  cout << "- packet_injection_rate = " << TGlobalParams::packet_injection_rate << endl;
  cout << "- probability_of_retransmission = " << TGlobalParams::probability_of_retransmission << endl;
  cout << "- traffic_distribution = " << TGlobalParams::traffic_distribution << endl;
  cout << "- simulation_time = " << TGlobalParams::simulation_time << endl;
  cout << "- stats_warm_up_time = " << TGlobalParams::stats_warm_up_time << endl;
  cout << "- rnd_generator_seed = " << TGlobalParams::rnd_generator_seed << endl;
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

void badInputFilename(char filename[])
{
  fprintf(stderr, "Error: The specified file '%s' could not be opened for reading\n", filename);
  exit(1);
}

//---------------------------------------------------------------------------

int sc_main(int arg_num, char* arg_vet[])
{
    // TEMP
    drained_volume = 0;
  bool specifiedPir = false;
  bool specifiedPor = false;

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
	int level = atoi(arg_vet[i+1]);
	if ( (level>0) && (level<4))
	{
	  TGlobalParams::verbose_mode = level;
	  i+=2;
	}
	else if ( level>=4)
	{
	  TGlobalParams::verbose_mode = -level;
	  i+=2;
	}
        else badArgument(arg_vet[i+1], arg_vet[i]);
      }
      else if(!strcmp(arg_vet[i],"-trace"))
      {
        TGlobalParams::trace_mode = true;
        strcpy(TGlobalParams::trace_filename, arg_vet[i+1]);
        i+=2;
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
        if(new_buffer>=1)
	{
          TGlobalParams::buffer_depth = new_buffer;
          i+=2;
        }
        else badArgument(arg_vet[i+1], arg_vet[i]);
      }
      else if(!strcmp(arg_vet[i],"-size"))
      {
        int new_min_size = atoi(arg_vet[i+1]);
        int new_max_size = atoi(arg_vet[i+2]);
        if(new_min_size>=2 && new_max_size >= new_min_size)
	{
          TGlobalParams::min_packet_size = new_min_size;
          TGlobalParams::max_packet_size = new_max_size;
          i+=3;
        }
        else badArgument(arg_vet[i+1], arg_vet[i]);
      }
      else if(!strcmp(arg_vet[i],"-routing"))
      {
        if(!strcmp(arg_vet[i+1],"xy")) TGlobalParams::routing_algorithm = ROUTING_XY;
        else if(!strcmp(arg_vet[i+1],"westfirst")) TGlobalParams::routing_algorithm = ROUTING_WEST_FIRST;
        else if(!strcmp(arg_vet[i+1],"northlast")) TGlobalParams::routing_algorithm = ROUTING_NORTH_LAST;
        else if(!strcmp(arg_vet[i+1],"negativefirst")) TGlobalParams::routing_algorithm = ROUTING_NEGATIVE_FIRST;
        else if(!strcmp(arg_vet[i+1],"oddeven")) TGlobalParams::routing_algorithm = ROUTING_ODD_EVEN;
        else if(!strcmp(arg_vet[i+1],"dyad")) 
	  {
	    TGlobalParams::routing_algorithm = ROUTING_DYAD;
	    TGlobalParams::dyad_threshold = atof(arg_vet[i+2]);
	    i++;
	  }
        else if(!strcmp(arg_vet[i+1],"fullyadaptive")) TGlobalParams::routing_algorithm = ROUTING_FULLY_ADAPTIVE;
        else if(!strcmp(arg_vet[i+1],"table"))
	{
          TGlobalParams::routing_algorithm = ROUTING_TABLE_BASED;
          strcpy(TGlobalParams::routing_table_filename, arg_vet[i+2]);
          FILE* fp = fopen(TGlobalParams::routing_table_filename, "r");
          if(fp==NULL) badInputFilename(TGlobalParams::routing_table_filename);
          fclose(fp);
          TGlobalParams::packet_injection_rate = 0;
          i++;
        }
        else badArgument(arg_vet[i+1], arg_vet[i]);
        i+=2;
      }
      else if(!strcmp(arg_vet[i],"-sel"))
      {
        if(!strcmp(arg_vet[i+1],"random")) TGlobalParams::selection_strategy = SEL_RANDOM;
        else if(!strcmp(arg_vet[i+1],"bufferlevel")) TGlobalParams::selection_strategy = SEL_BUFFER_LEVEL;
        else if(!strcmp(arg_vet[i+1],"nop")) TGlobalParams::selection_strategy = SEL_NOP;
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
          specifiedPir = true;
        }
        else badArgument(arg_vet[i+1], arg_vet[i]);
      }
      else if(!strcmp(arg_vet[i],"-dist"))
      {
        float new_por = 0;
        if(!strcmp(arg_vet[i+1],"poisson"))
        {
          new_por = TGlobalParams::packet_injection_rate;
        }
        else if(!strcmp(arg_vet[i+1],"burst"))
        {
          float burstness = atof(arg_vet[i+2]);
          new_por = TGlobalParams::packet_injection_rate/(1-burstness);
          i++;
        }
        else if(!strcmp(arg_vet[i+1],"pareto"))
        {
          float Aon = atof(arg_vet[i+2]);
          float Aoff = atof(arg_vet[i+3]);
          float r = atof(arg_vet[i+4]);
          new_por = TGlobalParams::packet_injection_rate*pow((1-r),(1/Aoff-1/Aon));
          i+=3;
        }
        else if(!strcmp(arg_vet[i+1],"custom"))
        {
          new_por = atof(arg_vet[i+2]);
          i++;
        }
        else badArgument(arg_vet[i+1], arg_vet[i]);
        assert(new_por>=0 && new_por<=1);
        specifiedPor = true;
        TGlobalParams::probability_of_retransmission = new_por;  
        i+=2;
      }
      else if(!strcmp(arg_vet[i],"-traffic"))
      {
	  if(!strcmp(arg_vet[i+1],"random")) TGlobalParams::traffic_distribution = TRAFFIC_RANDOM;
	  else if(!strcmp(arg_vet[i+1],"transpose1")) TGlobalParams::traffic_distribution = TRAFFIC_TRANSPOSE1;
	  else if(!strcmp(arg_vet[i+1],"transpose2")) TGlobalParams::traffic_distribution = TRAFFIC_TRANSPOSE2;
	  else if(!strcmp(arg_vet[i+1],"bitreversal")) TGlobalParams::traffic_distribution = TRAFFIC_BIT_REVERSAL;
	  else if(!strcmp(arg_vet[i+1],"butterfly")) TGlobalParams::traffic_distribution = TRAFFIC_BUTTERFLY;
	  else if(!strcmp(arg_vet[i+1],"shuffle")) TGlobalParams::traffic_distribution = TRAFFIC_SHUFFLE;
	  else if(!strcmp(arg_vet[i+1],"table"))
	  {
	      TGlobalParams::traffic_distribution = TRAFFIC_TABLE_BASED;
	      strcpy(TGlobalParams::traffic_table_filename, arg_vet[i+2]);
	      FILE* fp = fopen(TGlobalParams::traffic_table_filename, "r");
	      if(fp==NULL) badInputFilename(TGlobalParams::traffic_table_filename);
	      fclose(fp);
	      i++;
	  }
	  else badArgument(arg_vet[i+1], arg_vet[i]);
	  i+=2;
      }
      else if(!strcmp(arg_vet[i],"-hs")) 
      {
	  int node = atoi(arg_vet[i+1]);
	  double percentage = atof(arg_vet[i+2]);

	  if ( node>=0 && percentage >= 0.0)
	  {
	      pair<int,double> t(node,percentage);
	      TGlobalParams::hotspots.push_back(t);
	      i+=3;
	      cout << "\n Added node " << node << " with P = " << percentage << endl;
	  }
	  else
	  {
	      cout << "\n Bad -hs arguments ! " << endl;
	      exit(1);
	  }
      }
      else if(!strcmp(arg_vet[i],"-warmup"))
      {
        int new_warmup = atoi(arg_vet[i+1]);
        if(new_warmup>=0)
	{
          TGlobalParams::stats_warm_up_time = new_warmup;
          i+=2;
        }
        else badArgument(arg_vet[i+1], arg_vet[i]);
      }
      else if(!strcmp(arg_vet[i],"-seed"))
      {
        int new_rnd_generator_seed = atoi(arg_vet[i+1]);
        if(new_rnd_generator_seed>=0)
	{
          TGlobalParams::rnd_generator_seed = new_rnd_generator_seed;
          i+=2;
        }
        else badArgument(arg_vet[i+1], arg_vet[i]);
      }
      else if(!strcmp(arg_vet[i],"-detailed"))
	{
	  TGlobalParams::detailed = true;
	  i++;
	}
      else if(!strcmp(arg_vet[i],"-volume"))
	{
        int new_vol = atoi(arg_vet[i+1]);
        if(new_vol>1)
	{
          TGlobalParams::max_volume_to_be_drained = new_vol;
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

  // Check special cases
  if(specifiedPir && !specifiedPor)
    TGlobalParams::probability_of_retransmission = TGlobalParams::packet_injection_rate;

  if (TGlobalParams::routing_algorithm == ROUTING_XY &&
      TGlobalParams::selection_strategy != SEL_RANDOM)
  {
    cout << "\n Warning: using -sel option in conjunction with XY static routing!" << endl;
  }

  // Show configuration
  if(TGlobalParams::verbose_mode > VERBOSE_OFF) showConfig();

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

  return 0;
}

