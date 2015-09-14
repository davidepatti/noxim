/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2015 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the power model
 */

#include <iostream>
#include "Power.h"
#include "Utils.h"
#include "systemc.h"

#define W2J(watt) ((watt)*GlobalParams::clock_period_ps*1.0e-12)

using namespace std;


Power::Power()
{
    total_power_s = 0.0;

    buffer_router_push_pwr_d = 0.0;
    buffer_router_pop_pwr_d = 0.0;
    buffer_router_front_pwr_d = 0.0;
    buffer_router_pwr_s = 0.0;
    
    buffer_to_tile_push_pwr_d = 0.0;
    buffer_to_tile_pop_pwr_d = 0.0;
    buffer_to_tile_front_pwr_d = 0.0;
    buffer_to_tile_pwr_s = 0.0;

    buffer_from_tile_push_pwr_d = 0.0;
    buffer_from_tile_pop_pwr_d = 0.0;
    buffer_from_tile_front_pwr_d = 0.0;
    buffer_from_tile_pwr_s = 0.0;

    antenna_buffer_push_pwr_d = 0.0;
    antenna_buffer_pop_pwr_d = 0.0;
    antenna_buffer_front_pwr_d = 0.0;
    antenna_buffer_pwr_s = 0.0;

    wireless_rx_pwr = 0.0;
    transceiver_tx_pwr_s = 0.0;
    transceiver_rx_pwr_s = 0.0;
    transceiver_tx_pwr_biasing = 0.0;
    transceiver_rx_pwr_biasing = 0.0;
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

    default_tx_energy = 0.0;

    ni_pwr_d = 0.0;
    ni_pwr_s = 0.0;


    sleep_end_cycle = NOT_VALID;

    initPowerBreakdown();
}

void Power::configureRouter(int link_width,
	int buffer_depth,
	int buffer_item_size,
	string routing_function,
	string selection_function)
{
// (s)tatic, (d)ynamic power

    // Buffer 
    pair<int,int> key = pair<int,int>(buffer_depth, buffer_item_size);
    
    assert(GlobalParams::power_configuration.bufferPowerConfig.leakage.find(key) != GlobalParams::power_configuration.bufferPowerConfig.leakage.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.push.find(key) != GlobalParams::power_configuration.bufferPowerConfig.push.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.front.find(key) != GlobalParams::power_configuration.bufferPowerConfig.front.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.pop.find(key) != GlobalParams::power_configuration.bufferPowerConfig.pop.end());

    // Dynamic values are expressed in Joule
    // Static/Leakage values must be converted from Watt to Joule

    buffer_router_pwr_s = W2J(GlobalParams::power_configuration.bufferPowerConfig.leakage[key]);
    buffer_router_push_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.push[key];
    buffer_router_front_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.front[key];
    buffer_router_pop_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.pop[key];

    // Routing 
    assert(GlobalParams::power_configuration.routerPowerConfig.routing_algorithm_pm.find(routing_function) != GlobalParams::power_configuration.routerPowerConfig.routing_algorithm_pm.end());

    routing_pwr_s = W2J(GlobalParams::power_configuration.routerPowerConfig.routing_algorithm_pm[routing_function].first);
    routing_pwr_d = GlobalParams::power_configuration.routerPowerConfig.routing_algorithm_pm[routing_function].second;

    // Selection 
    assert(GlobalParams::power_configuration.routerPowerConfig.selection_strategy_pm.find(selection_function) != GlobalParams::power_configuration.routerPowerConfig.selection_strategy_pm.end());

    selection_pwr_s = W2J(GlobalParams::power_configuration.routerPowerConfig.selection_strategy_pm[selection_function].first);
    selection_pwr_d = GlobalParams::power_configuration.routerPowerConfig.selection_strategy_pm[selection_function].second;

    // CrossBar
    // TODO future work: tuning of crossbar radix
    pair<int,int> xbar_k = pair<int,int>(5,GlobalParams::flit_size);
    crossbar_pwr_s = W2J(GlobalParams::power_configuration.routerPowerConfig.crossbar_pm[xbar_k].first);
    crossbar_pwr_d = GlobalParams::power_configuration.routerPowerConfig.crossbar_pm[xbar_k].second;
    
    // NetworkInterface
    ni_pwr_s = W2J(GlobalParams::power_configuration.routerPowerConfig.network_interface[GlobalParams::flit_size].first);
    ni_pwr_d = GlobalParams::power_configuration.routerPowerConfig.network_interface[GlobalParams::flit_size].second;

    // Link 
    // Router has both type of links
    double length_r2h = GlobalParams::r2h_link_length;
    double length_r2r = GlobalParams::r2r_link_length;
    
    assert(GlobalParams::power_configuration.linkBitLinePowerConfig.find(length_r2r)!=GlobalParams::power_configuration.linkBitLinePowerConfig.end());
    assert(GlobalParams::power_configuration.linkBitLinePowerConfig.find(length_r2h)!=GlobalParams::power_configuration.linkBitLinePowerConfig.end());


    link_r2r_pwr_s= W2J(link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2r].first);
    link_r2r_pwr_d= link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2r].second;
    link_r2h_pwr_s= W2J(link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2h].first);
    link_r2h_pwr_d= link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2h].second;
}

