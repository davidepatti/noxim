/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the command line parser
 */

#include "ConfigurationManager.h"
#include <systemc.h> //Included for the function time() 

void loadConfiguration() {

    YAML::Node config = YAML::LoadFile("config.yaml");

    // Initialize global configuration parameters (can be overridden with command-line arguments)
    GlobalParams::verbose_mode = config["verbose_mode"].as<int>();
    GlobalParams::trace_mode = config["trace_mode"].as<bool>();
    strcpy(GlobalParams::trace_filename, config["trace_filename"].as<string>().c_str());
    GlobalParams::mesh_dim_x = config["mesh_dim_x"].as<int>();
    GlobalParams::mesh_dim_y = config["mesh_dim_y"].as<int>();
    GlobalParams::buffer_depth = config["buffer_depth"].as<int>();
    GlobalParams::min_packet_size = config["min_packet_size"].as<int>();
    GlobalParams::max_packet_size = config["max_packet_size"].as<int>();
    GlobalParams::routing_algorithm = config["routing_algorithm"].as<int>();
    strcpy(GlobalParams::routing_table_filename, config["routing_table_filename"].as<string>().c_str()); 
    GlobalParams::selection_strategy = config["selection_strategy"].as<int>();
    GlobalParams::packet_injection_rate = config["packet_injection_rate"].as<float>();
    GlobalParams::probability_of_retransmission = config["probability_of_retransmission"].as<float>();
    GlobalParams::traffic_distribution = config["traffic_distribution"].as<int>();
    strcpy(GlobalParams::traffic_table_filename, config["traffic_table_filename"].as<string>().c_str());
    GlobalParams::simulation_time = config["simulation_time"].as<int>();
    GlobalParams::reset_time = config["reset_time"].as<int>();
    GlobalParams::stats_warm_up_time = config["stats_warm_up_time"].as<int>();
    GlobalParams::rnd_generator_seed = time(NULL);
    GlobalParams::detailed = config["detailed"].as<bool>();
    GlobalParams::dyad_threshold = config["dyad_threshold"].as<float>();
    GlobalParams::max_volume_to_be_drained = config["max_volume_to_be_drained"].as<unsigned int>();
    //GlobalParams::hotspots;
    strcpy(GlobalParams::router_power_filename, config["router_power_filename"].as<string>().c_str()); 
    GlobalParams::low_power_link_strategy = config["low_power_link_strategy"].as<bool>();
    GlobalParams::qos = config["qos"].as<double>();
    GlobalParams::show_buffer_stats = config["show_buffer_stats"].as<bool>();
    GlobalParams::use_winoc = config["use_winoc"].as<bool>();
    
    for(YAML::const_iterator hubs_it = config["Hubs"].begin(); 
        hubs_it != config["Hubs"].end();
        ++hubs_it)
    {   
        int hub_id = hubs_it->first.as<int>();
        YAML::Node hub_config_node = hubs_it->second;

        GlobalParams::hub_configuration[hub_id] = hub_config_node.as<HubConfig>(); 

        YAML::Node node;
        node[hub_id] = GlobalParams::hub_configuration[hub_id];
        cout << node[hub_id] << endl;
    }

    for(YAML::const_iterator channels_it= config["Channels"].begin(); 
        channels_it != config["Channels"].end();
        ++channels_it)
    {    
        int channel_id = channels_it->first.as<int>();
        YAML::Node channel_config_node = channels_it->second;

        GlobalParams::channel_configuration[channel_id] = channel_config_node.as<ChannelConfig>(); 

        YAML::Node node;
        node[channel_id] = GlobalParams::channel_configuration[channel_id];
        cout << node[channel_id] << endl;
    }
}

