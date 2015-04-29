/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the power model
 */

#include <iostream>
#include "Power.h"
using namespace std;


Power::Power()
{
    total_power_d = 0.0;
    total_power_s = 0.0;

    buffer_push_pwr_d = 0.0;
    buffer_pop_pwr_d = 0.0;
    buffer_front_pwr_d = 0.0;
    buffer_pwr_s = 0.0;
    
    antenna_buffer_push_pwr_d = 0.0;
    antenna_buffer_pop_pwr_d = 0.0;
    antenna_buffer_front_pwr_d = 0.0;
    antenna_buffer_pwr_s = 0.0;

    flit_wireless_rx_pwr = 0.0;
    transceiver_pwr_s = 0.0;
    wireless_snooping = 0.0;

    routing_pwr_d = 0.0;
    routing_pwr_s = 0.0;

    selection_pwr_d = 0.0;
    selection_pwr_s = 0.0;

    crossbar_pwr_d = 0.0;
    crossbar_pwr_s = 0.0;

    link_pwr_d = 0.0;
    link_pwr_s = 0.0;

    ni_pwr_d = 0.0;
    ni_pwr_s = 0.0;

}

void Power::configureRouter(int link_width,
	int buffer_depth,
	int buffer_size,
	string routing_function,
	string selection_function)
{
// (s)tatic, (d)ynamic power

    // Buffer 
    pair<int,int> key = pair<int,int>(buffer_depth, buffer_size);
    
    assert(GlobalParams::power_configuration.bufferPowerConfig.leakage_pm.find(key) != GlobalParams::power_configuration.bufferPowerConfig.leakage_pm.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.push_pm.find(key) != GlobalParams::power_configuration.bufferPowerConfig.push_pm.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.front_pm.find(key) != GlobalParams::power_configuration.bufferPowerConfig.front_pm.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.pop_pm.find(key) != GlobalParams::power_configuration.bufferPowerConfig.pop_pm.end());

    buffer_pwr_s = GlobalParams::power_configuration.bufferPowerConfig.leakage_pm[key];
    buffer_push_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.push_pm[key];
    buffer_front_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.front_pm[key];
    buffer_pop_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.pop_pm[key];

    // Routing 
    assert(GlobalParams::power_configuration.routerPowerConfig.routing_algorithm_pm.find(routing_function) != GlobalParams::power_configuration.routerPowerConfig.routing_algorithm_pm.end());

    routing_pwr_s = GlobalParams::power_configuration.routerPowerConfig.routing_algorithm_pm[routing_function].first;
    routing_pwr_d = GlobalParams::power_configuration.routerPowerConfig.routing_algorithm_pm[routing_function].second;

    // Selection 
    assert(GlobalParams::power_configuration.routerPowerConfig.selection_strategy_pm.find(selection_function) != GlobalParams::power_configuration.routerPowerConfig.selection_strategy_pm.end());

    selection_pwr_s = GlobalParams::power_configuration.routerPowerConfig.selection_strategy_pm[selection_function].first;
    selection_pwr_d = GlobalParams::power_configuration.routerPowerConfig.selection_strategy_pm[selection_function].second;

    // CrossBar
    crossbar_pwr_s = GlobalParams::power_configuration.routerPowerConfig.crossbar_pm.first;
    crossbar_pwr_d = GlobalParams::power_configuration.routerPowerConfig.crossbar_pm.second;
    
    // Link 
    link_pwr_s= link_width * GlobalParams::power_configuration.routerPowerConfig.link_bit_line_pm.first;
    link_pwr_d= link_width * GlobalParams::power_configuration.routerPowerConfig.link_bit_line_pm.second;

    // NetworkInterface
    ni_pwr_s = GlobalParams::power_configuration.routerPowerConfig.network_interface_pm.first;
    ni_pwr_d = GlobalParams::power_configuration.routerPowerConfig.network_interface_pm.second;
}

