/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the tile
 */

#ifndef __WINOC_H__
#define __WINOC_H__

#include <map>
#include "Main.h"
#include "RadioHub.h"
#include "TokenRing.h"
#include "Power.h"

#include "yaml-cpp/yaml.h"

using namespace std;

class WiNoC
{

public:
  
  Power power;


private:

  map<int, int>            routers_to_radio_hubs; // router id --> radio hub id

  map<int, RadioHub>  radio_hubs; // id radio hub --> radio hub

  map<int, TokenRing> token_rings; // radio channel --> token ring


public:

  WiNoC() { }

  // Load configuration file
  void Configure(char* cfg_fname);

  // Show WiNoC configuration
  void ShowConfiguration(char *prefix);

  // Returns true if router_id1 and router_id2 are connected to the
  // same radio hub
  bool SameRadioHub(int router_id1, int router_id2);

  // Single step simulation of the WiNoC (should be called each clock
  // cycle)
  void Run();

  // Returns true if the flit can be injected to the WiNoC
  bool CanTransmit(int router_id);

  // Injects the flits. To be used after CanTransmit() check
  void InjectToRadioHub(int router_id, Flit& flit);

  // Returns true if there is a flit available to be consumed
  bool FlitAvailable(int router_id);

  // Returns the flit available to be consumed. To be used after a
  // FlitAvailable() check
  Flit GetFlit(int router_id);

  // Show the WiNoC configuration (verbose)
  void ShowConfiguration();

  // Configure the buffer size linked to router_id
  void SetFromRouterBufferSize(int router_id, int buffer_size);

  double getPower() const { return power.getPowerRH(); }


private:

  void TransmitCycle();

  map<int,int> AllocateChannels(int radio_hub_src_id,
				set<int>& radio_hub_destinations);

  int GetRandomElement(set<int>& myset);

  void UpdateTokens();

  void TransmitFlit(RadioHub *p_radio_hub_src, 
		    RadioHub *p_radio_hub_dst);

  set<int> RouterID2RadioHubID(set<int>& router_dst);
};

#endif