void showHelp(char selfname[])
{
    cout << "Usage: " << selfname << " [options]" << endl
         << "Where [options] is one or more of the following ones:" << endl
         << "\t-help\t\tShow this help and exit" << endl
         << "\t-verbose N\tVerbosity level (1=low, 2=medium, 3=high, default off)" << endl
         << "\t-trace FILENAME\tTrace signals to a VCD file named 'FILENAME.vcd' (default off)" << endl
         << "\t-dimx N\t\tSet the mesh X dimension to the specified integer value (default " << GlobalParams::mesh_dim_x << ")" << endl
         << "\t-dimy N\t\tSet the mesh Y dimension to the specified integer value (default " << GlobalParams::mesh_dim_y << ")" << endl
         << "\t-buffer N\tSet the buffer depth of each channel of the router to the specified integer value [flits] (default "<< GlobalParams::buffer_depth << ")" << endl
         << "\t-winoc enable radio hub wireless transmission" << endl
         << "\t-size Nmin Nmax\tSet the minimum and maximum packet size to the specified integer values [flits] (default min=" << GlobalParams::min_packet_size << ", max=" << GlobalParams::max_packet_size << ")" << endl
         << "\t-routing TYPE\tSet the routing algorithm to TYPE where TYPE is one of the following (default " << GlobalParams::routing_algorithm << "):" << endl
         << "\t\txy\t\tXY routing algorithm" << endl
         << "\t\twestfirst\tWest-First routing algorithm" << endl
         << "\t\tnorthlast\tNorth-Last routing algorithm" << endl
         << "\t\tnegativefirst\tNegative-First routing algorithm" << endl
         << "\t\toddeven\t\tOdd-Even routing algorithm" << endl
         << "\t\tdyad T\t\tDyAD routing algorithm with threshold T" << endl
         << "\t\tfullyadaptive\tFully-Adaptive routing algorithm" << endl
         << "\t\ttable FILENAME\tRouting Table Based routing algorithm with table in the specified file" << endl
         << "\t-sel TYPE\tSet the selection strategy to TYPE where TYPE is one of the following (default " << GlobalParams::selection_strategy << "):" << endl
         << "\t\trandom\t\tRandom selection strategy" << endl
         << "\t\tbufferlevel\tBuffer-Level Based selection strategy" << endl
         << "\t\tnop\t\tNeighbors-on-Path selection strategy" << endl
         <<	"\t-pir R TYPE\tSet the packet injection rate to the specified real value [0..1] (default " << GlobalParams::packet_injection_rate << ") " << "and the time distribution" << endl
         << "\t\t\tof traffic to TYPE where TYPE is one of the following:" << endl
         << "\t\tpoisson\t\tMemory-less Poisson distribution (default)" << endl
         << "\t\tburst R\t\tBurst distribution with given real burstness" << endl
         << "\t\tpareto on off r\tSelf-similar Pareto distribution with given real parameters (alfa-on alfa-off r)" << endl
         << "\t\tcustom R\tCustom distribution with given real probability of retransmission" << endl
         << "\t-traffic TYPE\tSet the spatial distribution of traffic to TYPE where TYPE is one of the following (default " << GlobalParams::traffic_distribution << "):" << endl
         << "\t\trandom\t\tRandom traffic distribution" << endl
         << "\t\ttranspose1\tTranspose matrix 1 traffic distribution" << endl
         << "\t\ttranspose2\tTranspose matrix 2 traffic distribution" << endl
         << "\t\tbitreversal\tBit-reversal traffic distribution" << endl
         << "\t\tbutterfly\tButterfly traffic distribution" << endl
         << "\t\tshuffle\t\tShuffle traffic distribution" << endl
         <<	"\t\ttable FILENAME\tTraffic Table Based traffic distribution with table in the specified file" << endl
         << "\t-hs ID P\tAdd node ID to hotspot nodes, with percentage P (0..1) (Only for 'random' traffic)" << endl
         << "\t-pwr FILENAME\tRouter and link power data (default " << GlobalParams::router_power_filename << ")" << endl
         << "\t-lpls\t\tEnable low power link strategy (default " << GlobalParams::low_power_link_strategy << ")" << endl
         << "\t-qos PERCENTAGE\tPercentage of communication that have to be routed on regular links (default " << GlobalParams::qos << ")" << endl
         << "\t-warmup N\tStart to collect statistics after N cycles (default " << GlobalParams::stats_warm_up_time << ")" << endl
         << "\t-seed N\t\tSet the seed of the random generator (default time())" << endl
         << "\t-detailed\tShow detailed statistics" << endl
         << "\t-show_buf_stats\tShow buffers statistics (default " << GlobalParams::show_buffer_stats << ")" << endl
         << "\t-volume N\tStop the simulation when either the maximum number of cycles has been reached or N flits have" << endl
         << "\t\t\tbeen delivered" << endl
         << "\t-sim N\t\tRun for the specified simulation time [cycles] (default " << GlobalParams::simulation_time << ")" << endl
         << endl
         << "If you find this program useful please don't forget to mention in your paper Maurizio Palesi <maurizio.palesi@unikore.it>" << endl
         <<	"If you find this program useless please feel free to complain with Davide Patti <dpatti@dieei.unict.it>" << endl
         <<	"And if you want to send money please feel free to PayPal to Fabrizio Fazzino <fabrizio@fazzino.it>" << endl;
}

