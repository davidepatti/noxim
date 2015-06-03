/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2015 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the command line parser
 */

#include "GlobalParams.h"

int GlobalParams::verbose_mode;
int GlobalParams::trace_mode;
char GlobalParams::trace_filename[128];
int GlobalParams::mesh_dim_x;
int GlobalParams::mesh_dim_y;
float GlobalParams::r2r_link_length;
float GlobalParams::r2h_link_length;
int GlobalParams::buffer_depth;
int GlobalParams::flit_size;
int GlobalParams::min_packet_size;
int GlobalParams::max_packet_size;
char GlobalParams::routing_algorithm[128];
char GlobalParams::routing_table_filename[128];
string GlobalParams::selection_strategy;
float GlobalParams::packet_injection_rate;
float GlobalParams::probability_of_retransmission;
float GlobalParams::locality;
int GlobalParams::traffic_distribution;
char GlobalParams::traffic_table_filename[128];
char GlobalParams::config_filename[128];
int GlobalParams::clock_period_ps;
int GlobalParams::simulation_time;
int GlobalParams::reset_time;
int GlobalParams::stats_warm_up_time;
int GlobalParams::rnd_generator_seed;
bool GlobalParams::detailed;
float GlobalParams::dyad_threshold;
unsigned int GlobalParams::max_volume_to_be_drained;
vector <pair <int, double> > GlobalParams::hotspots;
bool GlobalParams::show_buffer_stats;
bool GlobalParams::use_winoc;
bool GlobalParams::use_wirxsleep;
ChannelConfig GlobalParams::default_channel_configuration;
map<int, ChannelConfig> GlobalParams::channel_configuration;
HubConfig GlobalParams::default_hub_configuration;
map<int, HubConfig> GlobalParams::hub_configuration;
map<int, int> GlobalParams::hub_for_tile;
PowerConfig GlobalParams::power_configuration;
