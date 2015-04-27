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

double Power::pwr_buffering   = 0.0;
double Power::pwr_routing     = 0.0;
double Power::pwr_selection   = 0.0;
double Power::pwr_crossbar    = 0.0;
double Power::pwr_link        = 0.0;
double Power::pwr_leakage     = 0.0;
double Power::pwr_end2end     = 0.0;
double Power::pwr_rhtxwifi    = 0.0;
double Power::pwr_rhtxelec    = 0.0;

bool   Power::power_data_loaded = false;

map<pair<int,int>, double> Power::rh_power_map;

Power::Power()
{
  pwr = 0.0;


  if (!power_data_loaded)
    {
      assert(LoadPowerData(GlobalParams::router_power_filename));
      power_data_loaded = true;
    }
}

void Power::configureRouter(int link_width,
	int buffer_depth,
	int buffer_size,
	string routing_function,
	string selection_function)
{

// (s)tatic, (d)ynamic power

// Buffer //////////////////////////////////
    pair<int,int> key = pair<int,int>(buffer_depth,buffer_size);

    assert(buffer_push_pm.find(key)!=buffer_push_pm.end());
    assert(buffer_pop_pm.find(key)!=buffer_pop_pm.end());
    assert(buffer_front_pm.find(key)!=buffer_front_pm.end());
    assert(buffer_leakage_pm.find(key)!=buffer_leakage_pm.end());

    buffer_push_pwr_d = buffer_push_pm[key];
    buffer_pop_pwr_d = buffer_pop_pm[key];
    buffer_front_pwr_d = buffer_front_pm[key];
    buffer_pwr_s = buffer_leakage_pm[key];


// Routing //////////////////////////////////

    assert(routing_pm_s.find(routing_function)!=routing_pm_s.end());
    assert(routing_pm_d.find(routing_function)!=routing_pm_d.end());

    routing_pwr_d = routing_pm_d[routing_function];
    routing_pwr_s = routing_pm_s[routing_function];

// Selection //////////////////////////////////
    assert(selection_pm_s.find(selection_function)!=selection_pm_s.end());
    assert(selection_pm_d.find(selection_function)!=selection_pm_d.end());

    selection_pwr_d = selection_pm_d[selection_function];
    selection_pwr_s = selection_pm_s[selection_function];

// Link 
    link_pwr_s = link_width * bit_line_pwr_s;
    link_pwr_d = link_width * bit_line_pwr_d;


}

void Power::configureHub(int link_width,
	int buffer_depth,
	int buffer_size,
	int antenna_buffer_depth,
	int antenna_buffer_size)
{
// (s)tatic, (d)ynamic power

// Buffer //////////////////////////////////
    pair<int,int> key = pair<int,int>(buffer_depth,buffer_size);

    assert(buffer_push_pm.find(key)!=buffer_push_pm.end());
    assert(buffer_pop_pm.find(key)!=buffer_pop_pm.end());
    assert(buffer_front_pm.find(key)!=buffer_front_pm.end());
    assert(buffer_leakage_pm.find(key)!=buffer_leakage_pm.end());

    buffer_push_pwr_d = buffer_push_pm[key];
    buffer_pop_pwr_d = buffer_pop_pm[key];
    buffer_front_pwr_d = buffer_front_pm[key];
    buffer_pwr_s = buffer_leakage_pm[key];
// Buffer Antenna//////////////////////////////////
    pair<int,int> akey = pair<int,int>(antenna_buffer_depth,antenna_buffer_size);

    assert(antenna_buffer_push_pm.find(akey)!=buffer_push_pm.end());
    assert(antenna_buffer_pop_pm.find(akey)!=buffer_pop_pm.end());
    assert(antenna_buffer_front_pm.find(akey)!=buffer_front_pm.end());
    assert(antenna_buffer_leakage_pm.find(akey)!=buffer_leakage_pm.end());

    antenna_buffer_push_pwr_d = buffer_push_pm[akey];
    antenna_buffer_pop_pwr_d = buffer_pop_pm[akey];
    antenna_buffer_front_pwr_d = buffer_front_pm[akey];
    antenna_buffer_pwr_s = buffer_leakage_pm[akey];

    // mappa (id_src,id_dst) -> bit pwr tx
    bit_wireless_tx_pwr = bit_wireless_tx_pm;

    // non e' una mappa, e' indipendente da sorgente e destinazione
    flit_wireless_rx_pwr = antenna_buffer_size * bit_wireless_rx;

    transceiver_pwr_s = transceiver_pwd_s_TURI_SCEGLI_NOME;

}


void Power::Routing()
{
  pwr += pwr_routing;
}

void Power::Selection()
{
  pwr += pwr_selection;
}

void Power::Buffering()
{
  pwr += pwr_buffering;
}

void Power::Link()
{
  pwr += pwr_link;
}


void Power::Crossbar()
{
  pwr += pwr_crossbar;
}

void Power::Leakage()
{
  pwr += pwr_leakage;
}

void Power::EndToEnd()
{
  pwr += pwr_end2end;
}

void Power::RHTransmitWiFi(int rh_src_id, int rh_dst_id)
{
  pair<int,int> key(rh_src_id, rh_dst_id);
  map<pair<int,int>, double>::iterator i = rh_power_map.find(key);

  if (i == rh_power_map.end())
    pwr_rh += pwr_rhtxwifi;
  else
    pwr_rh += i->second;
}

void Power::RHTransmitElec()
{
  pwr_rh += pwr_rhtxelec;
}

bool Power::LoadPowerData(const char *fname)
{
  YAML::Node config = YAML::LoadFile(fname);
  YAML::Node router = config["Router"];
  YAML::Node radio_hub = config["RadioHub"];

  pwr_routing = router["Routing"].as<double>();
  pwr_buffering = router["Buffering"].as<double>();
  pwr_selection = router["Selection"].as<double>();
  pwr_crossbar = router["Crossbar"].as<double>();
  pwr_link = router["Link"].as<double>();
  pwr_leakage = router["Leakage"].as<double>();
  pwr_end2end = router["End2end"].as<double>();
  pwr_rhtxwifi = radio_hub["DefaultPower"].as<double>();
  pwr_rhtxelec = radio_hub["WiresPowerContribution"].as<double>();

  for (size_t i = 0; i < radio_hub["PowerMap"].size(); i++)
  {
      pair<int,int> key(radio_hub["PowerMap"][i]["TXHubID"].as<int>(), radio_hub["PowerMap"][i]["RXHubID"].as<int>());
      rh_power_map[key] =radio_hub["PowerMap"][i]["Energy"].as<double>();
  }

  return true;
}