void Power::configureHub(int link_width,
	int buffer_to_tile_depth, // buffer to tile
	int buffer_from_tile_depth, // buffer from tile
	int buffer_item_size,
	int antenna_buffer_depth, // rx/tx antenna buffers
	int antenna_buffer_item_size,
	int data_rate_gbs)
{
// (s)tatic, (d)ynamic power

    // Buffer 
    pair<int,int> key_to_tile = pair<int,int>(buffer_to_tile_depth, buffer_item_size);
    pair<int,int> key_from_tile = pair<int,int>(buffer_from_tile_depth, buffer_item_size);
    
    assert(GlobalParams::power_configuration.bufferPowerConfig.leakage.find(key_to_tile) != GlobalParams::power_configuration.bufferPowerConfig.leakage.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.push.find(key_to_tile) != GlobalParams::power_configuration.bufferPowerConfig.push.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.front.find(key_to_tile) != GlobalParams::power_configuration.bufferPowerConfig.front.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.pop.find(key_to_tile) != GlobalParams::power_configuration.bufferPowerConfig.pop.end());

    assert(GlobalParams::power_configuration.bufferPowerConfig.leakage.find(key_from_tile) != GlobalParams::power_configuration.bufferPowerConfig.leakage.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.push.find(key_from_tile) != GlobalParams::power_configuration.bufferPowerConfig.push.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.front.find(key_from_tile) != GlobalParams::power_configuration.bufferPowerConfig.front.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.pop.find(key_from_tile) != GlobalParams::power_configuration.bufferPowerConfig.pop.end());

    buffer_to_tile_pwr_s = W2J(GlobalParams::power_configuration.bufferPowerConfig.leakage[key_to_tile]);
    buffer_to_tile_push_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.push[key_to_tile];
    buffer_to_tile_front_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.front[key_to_tile];
    buffer_to_tile_pop_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.pop[key_to_tile];

    buffer_from_tile_pwr_s = W2J(GlobalParams::power_configuration.bufferPowerConfig.leakage[key_from_tile]);
    buffer_from_tile_push_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.push[key_from_tile];
    buffer_from_tile_front_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.front[key_from_tile];
    buffer_from_tile_pop_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.pop[key_from_tile];
   
    // Buffer Antenna
    pair<int,int> akey = pair<int,int>(antenna_buffer_depth,antenna_buffer_item_size);
    
    assert(GlobalParams::power_configuration.bufferPowerConfig.leakage.find(akey) != GlobalParams::power_configuration.bufferPowerConfig.leakage.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.push.find(akey) != GlobalParams::power_configuration.bufferPowerConfig.push.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.front.find(akey) != GlobalParams::power_configuration.bufferPowerConfig.front.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.pop.find(akey) != GlobalParams::power_configuration.bufferPowerConfig.pop.end());

    antenna_buffer_pwr_s = W2J(GlobalParams::power_configuration.bufferPowerConfig.leakage[akey]);
    antenna_buffer_push_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.push[akey];
    antenna_buffer_front_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.front[akey];
    antenna_buffer_pop_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.pop[akey];

    attenuation_map = GlobalParams::power_configuration.hubPowerConfig.transmitter_attenuation_map;


    // TX
    // Joule
    default_tx_energy = (GlobalParams::power_configuration.hubPowerConfig.default_tx_energy / (10e9*data_rate_gbs) )* antenna_buffer_item_size;

    // RX Dynamic
    wireless_rx_pwr = antenna_buffer_item_size * GlobalParams::power_configuration.hubPowerConfig.rx_dynamic;
    
    // RX snooping
    wireless_snooping = GlobalParams::power_configuration.hubPowerConfig.rx_snooping;

    // RX leakage
    transceiver_rx_pwr_s = W2J(GlobalParams::power_configuration.hubPowerConfig.transceiver_leakage.first);
    // TX leakage
    transceiver_tx_pwr_s = W2J(GlobalParams::power_configuration.hubPowerConfig.transceiver_leakage.second);
   
    // RX biasing
    transceiver_rx_pwr_biasing = W2J(GlobalParams::power_configuration.hubPowerConfig.transceiver_biasing.first);
    // TX biasing
    transceiver_tx_pwr_biasing = W2J(GlobalParams::power_configuration.hubPowerConfig.transceiver_biasing.second);
    // Link 
    // Hub has only Router/Hub link connections
    double length_r2h = GlobalParams::r2h_link_length;
    assert(GlobalParams::power_configuration.linkBitLinePowerConfig.find(length_r2h)!=GlobalParams::power_configuration.linkBitLinePowerConfig.end());

    link_r2h_pwr_s= W2J(link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2h].first);
    link_r2h_pwr_d= link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2h].second;

}


