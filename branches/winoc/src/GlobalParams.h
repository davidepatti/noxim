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

#include <vector>

using namespace std;

// Define the directions as numbers
#define DIRECTIONS             4
#define DIRECTION_NORTH        0
#define DIRECTION_EAST         1
#define DIRECTION_SOUTH        2
#define DIRECTION_WEST         3
#define DIRECTION_LOCAL        4
#define DIRECTION_WIRELESS     5

// Generic not reserved resource
#define NOT_RESERVED          -2

// To mark invalid or non exhistent values
#define NOT_VALID             -1

// Routing algorithms
#define ROUTING_XY             0
#define ROUTING_WEST_FIRST     1
#define ROUTING_NORTH_LAST     2
#define ROUTING_NEGATIVE_FIRST 3
#define ROUTING_ODD_EVEN       4
#define ROUTING_DYAD           5
#define ROUTING_FULLY_ADAPTIVE 8
#define ROUTING_TABLE_BASED    9
#define INVALID_ROUTING       -1

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

// TODO by Fafa - this MUST be removed!!! Use only STL vectors instead!!!
#define MAX_STATIC_DIM 32

// TODO check the list of attributes for ChannelConf 
typedef struct {
    int BER;
    int delay;
} ChannelConf;

typedef struct {
    int * attachedNodes;
    int * txChannels;
    int * rxChannels;
    int attachedNodes_num;
    int txChannels_num;
    int rxChannels_num;
} HubConf;


struct GlobalParams {
    static int verbose_mode;
    static int trace_mode;
    static char trace_filename[128];
    static int mesh_dim_x;
    static int mesh_dim_y;
    static int buffer_depth;
    static int min_packet_size;
    static int max_packet_size;
    static int routing_algorithm;
    static char routing_table_filename[128];
    static int selection_strategy;
    static float packet_injection_rate;
    static float probability_of_retransmission;
    static int traffic_distribution;
    static char traffic_table_filename[128];
    static int simulation_time;
    static int reset_time;
    static int stats_warm_up_time;
    static int rnd_generator_seed;
    static bool detailed;
    static vector <pair <int, double> > hotspots;
    static float dyad_threshold;
    static unsigned int max_volume_to_be_drained;
    static char router_power_filename[128];
    static bool low_power_link_strategy;
    static double qos;
    static bool show_buffer_stats;
    static bool use_winoc;
    static int channels_num;
    static int hubs_num;
    static ChannelConf *channel_conf;
    static HubConf *hub_conf;
};

#endif
