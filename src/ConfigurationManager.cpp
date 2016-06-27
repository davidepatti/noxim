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
#include "Power.h"
#include <systemc.h> //Included for the function time() 

void loadConfiguration() {

    cout << "Loading configuration from file " << GlobalParams::config_filename << endl;
    YAML::Node config = YAML::LoadFile(GlobalParams::config_filename);

    // Initialize global configuration parameters (can be overridden with command-line arguments)
    GlobalParams::verbose_mode = config["verbose_mode"].as<int>();
    GlobalParams::trace_mode = config["trace_mode"].as<bool>();
    strcpy(GlobalParams::trace_filename, config["trace_filename"].as<string>().c_str());
    GlobalParams::mesh_dim_x = config["mesh_dim_x"].as<int>();
    GlobalParams::mesh_dim_y = config["mesh_dim_y"].as<int>();
    GlobalParams::r2r_link_length = config["r2r_link_length"].as<float>();
    GlobalParams::r2h_link_length = config["r2h_link_length"].as<float>();
    GlobalParams::buffer_depth = config["buffer_depth"].as<int>();
    GlobalParams::flit_size = config["flit_size"].as<int>();
    GlobalParams::min_packet_size = config["min_packet_size"].as<int>();
    GlobalParams::max_packet_size = config["max_packet_size"].as<int>();
    strcpy(GlobalParams::routing_algorithm, config["routing_algorithm"].as<string>().c_str());
    strcpy(GlobalParams::routing_table_filename, config["routing_table_filename"].as<string>().c_str()); 
    GlobalParams::selection_strategy = config["selection_strategy"].as<string>();
    GlobalParams::packet_injection_rate = config["packet_injection_rate"].as<float>();
    GlobalParams::probability_of_retransmission = config["probability_of_retransmission"].as<float>();
    GlobalParams::traffic_distribution = config["traffic_distribution"].as<int>();
    strcpy(GlobalParams::traffic_table_filename, config["traffic_table_filename"].as<string>().c_str());
    GlobalParams::clock_period_ps = config["clock_period_ps"].as<int>();
    GlobalParams::simulation_time = config["simulation_time"].as<double>();
    GlobalParams::reset_time = config["reset_time"].as<int>();
    GlobalParams::stats_warm_up_time = config["stats_warm_up_time"].as<int>();
    GlobalParams::rnd_generator_seed = time(NULL);
    GlobalParams::detailed = config["detailed"].as<bool>();
    GlobalParams::dyad_threshold = config["dyad_threshold"].as<float>();
    GlobalParams::max_volume_to_be_drained = config["max_volume_to_be_drained"].as<unsigned int>();
    //GlobalParams::hotspots;
    GlobalParams::show_buffer_stats = config["show_buffer_stats"].as<bool>();
    GlobalParams::use_winoc = config["use_winoc"].as<bool>();
    GlobalParams::use_wirxsleep = config["use_wirxsleep"].as<bool>();
    
    GlobalParams::default_hub_configuration = config["Hubs"]["defaults"].as<HubConfig>();

    GlobalParams::payload_pattern = config["payload_pattern"].as<int>();
    strcpy(GlobalParams::interconnect_model_filename, config["interconnect_model_filename"].as<string>().c_str());
    GlobalParams::link_pwr_model = config["link_pwr_model"].as<int>();
    GlobalParams::use_script_mode = config["use_script_mode"].as<bool>();
    GlobalParams::get_instant_noc_power = config["get_instant_noc_power"].as<bool>();
    strcpy(GlobalParams::app_traffic_pathname, config["app_traffic_pathname"].as<string>().c_str());
    
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

    GlobalParams::power_configuration = config["Energy"].as<PowerConfig>();
}

