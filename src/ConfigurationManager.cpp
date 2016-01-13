/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2015 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the command line parser
 */

#include "ConfigurationManager.h"
#include <systemc.h> //Included for the function time() 

void loadConfiguration() {

    YAML::Node config;
    YAML::Node power_config;

    cout << "Loading configuration from file \"" << GlobalParams::config_filename << "\"...";
    try {
        config = YAML::LoadFile(GlobalParams::config_filename);
        cout << " Done" << endl;
    } catch (YAML::BadFile &e){
        cout << " Failed" << endl;
        cerr << "The specified YAML configuration file was not found!" << endl;
        exit(0);
    }

    cout << "Loading power configurations from file \"" << GlobalParams::power_config_filename << "\"...";
    try {
        power_config = YAML::LoadFile(GlobalParams::power_config_filename);
        cout << " Done" << endl;
    } catch (YAML::BadFile &e){
        cout << " Failed" << endl;
        cerr << "The specified YAML power configurations file was not found!" << endl;
        exit(0);
    }

    // Initialize global configuration parameters (can be overridden with command-line arguments)
    GlobalParams::verbose_mode = config["verbose_mode"].as<string>();
    GlobalParams::trace_mode = config["trace_mode"].as<bool>();
    GlobalParams::trace_filename = config["trace_filename"].as<string>();
    GlobalParams::mesh_dim_x = config["mesh_dim_x"].as<int>();
    GlobalParams::mesh_dim_y = config["mesh_dim_y"].as<int>();
    GlobalParams::r2r_link_length = config["r2r_link_length"].as<double>();
    GlobalParams::r2h_link_length = config["r2h_link_length"].as<double>();
    GlobalParams::buffer_depth = config["buffer_depth"].as<int>();
    GlobalParams::flit_size = config["flit_size"].as<int>();
    GlobalParams::min_packet_size = config["min_packet_size"].as<int>();
    GlobalParams::max_packet_size = config["max_packet_size"].as<int>();
    GlobalParams::routing_algorithm = config["routing_algorithm"].as<string>();
    GlobalParams::routing_table_filename = config["routing_table_filename"].as<string>(); 
    GlobalParams::selection_strategy = config["selection_strategy"].as<string>();
    GlobalParams::packet_injection_rate = config["packet_injection_rate"].as<double>();
    GlobalParams::probability_of_retransmission = config["probability_of_retransmission"].as<double>();
    GlobalParams::traffic_distribution = config["traffic_distribution"].as<string>();
    GlobalParams::traffic_table_filename = config["traffic_table_filename"].as<string>();
    GlobalParams::clock_period_ps = config["clock_period_ps"].as<int>();
    GlobalParams::simulation_time = config["simulation_time"].as<int>();
    GlobalParams::reset_time = config["reset_time"].as<int>();
    GlobalParams::stats_warm_up_time = config["stats_warm_up_time"].as<int>();
    GlobalParams::rnd_generator_seed = time(NULL);
    GlobalParams::detailed = config["detailed"].as<bool>();
    GlobalParams::dyad_threshold = config["dyad_threshold"].as<double>();
    GlobalParams::max_volume_to_be_drained = config["max_volume_to_be_drained"].as<unsigned int>();
    //GlobalParams::hotspots;
    GlobalParams::show_buffer_stats = config["show_buffer_stats"].as<bool>();
    GlobalParams::use_winoc = config["use_winoc"].as<bool>();
    GlobalParams::use_wirxsleep = config["use_wirxsleep"].as<bool>();
    
    GlobalParams::default_hub_configuration = config["Hubs"]["defaults"].as<HubConfig>();

    for(YAML::const_iterator hubs_it = config["Hubs"].begin(); 
        hubs_it != config["Hubs"].end();
        ++hubs_it)
    {   
        int hub_id = hubs_it->first.as<int>(-1);
        if (hub_id < 0)
            continue;

        YAML::Node hub_config_node = hubs_it->second;

        GlobalParams::hub_configuration[hub_id] = hub_config_node.as<HubConfig>(); 

        YAML::Node node;
        node[hub_id] = GlobalParams::hub_configuration[hub_id];
    }

    GlobalParams::default_channel_configuration = config["Channels"]["defaults"].as<ChannelConfig>();

    for(YAML::const_iterator channels_it= config["Channels"].begin(); 
        channels_it != config["Channels"].end();
        ++channels_it)
    {    
        int channel_id = channels_it->first.as<int>(-1);
        if (channel_id < 0)
            continue;

        YAML::Node channel_config_node = channels_it->second;

        GlobalParams::channel_configuration[channel_id] = channel_config_node.as<ChannelConfig>(); 

        YAML::Node node;
        node[channel_id] = GlobalParams::channel_configuration[channel_id];
    }

    GlobalParams::power_configuration = power_config["Energy"].as<PowerConfig>();
}