void showConfig()
{
    cout << "Using the following configuration: " << endl
         << "- verbose_mode = " << GlobalParams::verbose_mode << endl
         << "- trace_mode = " << GlobalParams::trace_mode << endl
      // << "- trace_filename = " << GlobalParams::trace_filename << endl
         << "- mesh_dim_x = " << GlobalParams::mesh_dim_x << endl
         << "- mesh_dim_y = " << GlobalParams::mesh_dim_y << endl
         << "- buffer_depth = " << GlobalParams::buffer_depth << endl
         << "- max_packet_size = " << GlobalParams::max_packet_size << endl
         << "- routing_algorithm = " << GlobalParams::routing_algorithm << endl
      // << "- routing_table_filename = " << GlobalParams::routing_table_filename << endl
         << "- selection_strategy = " << GlobalParams::selection_strategy << endl
         << "- packet_injection_rate = " << GlobalParams::packet_injection_rate << endl
         << "- probability_of_retransmission = " << GlobalParams::probability_of_retransmission << endl
         << "- traffic_distribution = " << GlobalParams::traffic_distribution << endl
         << "- simulation_time = " << GlobalParams::simulation_time << endl
         << "- stats_warm_up_time = " << GlobalParams::stats_warm_up_time << endl
         << "- rnd_generator_seed = " << GlobalParams::rnd_generator_seed << endl;
}

void checkConfiguration()
{
    if (GlobalParams::mesh_dim_x <= 1) {
	cerr << "Error: dimx must be greater than 1" << endl;
	exit(1);
    }

    if (GlobalParams::mesh_dim_y <= 1) {
	cerr << "Error: dimy must be greater than 1" << endl;
	exit(1);
    }

    if (GlobalParams::buffer_depth < 1) {
	cerr << "Error: buffer must be >= 1" << endl;
	exit(1);
    }

    if (GlobalParams::min_packet_size < 2 ||
	GlobalParams::max_packet_size < 2) {
	cerr << "Error: packet size must be >= 2" << endl;
	exit(1);
    }

    if (GlobalParams::min_packet_size >
	GlobalParams::max_packet_size) {
	cerr << "Error: min packet size must be less than max packet size"
	    << endl;
	exit(1);
    }

    if (GlobalParams::routing_algorithm == INVALID_ROUTING) {
	cerr << "Error: invalid routing algorithm" << endl;
	exit(1);
    }

    if (GlobalParams::selection_strategy == INVALID_SELECTION) {
	cerr << "Error: invalid selection policy" << endl;
	exit(1);
    }

    if (GlobalParams::packet_injection_rate <= 0.0 ||
	GlobalParams::packet_injection_rate > 1.0) {
	cerr <<
	    "Error: packet injection rate mmust be in the interval ]0,1]"
	    << endl;
	exit(1);
    }

    if (GlobalParams::traffic_distribution == INVALID_TRAFFIC) {
	cerr << "Error: invalid traffic" << endl;
	exit(1);
    }

    for (unsigned int i = 0; i < GlobalParams::hotspots.size(); i++) {
	if (GlobalParams::hotspots[i].first >=
	    GlobalParams::mesh_dim_x *
	    GlobalParams::mesh_dim_y) {
	    cerr << "Error: hotspot node " << GlobalParams::
		hotspots[i].first << " is invalid (out of range)" << endl;
	    exit(1);
	}
	if (GlobalParams::hotspots[i].second < 0.0
	    && GlobalParams::hotspots[i].second > 1.0) {
	    cerr <<
		"Error: hotspot percentage must be in the interval [0,1]"
		<< endl;
	    exit(1);
	}
    }

    if (GlobalParams::stats_warm_up_time < 0) {
	cerr << "Error: warm-up time must be positive" << endl;
	exit(1);
    }

    if (GlobalParams::simulation_time < 0) {
	cerr << "Error: simulation time must be positive" << endl;
	exit(1);
    }

    if (GlobalParams::stats_warm_up_time >
	GlobalParams::simulation_time) {
	cerr << "Error: warmup time must be less than simulation time" <<
	    endl;
	exit(1);
    }

    if (GlobalParams::qos < 1.0 && 
	!GlobalParams::low_power_link_strategy) {
      cerr << "Warning: it makes no sense using -qos without -lpls option enabled" << endl;
    }

    if (GlobalParams::qos > 1.0 || GlobalParams::qos < 0.0) {
	cerr << "Error: qos must be in the range [0,1]" << endl;
	exit(1);
    }
}

