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
#include "Utils.h"

using namespace std;


Power::Power()
{
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
    
    for (map<double,pair<double,double> > ::iterator i = GlobalParams::power_configuration.linkBitLinePowerConfig.begin();
	    i!=GlobalParams::power_configuration.linkBitLinePowerConfig.end(); i++)
	cout << i->first << " " << i->second.first << " " << i->second.second << endl;

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
    power_breakdown_d["buffer_push_pwr_d"] += buffer_push_pwr_d;
}

void Power::bufferPop()
{
    power_breakdown_d["buffer_pop_pwr_d"]+= buffer_pop_pwr_d;
}

void Power::bufferFront()
{
    power_breakdown_d["buffer_front_pwr_d"]+= buffer_front_pwr_d;
}

void Power::antennaBufferPush()
{
    power_breakdown_d["antenna_buffer_push_pwr_d"]+= antenna_buffer_push_pwr_d;
}

void Power::antennaBufferPop()
{
    power_breakdown_d["antenna_buffer_pop_pwr_d"]+= antenna_buffer_pop_pwr_d;
}

void Power::antennaBufferFront()
{
    power_breakdown_d["antenna_buffer_front_pwr_d"]+= antenna_buffer_front_pwr_d;
}

void Power::routing()
{
    power_breakdown_d["routing_pwr_d"]+= routing_pwr_d;
}

void Power::selection()
{
    power_breakdown_d["selection_pwr_d"]+=selection_pwr_d ;
}

void Power::crossBar()
{
    power_breakdown_d["crossbar_pwr_d"]+=crossbar_pwr_d;
}

void Power::r2rLink()
{
    power_breakdown_d["link_r2r_pwr_d"]+=link_r2r_pwr_d;
}

void Power::r2hLink()
{
    power_breakdown_d["link_r2h_pwr_d"]+=link_r2h_pwr_d;
}

void Power::networkInterface()
{
    power_breakdown_d["ni_pwr_d"]+=ni_pwr_d;
}


double Power::getDynamicPower()
{
    double power = 0.0;
    for (map<string,double>::iterator i = power_breakdown_d.begin(); i!=power_breakdown_d.end(); i++)
	power+= i->second;

    return power;
}

double Power::getStaticPower()
{
    double power = 0.0;
    for (map<string,double>::iterator i = power_breakdown_s.begin(); i!=power_breakdown_s.end(); i++)
	power+= i->second;

    return power;
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

    power_breakdown_d["wireless_tx"] += attenuation2power(attenuation_map[key]) * length;
}

void Power::wirelessDynamicRx(int no_receivers)
{
    power_breakdown_d["wireless_dynamic_rx_pwr"]+= wireless_rx_pwr*no_receivers;
}

void Power::wirelessSnooping()
{
    power_breakdown_d["wireless_snooping"] += wireless_snooping;
}

void Power::biasing()
{
    power_breakdown_s["transceiver_pwr_biasing"] += transceiver_pwr_biasing;
}

void Power::leakage()
{
    power_breakdown_s["buffer_pwr_s"]+=buffer_pwr_s;
    power_breakdown_s["antenna_buffer_pwr_s"]+=antenna_buffer_pwr_s;
    power_breakdown_s["routing_pwr_s"]+=routing_pwr_s;
    power_breakdown_s["selection_pwr_s"]+=selection_pwr_s;
    power_breakdown_s["crossbar_pwr_s"]+=crossbar_pwr_s;
    power_breakdown_s["link_r2r_pwr_s"]+=link_r2r_pwr_s;
    power_breakdown_s["link_r2h_pwr_s"]+=link_r2h_pwr_s;
    power_breakdown_s["transceiver_pwr_s"]+=transceiver_pwr_s;
    power_breakdown_s["ni_pwr_s"]+=ni_pwr_s;
}



void Power::printBreakDown(std::ostream & out)
{
    printMap("power_breakdown_d",power_breakdown_d,cout);
    printMap("power_breakdown_s",power_breakdown_s,cout);
}
    