void showHelp(char selfname[])
{
    cout << "Usage: " << selfname << " [options]" << endl
         << "Where [options] is one or more of the following ones:" << endl
         << "\t-help\t\tShow this help and exit" << endl
         << "\t-config\t\tLoad the specified configuration file" << endl
         << "\t-power\t\tLoad the specified power configurations file" << endl
         << "\t-verbose N\tVerbosity level (1=low, 2=medium, 3=high)" << endl
         << "\t-trace FILENAME\tTrace signals to a VCD file named 'FILENAME.vcd'" << endl
         << "\t-dimx N\t\tSet the mesh X dimension" << endl
         << "\t-dimy N\t\tSet the mesh Y dimension" << endl
         << "\t-buffer N\tSet the depth of router input buffers [flits]" << endl
         << "\t-winoc enable radio hub wireless transmission" << endl
         << "\t-wirxsleep enable radio hub wireless power manager" << endl
         << "\t-size Nmin Nmax\tSet the minimum and maximum packet size [flits]" << endl
         << "\t-routing TYPE\tSet the routing algorithm to one of the following:" << endl
         << "\t\tXY\t\tXY routing algorithm" << endl
         << "\t\tWEST_FIRST\tWest-First routing algorithm" << endl
         << "\t\tNORTH_LAST\tNorth-Last routing algorithm" << endl
         << "\t\tNEGATIVE_FIRST\tNegative-First routing algorithm" << endl
         << "\t\tODD_EVEN\t\tOdd-Even routing algorithm" << endl
         << "\t\tDYAD T\t\tDyAD routing algorithm with threshold T" << endl
         << "\t\tTABLE_BASED FILENAME\tRouting Table Based routing algorithm with table in the specified file" << endl
         << "\t-sel TYPE\tSet the selection strategy to one of the following:" << endl
         << "\t\tRANDOM\t\tRandom selection strategy" << endl
         << "\t\tBUFFER_LEVEL\tBuffer-Level Based selection strategy" << endl
         << "\t\tNOP\t\tNeighbors-on-Path selection strategy" << endl
         <<	"\t-pir R TYPE\tSet the packet injection rate R [0..1] and the time distribution TYPE" << endl
         << "\t\t\twhere TYPE is one of the following:" << endl
         << "\t\tpoisson\t\tMemory-less Poisson distribution" << endl
         << "\t\tburst R\t\tBurst distribution with given real burstness" << endl
         << "\t\tpareto on off r\tSelf-similar Pareto distribution with given real parameters (alfa-on alfa-off r)" << endl
         << "\t\tcustom R\tCustom distribution with given real probability of retransmission" << endl
         << "\t-traffic TYPE\tSet the spatial distribution of traffic to TYPE where TYPE is one of the following:" << endl
         << "\t\trandom\t\tRandom traffic distribution" << endl
         << "\t\tlocal L\t\tRandom traffic with a fraction L (0..1) of packets having a destination connected to the local hub, i.e. not using wireless" << endl
         << "\t\tulocal\t\tRandom traffic with locality smooth distribution" << endl
         << "\t\ttranspose1\tTranspose matrix 1 traffic distribution" << endl
         << "\t\ttranspose2\tTranspose matrix 2 traffic distribution" << endl
         << "\t\tbitreversal\tBit-reversal traffic distribution" << endl
         << "\t\tbutterfly\tButterfly traffic distribution" << endl
         << "\t\tshuffle\t\tShuffle traffic distribution" << endl
         <<	"\t\ttable FILENAME\tTraffic Table Based traffic distribution with table in the specified file" << endl
         << "\t-hs ID P\tAdd node ID to hotspot nodes, with percentage P (0..1) (Only for 'random' traffic)" << endl
         << "\t-warmup N\tStart to collect statistics after N cycles" << endl
         << "\t-seed N\t\tSet the seed of the random generator (default time())" << endl
         << "\t-detailed\tShow detailed statistics" << endl
         << "\t-show_buf_stats\tShow buffers statistics" << endl
         << "\t-volume N\tStop the simulation when either the maximum number of cycles has been reached or N flits have" << endl
         << "\t\t\tbeen delivered" << endl
         << "\t-sim N\t\tRun for the specified simulation time [cycles]" << endl
         << endl
         << "If you find this program useful please don't forget to mention in your paper Maurizio Palesi <maurizio.palesi@unikore.it>" << endl
         <<	"If you find this program useless please feel free to complain with Davide Patti <davide.patti@dieei.unict.it>" << endl
         <<	"If you want to send money please feel free to PayPal to Fabrizio Fazzino <fabrizio@fazzino.it>" << endl
         <<	"and if need to solve any other problem of your life please contact Turi Monteleone <salvatore.monteleone@dieei.unict.it>" << endl;
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
         << "- clock_period = " << GlobalParams::clock_period_ps << "ps" << endl
         << "- simulation_time = " << GlobalParams::simulation_time << endl
         << "- warm_up_time = " << GlobalParams::stats_warm_up_time << endl
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

    if (GlobalParams::selection_strategy.compare("INVALID_SELECTION") == 0) {
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

    if (GlobalParams::locality<0 || GlobalParams::locality>1)
    {
	cerr << "Error: traffic locality must be in the range 0..1" << endl;
	exit(1);
    }


}

void parseCmdLine(int arg_num, char *arg_vet[])
{
    if (arg_num == 1)
	cout <<
	    "Running with default parameters (use '-help' option to see how to override them)"
	    << endl;
    else 
    {
	for (int i = 1; i < arg_num; i++) 
	{
	    if (!strcmp(arg_vet[i], "-verbose"))
		GlobalParams::verbose_mode = atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-trace")) 
	    {
		GlobalParams::trace_mode = true;
		GlobalParams::trace_filename = arg_vet[++i];
	    } 
	    else if (!strcmp(arg_vet[i], "-dimx"))
		GlobalParams::mesh_dim_x = atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-dimy"))
		GlobalParams::mesh_dim_y = atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-buffer"))
		GlobalParams::buffer_depth = atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-winoc")) 
		GlobalParams::use_winoc = true;
	    else if (!strcmp(arg_vet[i], "-wirxsleep")) 
	    {
		GlobalParams::use_wirxsleep = true;
	    }
	    else if (!strcmp(arg_vet[i], "-size")) 
	    {
		GlobalParams::min_packet_size = atoi(arg_vet[++i]);
		GlobalParams::max_packet_size = atoi(arg_vet[++i]);
	    } 
	    else if (!strcmp(arg_vet[i], "-routing")) 
	    {
		GlobalParams::routing_algorithm = arg_vet[++i];
		if (GlobalParams::routing_algorithm == ROUTING_DYAD)
		    GlobalParams::dyad_threshold = atof(arg_vet[++i]);
		else if (GlobalParams::routing_algorithm == ROUTING_TABLE_BASED) 
		{
		    GlobalParams::routing_table_filename = arg_vet[++i];
		    GlobalParams::packet_injection_rate = 0;
		} 
	    } 
	    else if (!strcmp(arg_vet[i], "-sel")) {
		GlobalParams::selection_strategy = arg_vet[++i];
	    } 
	    else if (!strcmp(arg_vet[i], "-pir")) 
	    {
		GlobalParams::packet_injection_rate = atof(arg_vet[++i]);
		char *distribution = arg_vet[++i];
		if (!strcmp(distribution, "poisson"))
		    GlobalParams::probability_of_retransmission = GlobalParams::packet_injection_rate;
		else if (!strcmp(distribution, "burst")) 
		{
		    double burstness = atof(arg_vet[++i]);
		    GlobalParams::probability_of_retransmission = GlobalParams::packet_injection_rate / (1 - burstness);
		} 
		else if (!strcmp(distribution, "pareto")) {
		    double Aon = atof(arg_vet[++i]);
		    double Aoff = atof(arg_vet[++i]);
		    double r = atof(arg_vet[++i]);
		    GlobalParams::probability_of_retransmission =
			GlobalParams::packet_injection_rate *
			pow((1 - r), (1 / Aoff - 1 / Aon));
		} 
		else if (!strcmp(distribution, "custom"))
		    GlobalParams::probability_of_retransmission = atof(arg_vet[++i]);
		else assert("Invalid pir format" && false);
	    } 
	    else if (!strcmp(arg_vet[i], "-traffic")) 
	    {
		char *traffic = arg_vet[++i];
		if (!strcmp(traffic, "random")) GlobalParams::traffic_distribution = TRAFFIC_RANDOM;
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
		else if (!strcmp(traffic, "ulocal"))
		    GlobalParams::traffic_distribution =
			TRAFFIC_ULOCAL;
		else if (!strcmp(traffic, "table")) {
		    GlobalParams::traffic_distribution =
			TRAFFIC_TABLE_BASED;
		    GlobalParams::traffic_table_filename = arg_vet[++i];
		} else if (!strcmp(traffic, "local")) {
		    GlobalParams::traffic_distribution = TRAFFIC_LOCAL;
		    GlobalParams::locality=atof(arg_vet[++i]);
		}
		else assert(false);
	    } 
	    else if (!strcmp(arg_vet[i], "-hs")) 
	    {
		int node = atoi(arg_vet[++i]);
		double percentage = atof(arg_vet[++i]);
		pair < int, double >t(node, percentage);
		GlobalParams::hotspots.push_back(t);
	    } 
	    else if (!strcmp(arg_vet[i], "-warmup"))
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
	    else if (!strcmp(arg_vet[i], "-config") || !strcmp(arg_vet[i], "-power"))
		// -config is managed from configure function
		// i++ skips the configuration file name 
		i++;
	    else {
		cerr << "Error: Invalid option: " << arg_vet[i] << endl;
		exit(1);
	    }
	}
    }

}


void configure(int arg_num, char *arg_vet[]) {

    bool config_found = false;
    bool power_config_found = false;

    for (int i = 1; i < arg_num; i++) {
	    if (!strcmp(arg_vet[i], "-help")) {
		showHelp(arg_vet[0]);
		exit(0);
        }
    }

    for (int i = 1; i < arg_num; i++) {
	    if (!strcmp(arg_vet[i], "-config")) {
            GlobalParams::config_filename = arg_vet[++i];
            config_found = true;
            break;
        }
    }

    if (!config_found)
    {
        std::ifstream infile(CONFIG_FILENAME);
        if (infile.good())
            GlobalParams::config_filename = CONFIG_FILENAME;
        else
        {
            cerr << "No YAML configuration file found!\n Use -config to load examples from config_examples folder" << endl;
            exit(0);
        }
    }

    for (int i = 1; i < arg_num; i++) {
	    if (!strcmp(arg_vet[i], "-power")) {
            GlobalParams::power_config_filename = arg_vet[++i];
            power_config_found = true;
            break;
        }
    }

    if (!power_config_found)
    {
        std::ifstream infile(POWER_CONFIG_FILENAME);
        if (infile.good())
            GlobalParams::power_config_filename = POWER_CONFIG_FILENAME;
        else
        {
            cerr << "No YAML power configurations file found!\n Use -power to load examples from config_examples folder" << endl;
            exit(0);
        }
    }

    loadConfiguration();
    parseCmdLine(arg_num, arg_vet);

    checkConfiguration();

    // Show configuration
    if (GlobalParams::verbose_mode > VERBOSE_OFF)
	showConfig();
}