void parseCmdLine(int arg_num, char *arg_vet[])
{
    if (arg_num == 1)
	cout <<
	    "Running with default parameters (use '-help' option to see how to override them)"
	    << endl;
    else {
	for (int i = 1; i < arg_num; i++) {
	    if (!strcmp(arg_vet[i], "-help")) {
		showHelp(arg_vet[0]);
		exit(0);
	    } else if (!strcmp(arg_vet[i], "-verbose"))
		GlobalParams::verbose_mode = atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-trace")) {
		GlobalParams::trace_mode = true;
		strcpy(GlobalParams::trace_filename, arg_vet[++i]);
	    } else if (!strcmp(arg_vet[i], "-dimx"))
		GlobalParams::mesh_dim_x = atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-dimy"))
		GlobalParams::mesh_dim_y = atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-buffer"))
		GlobalParams::buffer_depth = atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-winoc")) {
		GlobalParams::use_winoc = true;
	    }
	    else if (!strcmp(arg_vet[i], "-size")) {
		GlobalParams::min_packet_size = atoi(arg_vet[++i]);
		GlobalParams::max_packet_size = atoi(arg_vet[++i]);
	    } else if (!strcmp(arg_vet[i], "-routing")) {
		char *routing = arg_vet[++i];
		if (!strcmp(routing, "xy"))
		    GlobalParams::routing_algorithm = ROUTING_XY;
		else if (!strcmp(routing, "westfirst"))
		    GlobalParams::routing_algorithm =
			ROUTING_WEST_FIRST;
		else if (!strcmp(routing, "northlast"))
		    GlobalParams::routing_algorithm =
			ROUTING_NORTH_LAST;
		else if (!strcmp(routing, "negativefirst"))
		    GlobalParams::routing_algorithm =
			ROUTING_NEGATIVE_FIRST;
		else if (!strcmp(routing, "oddeven"))
		    GlobalParams::routing_algorithm =
			ROUTING_ODD_EVEN;
		else if (!strcmp(routing, "dyad")) {
		    GlobalParams::routing_algorithm = ROUTING_DYAD;
		    GlobalParams::dyad_threshold = atof(arg_vet[++i]);
		} else if (!strcmp(routing, "fullyadaptive"))
		    GlobalParams::routing_algorithm =
			ROUTING_FULLY_ADAPTIVE;
		else if (!strcmp(routing, "table")) {
		    GlobalParams::routing_algorithm =
			ROUTING_TABLE_BASED;
		    strcpy(GlobalParams::routing_table_filename,
			   arg_vet[++i]);
		    GlobalParams::packet_injection_rate = 0;	// ??? why ???
		} else
		    GlobalParams::routing_algorithm = INVALID_ROUTING;
	    } else if (!strcmp(arg_vet[i], "-sel")) {
		char *selection = arg_vet[++i];
		if (!strcmp(selection, "random"))
		    GlobalParams::selection_strategy = SEL_RANDOM;
		else if (!strcmp(selection, "bufferlevel"))
		    GlobalParams::selection_strategy =
			SEL_BUFFER_LEVEL;
		else if (!strcmp(selection, "nop"))
		    GlobalParams::selection_strategy = SEL_NOP;
		else
		    GlobalParams::selection_strategy =
			INVALID_SELECTION;
	    } else if (!strcmp(arg_vet[i], "-pir")) {
		GlobalParams::packet_injection_rate =
		    atof(arg_vet[++i]);
		char *distribution = arg_vet[++i];
		if (!strcmp(distribution, "poisson"))
		    GlobalParams::probability_of_retransmission =
			GlobalParams::packet_injection_rate;
		else if (!strcmp(distribution, "burst")) {
		    float burstness = atof(arg_vet[++i]);
		    GlobalParams::probability_of_retransmission =
			GlobalParams::packet_injection_rate / (1 -
								    burstness);
		} else if (!strcmp(distribution, "pareto")) {
		    float Aon = atof(arg_vet[++i]);
		    float Aoff = atof(arg_vet[++i]);
		    float r = atof(arg_vet[++i]);
		    GlobalParams::probability_of_retransmission =
			GlobalParams::packet_injection_rate *
			pow((1 - r), (1 / Aoff - 1 / Aon));
		} else if (!strcmp(distribution, "custom"))
		    GlobalParams::probability_of_retransmission =
			atof(arg_vet[++i]);
	    } else if (!strcmp(arg_vet[i], "-traffic")) {
		char *traffic = arg_vet[++i];
		if (!strcmp(traffic, "random"))
		    GlobalParams::traffic_distribution =
			TRAFFIC_RANDOM;
		else if (!strcmp(traffic, "transpose1"))
		    GlobalParams::traffic_distribution =
			TRAFFIC_TRANSPOSE1;
		else if (!strcmp(traffic, "transpose2"))
		    GlobalParams::traffic_distribution =
			TRAFFIC_TRANSPOSE2;
		else if (!strcmp(traffic, "bitreversal"))
		    GlobalParams::traffic_distribution =
			TRAFFIC_BIT_REVERSAL;
		else if (!strcmp(traffic, "butterfly"))
		    GlobalParams::traffic_distribution =
			TRAFFIC_BUTTERFLY;
		else if (!strcmp(traffic, "shuffle"))
		    GlobalParams::traffic_distribution =
			TRAFFIC_SHUFFLE;
		else if (!strcmp(traffic, "table")) {
		    GlobalParams::traffic_distribution =
			TRAFFIC_TABLE_BASED;
		    strcpy(GlobalParams::traffic_table_filename,
			   arg_vet[++i]);
		} else
		    GlobalParams::traffic_distribution =
			INVALID_TRAFFIC;
	    } else if (!strcmp(arg_vet[i], "-hs")) {
		int node = atoi(arg_vet[++i]);
		double percentage = atof(arg_vet[++i]);
		pair < int, double >t(node, percentage);
		GlobalParams::hotspots.push_back(t);
	    } else if (!strcmp(arg_vet[i], "-warmup"))
		GlobalParams::stats_warm_up_time = atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-seed"))
		GlobalParams::rnd_generator_seed = atoi(arg_vet[++i]);
 	    else if (!strcmp(arg_vet[i], "-detailed"))
		GlobalParams::detailed = true;
	    else if (!strcmp(arg_vet[i], "-show_buf_stats"))
		GlobalParams::show_buffer_stats = true;
	    else if (!strcmp(arg_vet[i], "-volume"))
		GlobalParams::max_volume_to_be_drained =
		    atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-sim"))
		GlobalParams::simulation_time = atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-pwr"))
	      strcpy(GlobalParams::router_power_filename, arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-lpls"))
	      GlobalParams::low_power_link_strategy = true;
	    else if (!strcmp(arg_vet[i], "-qos"))
		GlobalParams::qos = atof(arg_vet[++i]);
	    else {
		cerr << "Error: Invalid option: " << arg_vet[i] << endl;
		exit(1);
	    }
	}
    }


}


void configure(int arg_num, char *arg_vet[]) {
    loadConfiguration();
    parseCmdLine(arg_num, arg_vet);

    checkConfiguration();

    // Show configuration
    if (GlobalParams::verbose_mode > VERBOSE_OFF)
	showConfig();
}