// Router buffer
void Power::bufferRouterPush()
{
    power_dynamic.breakdown[BUFFER_PUSH_PWR_D].value += buffer_router_push_pwr_d;
}

void Power::bufferRouterPop()
{
    power_dynamic.breakdown[BUFFER_POP_PWR_D].value += buffer_router_pop_pwr_d;
}

void Power::bufferRouterFront()
{
    power_dynamic.breakdown[BUFFER_FRONT_PWR_D].value += buffer_router_front_pwr_d;
}

// Hub to tile
void Power::bufferToTilePush()
{
    power_dynamic.breakdown[BUFFER_TO_TILE_PUSH_PWR_D].value += buffer_to_tile_push_pwr_d;
}

void Power::bufferToTilePop()
{
    power_dynamic.breakdown[BUFFER_TO_TILE_POP_PWR_D].value += buffer_to_tile_pop_pwr_d;
}

void Power::bufferToTileFront()
{

    power_dynamic.breakdown[BUFFER_TO_TILE_FRONT_PWR_D].value += buffer_to_tile_front_pwr_d;
}

// Hub from tile
void Power::bufferFromTilePush()
{
    power_dynamic.breakdown[BUFFER_FROM_TILE_PUSH_PWR_D].value += buffer_from_tile_push_pwr_d;
}

void Power::bufferFromTilePop()
{
    power_dynamic.breakdown[BUFFER_FROM_TILE_POP_PWR_D].value += buffer_from_tile_pop_pwr_d;
}

void Power::bufferFromTileFront()
{

    power_dynamic.breakdown[BUFFER_FROM_TILE_FRONT_PWR_D].value += buffer_from_tile_front_pwr_d;
}


// Antenna buffers (RX/TX)
void Power::antennaBufferPush()
{
    power_dynamic.breakdown[ANTENNA_BUFFER_PUSH_PWR_D].value += antenna_buffer_push_pwr_d;
}

void Power::antennaBufferPop()
{
    power_dynamic.breakdown[ANTENNA_BUFFER_POP_PWR_D].value += antenna_buffer_pop_pwr_d;
}

void Power::antennaBufferFront()
{
    power_dynamic.breakdown[ANTENNA_BUFFER_FRONT_PWR_D].value += antenna_buffer_front_pwr_d;
}


