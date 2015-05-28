/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the global params needed by Noxim
 * to forward configuration to every sub-block
 */

#ifndef __NOXIMGLOBALPARAMS_H__
#define __NOXIMGLOBALPARAMS_H__ 

#include <map>
#include <utility>
#include <vector>
#include <string>

using namespace std;

#define CONFIG_FILENAME        "config.yaml"

// Define the directions as numbers
#define DIRECTIONS             4
#define DIRECTION_NORTH        0
#define DIRECTION_EAST         1
#define DIRECTION_SOUTH        2
#define DIRECTION_WEST         3
#define DIRECTION_LOCAL        4
#define DIRECTION_HUB          5
#define DIRECTION_WIRELESS     747

// Generic not reserved resource
#define NOT_RESERVED          -2

// To mark invalid or non exhistent values
#define NOT_VALID             -1

// Routing algorithms
#define ROUTING_DYAD           "DYAD"
#define ROUTING_TABLE_BASED    "TABLE_BASED"

// Selection strategies
#define SEL_RANDOM             0
#define SEL_BUFFER_LEVEL       1
#define SEL_NOP                2
#define INVALID_SELECTION     -1

// Traffic distribution
#define TRAFFIC_RANDOM         0
#define TRAFFIC_TRANSPOSE1     1
#define TRAFFIC_TRANSPOSE2     2
#define TRAFFIC_HOTSPOT        3
#define TRAFFIC_TABLE_BASED    4
#define TRAFFIC_BIT_REVERSAL   5
#define TRAFFIC_SHUFFLE        6
#define TRAFFIC_BUTTERFLY      7
#define INVALID_TRAFFIC       -1

// Verbosity levels
#define VERBOSE_OFF            0
#define VERBOSE_LOW            1
#define VERBOSE_MEDIUM         2
#define VERBOSE_HIGH           3


// Wireless MAC constants
#define RELEASE_CHANNEL 1
#define HOLD_CHANNEL 	2

#define TOKEN_HOLD 	0
#define TOKEN_MAX_HOLD 	1
#define TOKEN_PACKET 	2

// TODO check the list of attributes for ChannelConfig 
typedef struct {
    pair<int, int> ber;
    int dataRate;
    vector<string> macPolicy;
} ChannelConfig;

typedef struct {
    vector<int> attachedNodes;
    vector<int> rxChannels;
    vector<int> txChannels;
    int toTileBufferSize;
    int fromTileBufferSize;
    int txBufferSize;
    int rxBufferSize;
} HubConfig;

typedef struct {
    map<pair <int, int>, double> front;
    map<pair <int, int>, double> pop;
    map<pair <int, int>, double> push;
    map<pair <int, int>, double> leakage;
} BufferPowerConfig;

typedef map<double, pair <double, double> > LinkBitLinePowerConfig;

typedef struct {
    map<pair<double, double>, pair<double, double> > crossbar_pm;
    map<int, pair<double, double> > network_interface;
    map<string, pair<double, double> > routing_algorithm_pm;
    map<string, pair<double, double> > selection_strategy_pm;
} RouterPowerConfig;

typedef struct {
    pair<double, double> transceiver_leakage;
    pair<double, double> transceiver_biasing;
    double rx_dynamic;
    double rx_snooping;
    double default_tx_energy;
    map<pair <int, int>, double> transmitter_attenuation_map;
} HubPowerConfig;

typedef struct {
    BufferPowerConfig bufferPowerConfig;
    LinkBitLinePowerConfig linkBitLinePowerConfig;
    RouterPowerConfig routerPowerConfig;
    HubPowerConfig hubPowerConfig;
} PowerConfig;

struct GlobalParams {
    static int verbose_mode;
    static int trace_mode;
    static char trace_filename[128];
    static int mesh_dim_x;
    static int mesh_dim_y;
    static float r2r_link_length;
    static float r2h_link_length;
    static int buffer_depth;
    static int flit_size;
    static int min_packet_size;
    static int max_packet_size;
    static char routing_algorithm[128];
    static char routing_table_filename[128];
    static string selection_strategy;
    static float packet_injection_rate;
    static float probability_of_retransmission;
    static int traffic_distribution;
    static char traffic_table_filename[128];
    static char config_filename[128];
    static int clock_period;
    static int simulation_time;
    static int reset_time;
    static int stats_warm_up_time;
    static int rnd_generator_seed;
    static bool detailed;
    static vector <pair <int, double> > hotspots;
    static float dyad_threshold;
    static unsigned int max_volume_to_be_drained;
    static bool show_buffer_stats;
    static bool use_winoc;
    static bool use_wirxsleep;
    static ChannelConfig default_channel_configuration;
    static map<int, ChannelConfig> channel_configuration;
    static HubConfig default_hub_configuration;
    static map<int, HubConfig> hub_configuration;
    static map<int, int> hub_for_tile;
    static PowerConfig power_configuration;
};

#endif