void Power::configureHub(int link_width,
	int buffer_depth,
	int buffer_size,
	int antenna_buffer_depth,
	int antenna_buffer_size)
{
// (s)tatic, (d)ynamic power

    // Buffer 
    pair<int,int> key = pair<int,int>(buffer_depth, buffer_size);
    
    assert(GlobalParams::power_configuration.bufferPowerConfig.leakage_pm.find(key) != GlobalParams::power_configuration.bufferPowerConfig.leakage_pm.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.push_pm.find(key) != GlobalParams::power_configuration.bufferPowerConfig.push_pm.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.front_pm.find(key) != GlobalParams::power_configuration.bufferPowerConfig.front_pm.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.pop_pm.find(key) != GlobalParams::power_configuration.bufferPowerConfig.pop_pm.end());

    buffer_pwr_s = GlobalParams::power_configuration.bufferPowerConfig.leakage_pm[key];
    buffer_push_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.push_pm[key];
    buffer_front_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.front_pm[key];
    buffer_pop_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.pop_pm[key];

    // Buffer Antenna
    pair<int,int> akey = pair<int,int>(antenna_buffer_depth,antenna_buffer_size);
    
    assert(GlobalParams::power_configuration.bufferPowerConfig.leakage_pm.find(akey) != GlobalParams::power_configuration.bufferPowerConfig.leakage_pm.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.push_pm.find(akey) != GlobalParams::power_configuration.bufferPowerConfig.push_pm.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.front_pm.find(akey) != GlobalParams::power_configuration.bufferPowerConfig.front_pm.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.pop_pm.find(akey) != GlobalParams::power_configuration.bufferPowerConfig.pop_pm.end());

    antenna_buffer_pwr_s = GlobalParams::power_configuration.bufferPowerConfig.leakage_pm[akey];
    antenna_buffer_push_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.push_pm[akey];
    antenna_buffer_front_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.front_pm[akey];
    antenna_buffer_pop_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.pop_pm[akey];

    bit_wireless_tx_pwr = GlobalParams::power_configuration.hubPowerConfig.transmitter_pm;
    //TODO: add default power values for SRC/DST couples not defined in the transmission power map

    flit_wireless_rx_pwr = antenna_buffer_size * GlobalParams::power_configuration.hubPowerConfig.receiver_pm;

    transceiver_pwr_s = GlobalParams::power_configuration.hubPowerConfig.transceiver_leakage_pm;
}


void Power::bufferPush()
{
    total_power_d+= buffer_push_pwr_d;
}

void Power::bufferPop()
{
    total_power_d+= buffer_pop_pwr_d;
}

void Power::bufferFront()
{
    total_power_d+= buffer_front_pwr_d;
}

void Power::antennaBufferPush()
{
    total_power_d+= antenna_buffer_push_pwr_d;
}

void Power::antennaBufferPop()
{
    total_power_d+= antenna_buffer_pop_pwr_d;
}

void Power::antennaBufferFront()
{
    total_power_d+= antenna_buffer_front_pwr_d;
}

void Power::routing()
{
    total_power_d+= routing_pwr_d;
}

void Power::selection()
{
    total_power_d+= selection_pwr_d;
}

void Power::crossBar()
{
    total_power_d+=crossbar_pwr_d;
}

void Power::link()
{
    total_power_d+=link_pwr_d;
}

void Power::networkInterface()
{
    total_power_d+=ni_pwr_d;
}


void Power::wirelessTx(int src,int dst,int length)
{
    pair<int,int> key = pair<int,int>(src,dst);

    assert(bit_wireless_tx_pwr.find(key)!=bit_wireless_tx_pwr.end());

    total_power_d += bit_wireless_tx_pwr[key] * length;

}

void Power::wirelessTotalRx(int no_receivers)
{
    total_power_d+= flit_wireless_rx_pwr*no_receivers;
}

void Power::wirelessSnooping()
{
    total_power_d += wireless_snooping;
}


void Power::leakage()
{
    total_power_s+= buffer_pwr_s+
	            antenna_buffer_pwr_s+
		    routing_pwr_s+
		    selection_pwr_s+
		    crossbar_pwr_s+
		    link_pwr_s+
		    transceiver_pwr_s+
		    ni_pwr_s;
}
