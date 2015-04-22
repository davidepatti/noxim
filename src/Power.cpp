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

    pair<int,int> key = pair<int,int>(buffer_depth,buffer_size);

    assert(buffer_push_pm.find(key)!=buffer_push_pm.end());
    assert(buffer_pop_pm.find(key)!=buffer_pop_pm.end());
    assert(buffer_front_pm.find(key)!=buffer_front_pm.end());

    buffer_push_pwr = buffer_push_pm[key];
    buffer_pop_pwr = buffer_pop_pm[key];
    buffer_front_pwr = buffer_front_pm[key];

    assert(routing_pm.find(routing_function)!=routing_pm.end());
    routing_pwr = routing_pm[routing_function];

    assert(selection_pm.find(selection_function)!=selection_pm.end());
    selection_pwr = selection_pm[selection_function];

    link_pwr = link_width * bit_line_pwr;


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
