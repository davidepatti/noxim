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

    wireless_rx_pwr = 0.0;
    transceiver_pwr_s = 0.0;
    transceiver_pwr_biasing = 0.0;
    wireless_snooping = 0.0;

    routing_pwr_d = 0.0;
    routing_pwr_s = 0.0;

    selection_pwr_d = 0.0;
    selection_pwr_s = 0.0;

    crossbar_pwr_d = 0.0;
    crossbar_pwr_s = 0.0;

    link_r2r_pwr_d = 0.0;
    link_r2r_pwr_s = 0.0;
    link_r2h_pwr_s = 0.0;
    link_r2h_pwr_d = 0.0;

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
    
    assert(GlobalParams::power_configuration.bufferPowerConfig.leakage.find(key) != GlobalParams::power_configuration.bufferPowerConfig.leakage.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.push.find(key) != GlobalParams::power_configuration.bufferPowerConfig.push.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.front.find(key) != GlobalParams::power_configuration.bufferPowerConfig.front.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.pop.find(key) != GlobalParams::power_configuration.bufferPowerConfig.pop.end());

    buffer_pwr_s = GlobalParams::power_configuration.bufferPowerConfig.leakage[key];
    buffer_push_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.push[key];
    buffer_front_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.front[key];
    buffer_pop_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.pop[key];

    // Routing 
    assert(GlobalParams::power_configuration.routerPowerConfig.routing_algorithm_pm.find(routing_function) != GlobalParams::power_configuration.routerPowerConfig.routing_algorithm_pm.end());

    routing_pwr_s = GlobalParams::power_configuration.routerPowerConfig.routing_algorithm_pm[routing_function].first;
    routing_pwr_d = GlobalParams::power_configuration.routerPowerConfig.routing_algorithm_pm[routing_function].second;

    // Selection 
    assert(GlobalParams::power_configuration.routerPowerConfig.selection_strategy_pm.find(selection_function) != GlobalParams::power_configuration.routerPowerConfig.selection_strategy_pm.end());

    selection_pwr_s = GlobalParams::power_configuration.routerPowerConfig.selection_strategy_pm[selection_function].first;
    selection_pwr_d = GlobalParams::power_configuration.routerPowerConfig.selection_strategy_pm[selection_function].second;

    // CrossBar
    // TODO future work: tuning of crossbar radix
    pair<int,int> xbar_k = pair<int,int>(5,GlobalParams::flit_size);
    crossbar_pwr_s = GlobalParams::power_configuration.routerPowerConfig.crossbar_pm[xbar_k].first;
    crossbar_pwr_d = GlobalParams::power_configuration.routerPowerConfig.crossbar_pm[xbar_k].second;
    
    // NetworkInterface
    ni_pwr_s = GlobalParams::power_configuration.routerPowerConfig.network_interface.first;
    ni_pwr_d = GlobalParams::power_configuration.routerPowerConfig.network_interface.second;

    // Link 
    // TODO TURI: aggiungere nel file di configurazione la
    // possibilita' di specificare le lunghezze di connessione
    // Router/Router Router/Hub:

    // Router has both type of links
    double length_r2h = 1.0; // TODO TURI GlobalParams::power_configuration.r2h_link_length;
    double length_r2r = 1.0; // TODO TURI GlobalParams::power_configuration.r2r_link_length;
    assert(GlobalParams::power_configuration.linkBitLinePowerConfig.find(length_r2r)!=GlobalParams::power_configuration.linkBitLinePowerConfig.end());
    assert(GlobalParams::power_configuration.linkBitLinePowerConfig.find(length_r2h)!=GlobalParams::power_configuration.linkBitLinePowerConfig.end());

    link_r2r_pwr_s= link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2r].first;
    link_r2r_pwr_d= link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2r].second;
    link_r2h_pwr_s= link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2h].first;
    link_r2h_pwr_d= link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2h].second;
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
    
    assert(GlobalParams::power_configuration.bufferPowerConfig.leakage.find(key) != GlobalParams::power_configuration.bufferPowerConfig.leakage.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.push.find(key) != GlobalParams::power_configuration.bufferPowerConfig.push.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.front.find(key) != GlobalParams::power_configuration.bufferPowerConfig.front.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.pop.find(key) != GlobalParams::power_configuration.bufferPowerConfig.pop.end());

    buffer_pwr_s = GlobalParams::power_configuration.bufferPowerConfig.leakage[key];
    buffer_push_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.push[key];
    buffer_front_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.front[key];
    buffer_pop_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.pop[key];

    // Buffer Antenna
    pair<int,int> akey = pair<int,int>(antenna_buffer_depth,antenna_buffer_size);
    
    assert(GlobalParams::power_configuration.bufferPowerConfig.leakage.find(akey) != GlobalParams::power_configuration.bufferPowerConfig.leakage.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.push.find(akey) != GlobalParams::power_configuration.bufferPowerConfig.push.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.front.find(akey) != GlobalParams::power_configuration.bufferPowerConfig.front.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.pop.find(akey) != GlobalParams::power_configuration.bufferPowerConfig.pop.end());

    antenna_buffer_pwr_s = GlobalParams::power_configuration.bufferPowerConfig.leakage[akey];
    antenna_buffer_push_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.push[akey];
    antenna_buffer_front_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.front[akey];
    antenna_buffer_pop_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.pop[akey];

    attenuation_map = GlobalParams::power_configuration.hubPowerConfig.transmitter_attenuation_map;

    wireless_rx_pwr = 1.0; // TODO TURI antenna_buffer_size * GlobalParams::power_configuration.hubPowerConfig.rx_dynamic;
    wireless_snooping =  1.0; // TODO TURIGlobalParams::power_configuration.hubPowerConfig.rx_snooping;

    transceiver_pwr_s = GlobalParams::power_configuration.hubPowerConfig.transceiver_leakage.first +
        GlobalParams::power_configuration.hubPowerConfig.transceiver_leakage.second;


    transceiver_pwr_biasing = GlobalParams::power_configuration.hubPowerConfig.transceiver_biasing.first +
        GlobalParams::power_configuration.hubPowerConfig.transceiver_biasing.second;
    // Link 
    // TODO TURI: aggiungere nel file di configurazione la
    // possibilita' di specificare le lunghezze di connessione
    // Hub/Router

    // Hub has only Router/Hub link connections
    double length_r2h = 1.0; // TODO TURI GlobalParams::power_configuration.r2h_link_length;
    assert(GlobalParams::power_configuration.linkBitLinePowerConfig.find(length_r2h)!=GlobalParams::power_configuration.linkBitLinePowerConfig.end());

    link_r2h_pwr_s= link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2h].first;
    link_r2h_pwr_d= link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2h].second;
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

void Power::r2rLink()
{
    total_power_d+=link_r2r_pwr_d;
}

void Power::r2hLink()
{
    total_power_d+=link_r2h_pwr_d;
}

void Power::networkInterface()
{
    total_power_d+=ni_pwr_d;
}


double Power::attenuation2power(double attenuation)
{
    // TODO
    return attenuation;
}



void Power::wirelessTx(int src,int dst,int length)
{
    pair<int,int> key = pair<int,int>(src,dst);
    assert(attenuation_map.find(key)!=attenuation_map.end());

    total_power_d += attenuation2power(attenuation_map[key]) * length;
}

void Power::wirelessDynamicRx(int no_receivers)
{
    total_power_d+= wireless_rx_pwr*no_receivers;
}

void Power::wirelessSnooping()
{
    total_power_d += wireless_snooping;
}

void Power::biasing()
{
    total_power_s+= transceiver_pwr_biasing;
}

void Power::leakage()
{
    total_power_s+= buffer_pwr_s+
	            antenna_buffer_pwr_s+
		    routing_pwr_s+
		    selection_pwr_s+
		    crossbar_pwr_s+
		    link_r2r_pwr_s+
		    link_r2h_pwr_s+
		    transceiver_pwr_s+
		    ni_pwr_s;
}