void Power::routing()
{
    power_dynamic.breakdown[ROUTING_PWR_D].value += routing_pwr_d;
}

void Power::selection()
{
    power_dynamic.breakdown[SELECTION_PWR_D].value +=selection_pwr_d ;
}

void Power::crossBar()
{
    power_dynamic.breakdown[CROSSBAR_PWR_D].value +=crossbar_pwr_d;
}

void Power::r2rLink()
{
    power_dynamic.breakdown[LINK_R2R_PWR_D].value +=link_r2r_pwr_d;
}

void Power::r2hLink()
{
    power_dynamic.breakdown[LINK_R2H_PWR_D].value +=link_r2h_pwr_d;
}

void Power::networkInterface()
{
    power_dynamic.breakdown[NI_PWR_D].value +=ni_pwr_d;
}


double Power::getDynamicPower()
{
    double power = 0.0;
    for (int i = 0; i<power_dynamic.size; i++)
    {
	power+= power_dynamic.breakdown[i].value;
    }

    return power;
}

double Power::getStaticPower()
{
    double power = 0.0;
    for (int i = 0; i<power_static.size; i++)
	power+= power_static.breakdown[i].value;

    return power;
}


double Power::attenuation2power(double attenuation)
{
    // TODO
    return attenuation;
}



void Power::wirelessTx(int src,int dst,int length)
{
    power_dynamic.breakdown[WIRELESS_TX].value += default_tx_energy;
    return;

    // TODO enable attenuation_map

    pair<int,int> key = pair<int,int>(src,dst);
    assert(attenuation_map.find(key)!=attenuation_map.end());

    power_dynamic.breakdown[WIRELESS_TX].value += attenuation2power(attenuation_map[key]) * length;
}

void Power::wirelessDynamicRx()
{
    power_dynamic.breakdown[WIRELESS_DYNAMIC_RX_PWR].value += wireless_rx_pwr;
}

void Power::wirelessSnooping()
{
    power_dynamic.breakdown[WIRELESS_SNOOPING].value += wireless_snooping;
}


void Power::biasingRx()
{
    power_static.breakdown[TRANSCEIVER_RX_PWR_BIASING].value += transceiver_rx_pwr_biasing;
}

void Power::biasingTx()
{
    power_static.breakdown[TRANSCEIVER_TX_PWR_BIASING].value += transceiver_tx_pwr_biasing;

}

// Note: In the following 3 functions buffer_pwr_s 
// is assumed as loaded with the proper values from configuration file:
// - Router: takes the value of input buffers leakage
// - Hub: takes the leakage value of buffer_from_tile/to_tile
void Power::leakageBufferRouter()
{
    power_static.breakdown[BUFFER_ROUTER_PWR_S].value +=buffer_router_pwr_s;
}

void Power::leakageBufferToTile()
{
    power_static.breakdown[BUFFER_TO_TILE_PWR_S].value +=buffer_to_tile_pwr_s;
}

void Power::leakageBufferFromTile()
{
    power_static.breakdown[BUFFER_FROM_TILE_PWR_S].value +=buffer_from_tile_pwr_s;
}

// Account for each buffer_rx (Targets) or buffer_tx (Initiators)
void Power::leakageAntennaBuffer()
{
    power_static.breakdown[ANTENNA_BUFFER_PWR_S].value +=(antenna_buffer_pwr_s);
}

void Power::leakageLinkRouter2Router()
{
    //power_static.breakdown[LINK_R2R_PWR_S].value +=link_r2r_pwr_s;
}

void Power::leakageLinkRouter2Hub()
{
    power_static.breakdown[LINK_R2H_PWR_S].value +=link_r2h_pwr_s;
}

void Power::leakageRouter()
{
    // note: leakage contributions depending on instance number are 
    // accounted in specific separate leakage functions
    power_static.breakdown[ROUTING_PWR_S].value +=routing_pwr_s;
    power_static.breakdown[SELECTION_PWR_S].value +=selection_pwr_s;
    power_static.breakdown[CROSSBAR_PWR_S].value +=crossbar_pwr_s;
    power_static.breakdown[NI_PWR_S].value +=ni_pwr_s;
}