void showHelp(char selfname[])
{
    cout << "Usage: " << selfname << " [options]" << endl
         << "Where [options] is one or more of the following ones:" << endl
         << "\t-help\t\tShow this help and exit" << endl
         << "\t-config\t\tLoad the specified configuration file" << endl
         << "\t-verbose N\tVerbosity level (1=low, 2=medium, 3=high, default off)" << endl
         << "\t-trace FILENAME\tTrace signals to a VCD file named 'FILENAME.vcd' (default off)" << endl
         << "\t-dimx N\t\tSet the mesh X dimension to the specified integer value (default " << GlobalParams::mesh_dim_x << ")" << endl
         << "\t-dimy N\t\tSet the mesh Y dimension to the specified integer value (default " << GlobalParams::mesh_dim_y << ")" << endl
         << "\t-buffer N\tSet the buffer depth of each channel of the router to the specified integer value [flits] (default "<< GlobalParams::buffer_depth << ")" << endl
         << "\t-winoc enable radio hub wireless transmission" << endl
         << "\t-wirxsleep enable radio hub wireless receiver auto sleep" << endl
         << "\t-size Nmin Nmax\tSet the minimum and maximum packet size to the specified integer values [flits] (default min=" << GlobalParams::min_packet_size << ", max=" << GlobalParams::max_packet_size << ")" << endl
         << "\t-routing TYPE\tSet the routing algorithm to TYPE where TYPE is one of the following (default " << GlobalParams::routing_algorithm << "):" << endl
         << "\t\tXY\t\tXY routing algorithm" << endl
         << "\t\tWEST_FIRST\tWest-First routing algorithm" << endl
         << "\t\tNORTH_LAST\tNorth-Last routing algorithm" << endl
         << "\t\tNEGATIVE_FIRST\tNegative-First routing algorithm" << endl
         << "\t\tODD_EVEN\t\tOdd-Even routing algorithm" << endl
         << "\t\tDYAD T\t\tDyAD routing algorithm with threshold T" << endl
         << "\t\tFULLY_ADAPTIVE\tFully-Adaptive routing algorithm" << endl
         << "\t\tTABLE_BASED FILENAME\tRouting Table Based routing algorithm with table in the specified file" << endl
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
         <<	"\t\tapp \t\tTraffic Based on application with PE files in the folder APP" << endl
         << "\t-hs ID P\tAdd node ID to hotspot nodes, with percentage P (0..1) (Only for 'random' traffic)" << endl
         << "\t-warmup N\tStart to collect statistics after N cycles (default " << GlobalParams::stats_warm_up_time << ")" << endl
         << "\t-seed N\t\tSet the seed of the random generator (default time())" << endl
         << "\t-detailed\tShow detailed statistics" << endl
         << "\t-show_buf_stats\tShow buffers statistics (default " << GlobalParams::show_buffer_stats << ")" << endl
         << "\t-volume N\tStop the simulation when either the maximum number of cycles has been reached or N flits have" << endl
         << "\t\t\tbeen delivered" << endl
         << "\t-sim N\t\tRun for the specified simulation time [cycles] (default " << GlobalParams::simulation_time << ")" << endl
         << "\t-flit_size N\t Set the flit size with N(32 or 64) bits" << endl
         << "\t-r2r_length N\t Set the r2r links' length with N mm" << endl
         << "\t-link_pwr_model TYPE\t Set the link power model TYPE used in simulation : " << endl
         << "\t\t static\t Use the classic link power model" << endl
         << "\t\t crosstalk\t Use an improved link pwr model bit-accurate that takes into" << endl << "\t\t\t account crosstalk" << endl
         << "\t-payload_pattern TYPE\t Set the flit's payload according to a payload TYPE : " << endl
         << "\t\tbest\t only 0s contained in data flit's payload" << endl
         << "\t\tworst\t data flit's payload is word with 010101 pattern followed" << endl << "\t\t\t by a word with 101010 pattern" << endl
         << "\t\ttrue_rng flit's payload is randomly generated " << endl   
         << "\t\tINFO\t There are others patterns available, see GlobalParams.h"  << endl   
         << "\t-q\t\t Use of noxim script mode that stores all results in 1 line" << endl
         << "\t-link_config FILENAME\t Set the link power model configuration file name with FILENAME" << endl
         << "\t-instantPow \t Create a file when it is recorded NoC total power at each clock cycle" << endl
         << "\t-appPath PATHNAME \t Set the folder name of application files " << endl
         << "\t-own_header \t When -traffic app is on, consider the first packet data as the packet header in "<< endl << "\t\t\t NoC_tracesIPx files"<< endl
         << "\t-alpha_trace \t Create alpha_file that records switching activity at each router's output" << endl
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
         << "- clock_period = " << GlobalParams::clock_period_ps << "ps" << endl
         << "- simulation_time = " << GlobalParams::simulation_time << endl
         << "- stats_warm_up_time = " << GlobalParams::stats_warm_up_time << endl
         << "- rnd_generator_seed = " << GlobalParams::rnd_generator_seed << endl
         << "- payload_pattern = " << GlobalParams::payload_pattern << endl
         << "- r2r_link_length = " << GlobalParams::r2r_link_length << endl
         << "- flit_size = " << GlobalParams::flit_size << endl
         << "- link_pwr_model = " << GlobalParams::link_pwr_model << endl 
         << "- interconnect filename = "  << GlobalParams::interconnect_model_filename << endl
         << "- appPath = " << GlobalParams::app_traffic_pathname << endl
         << endl;
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
	cerr << "Error: simulation time must be positive " << GlobalParams::simulation_time << endl;
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
    else {
	for (int i = 1; i < arg_num; i++) {
	    if (!strcmp(arg_vet[i], "-help")) {
		showHelp(arg_vet[0]);
		exit(0);
	    } else if (!strcmp(arg_vet[i], "-config")){
            i++;
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
	    else if (!strcmp(arg_vet[i], "-winoc")) 
		GlobalParams::use_winoc = true;
	    else if (!strcmp(arg_vet[i], "-wirxsleep")) 
		GlobalParams::use_wirxsleep = true;
	    else if (!strcmp(arg_vet[i], "-size")) {
		GlobalParams::min_packet_size = atoi(arg_vet[++i]);
		GlobalParams::max_packet_size = atoi(arg_vet[++i]);
	    } else if (!strcmp(arg_vet[i], "-routing")) {
		char *routing = arg_vet[++i];
		strcpy(GlobalParams::routing_algorithm, routing);
		if (!strcmp(routing, ROUTING_DYAD))
		    GlobalParams::dyad_threshold = atof(arg_vet[++i]);
		else if (!strcmp(routing, ROUTING_TABLE_BASED)) {
		    strcpy(GlobalParams::routing_table_filename, arg_vet[++i]);
		    GlobalParams::packet_injection_rate = 0;
		} 
	    } else if (!strcmp(arg_vet[i], "-sel")) {
		char *selection = arg_vet[++i];
		if (!strcmp(selection, "RANDOM"))
		    GlobalParams::selection_strategy = "RANDOM";
		else if (!strcmp(selection, "BUFFER_LEVEL"))
		    GlobalParams::selection_strategy =
			"BUFFER_LEVEL";
		else if (!strcmp(selection, "NOP"))
		    GlobalParams::selection_strategy = "NOP";
		else
		    GlobalParams::selection_strategy =
			"INVALID_SELECTION";
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
		    GlobalParams::probability_of_retransmission = atof(arg_vet[++i]);
		  else assert("Invalid pir format" && false);
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
                else if (!strcmp(traffic, "app"))
		    GlobalParams::traffic_distribution =
			TRAFFIC_APPLICATION;
		else if (!strcmp(traffic, "table")) {
		    GlobalParams::traffic_distribution =
			TRAFFIC_TABLE_BASED;
		    strcpy(GlobalParams::traffic_table_filename,
			   arg_vet[++i]);
		} else if (!strcmp(traffic, "local")) {
		    GlobalParams::traffic_distribution = TRAFFIC_LOCAL;
		    GlobalParams::locality=atof(arg_vet[++i]);
		}
		else assert(false);
	    } else if (!strcmp(arg_vet[i], "-hs")) {
		int node = atoi(arg_vet[++i]);
		double percentage = atof(arg_vet[++i]);
		pair < int, double >t(node, percentage);
		GlobalParams::hotspots.push_back(t);
	    } else if (!strcmp(arg_vet[i], "-appPath"))
                strcpy(GlobalParams::app_traffic_pathname, arg_vet[++i]);
            
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
		GlobalParams::simulation_time = atof(arg_vet[++i]);
            else if (!strcmp(arg_vet[i], "-link_pwr_model")){
		char *link_pwr_mdl = arg_vet[++i];
		if (!strcmp(link_pwr_mdl, "static"))
		    GlobalParams::link_pwr_model =
			STATIC_MODEL;
		else if (!strcmp(link_pwr_mdl, "crosstalk"))
		    GlobalParams::link_pwr_model =
			CROSSTALK_MODEL;
                else if (!strcmp(link_pwr_mdl, "palesi"))
		    GlobalParams::link_pwr_model =
			PALESI_MODEL;
            }else if (!strcmp(arg_vet[i], "-flit_size"))
		GlobalParams::flit_size = atoi(arg_vet[++i]);
            else if (!strcmp(arg_vet[i], "-payload_pattern")) {
		char *payload_pattern = arg_vet[++i];
		if (!strcmp(payload_pattern, "best"))
		    GlobalParams::payload_pattern =
			PAYLOAD_0_BEST;
		else if (!strcmp(payload_pattern, "worst"))
		    GlobalParams::payload_pattern =
			PAYLOAD_100_WORST;
                else if (!strcmp(payload_pattern, "true_rng"))
		    GlobalParams::payload_pattern =
			PAYLOAD_RANDOM;
                else if (!strcmp(payload_pattern, "50_worst"))
		    GlobalParams::payload_pattern =
			PAYLOAD_50_WORST;
                else if (!strcmp(payload_pattern, "50_best"))
		    GlobalParams::payload_pattern =
			PAYLOAD_50_BEST;
                else if (!strcmp(payload_pattern, "25_worst"))
		    GlobalParams::payload_pattern =
			PAYLOAD_25_WORST;
                else if (!strcmp(payload_pattern, "25_best"))
		    GlobalParams::payload_pattern =
			PAYLOAD_25_BEST;
                else if (!strcmp(payload_pattern, "75_worst"))
		    GlobalParams::payload_pattern =
			PAYLOAD_75_WORST;
                else if (!strcmp(payload_pattern, "75_best"))
		    GlobalParams::payload_pattern =
			PAYLOAD_75_BEST;
                else if (!strcmp(payload_pattern, "100_best"))
		    GlobalParams::payload_pattern =
			PAYLOAD_100_BEST;
            } else if (!strcmp(arg_vet[i], "-r2r_length"))
		GlobalParams::r2r_link_length =
		    atoi(arg_vet[++i]);
            else if (!strcmp(arg_vet[i], "-q"))
		GlobalParams::use_script_mode = true;
            else if (!strcmp(arg_vet[i], "-link_config")) {
		strcpy(GlobalParams::interconnect_model_filename, arg_vet[++i]);
	    }
            else if(!strcmp(arg_vet[i], "-instantPow"))
                GlobalParams::get_instant_noc_power = true;
            else if(!strcmp(arg_vet[i], "-own_header"))
                GlobalParams::use_own_header = true;
            else if(!strcmp(arg_vet[i], "-alpha_trace"))
                GlobalParams::alpha_trace = true;
	    else {
		cerr << "Error: Invalid option: " << arg_vet[i] << endl;
		exit(1);
	    }
	}
    }

}


void configure(int arg_num, char *arg_vet[]) {

    bool config_found = false;

    for (int i = 1; i < arg_num; i++) {
	    if (!strcmp(arg_vet[i], "-config")) {
            strcpy(GlobalParams::config_filename, arg_vet[++i]);
            config_found = true;
            break;
        }
    }

    if (!config_found)
    {
        std::ifstream infile(CONFIG_FILENAME);
        if (infile.good())
            strcpy(GlobalParams::config_filename, CONFIG_FILENAME);
        else
        {
            cerr << "No configuration file found!" << endl;
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