void Power::leakageTransceiverRx()
{

    power_static.breakdown[TRANSCEIVER_RX_PWR_S].value +=transceiver_rx_pwr_s;
}

void Power::leakageTransceiverTx()
{

    power_static.breakdown[TRANSCEIVER_TX_PWR_S].value +=transceiver_tx_pwr_s;
}

void Power::printBreakDown(std::ostream & out)
{
    assert(false);
    //printMap("power_dynamic",power_dynamic,cout);
    //printMap("power_static",power_static,cout);
}


void Power::rxSleep(int cycles)
{

    int sleep_start_cycle = (int)(sc_time_stamp().to_double()/GlobalParams::clock_period_ps);
    sleep_end_cycle = sleep_start_cycle + cycles;
}


bool Power::isSleeping()
{
    assert(GlobalParams::use_wirxsleep);
    int now = (int)(sc_time_stamp().to_double()/GlobalParams::clock_period_ps);

    return (now<sleep_end_cycle);

}


void Power::initPowerBreakdownEntry(PowerBreakdownEntry* pbe,string label)
{
    pbe->label = label;
    pbe->value = 0.0;
}



void Power::initPowerBreakdown()
{
    power_dynamic.size = NO_BREAKDOWN_ENTRIES_D;
    power_static.size = NO_BREAKDOWN_ENTRIES_S;

    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_PUSH_PWR_D], "buffer_push_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_POP_PWR_D],"buffer_pop_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_FRONT_PWR_D],"buffer_front_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_TO_TILE_PUSH_PWR_D],"buffer_to_tile_push_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_TO_TILE_POP_PWR_D],"buffer_to_tile_pop_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_TO_TILE_FRONT_PWR_D],"buffer_to_tile_front_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_FROM_TILE_PUSH_PWR_D],"buffer_from_tile_push_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_FROM_TILE_POP_PWR_D],"buffer_from_tile_pop_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_FROM_TILE_FRONT_PWR_D],"buffer_from_tile_front_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[ANTENNA_BUFFER_PUSH_PWR_D],"antenna_buffer_push_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[ANTENNA_BUFFER_POP_PWR_D],"antenna_buffer_pop_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[ANTENNA_BUFFER_FRONT_PWR_D],"antenna_buffer_front_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[ROUTING_PWR_D],"routing_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[SELECTION_PWR_D],"selection_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[CROSSBAR_PWR_D],"crossbar_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[LINK_R2R_PWR_D],"link_r2r_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[LINK_R2H_PWR_D],"link_r2h_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[NI_PWR_D],"ni_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[WIRELESS_TX],"wireless_tx");
    initPowerBreakdownEntry(&power_dynamic.breakdown[WIRELESS_DYNAMIC_RX_PWR],"wireless_dynamic_rx_pwr");
    initPowerBreakdownEntry(&power_dynamic.breakdown[WIRELESS_SNOOPING],"wireless_snooping");

    initPowerBreakdownEntry(&power_static.breakdown[TRANSCEIVER_RX_PWR_BIASING],"transceiver_rx_pwr_biasing");
    initPowerBreakdownEntry(&power_static.breakdown[TRANSCEIVER_TX_PWR_BIASING],"transceiver_tx_pwr_biasing");
    initPowerBreakdownEntry(&power_static.breakdown[BUFFER_ROUTER_PWR_S],"buffer_router_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[BUFFER_TO_TILE_PWR_S],"buffer_to_tile_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[BUFFER_FROM_TILE_PWR_S],"buffer_from_tile_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[ANTENNA_BUFFER_PWR_S],"antenna_buffer_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[LINK_R2H_PWR_S],"link_r2h_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[ROUTING_PWR_S],"routing_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[SELECTION_PWR_S],"selection_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[CROSSBAR_PWR_S],"crossbar_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[NI_PWR_S],"ni_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[TRANSCEIVER_RX_PWR_S],"transceiver_rx_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[TRANSCEIVER_TX_PWR_S],"transceiver_tx_pwr_s");
}

    



