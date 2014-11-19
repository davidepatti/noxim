/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the tile
 */

#ifndef __RADIOHUB_H__
#define __RADIOHUB_H__

#include <map>
#include <set>
#include "Buffer.h"

using namespace std;

class RadioHub
{
private:

  int id;

  map<int,Buffer> from_router_buffers;
  map<int,Buffer> from_rhub_buffers;
  set<int>             tx_channels;
  set<int>             rx_channels;

public:
  
  RadioHub() { }

  RadioHub(int _id) : id(_id) { }

  int GetID() { return id; }

  void ShowConfiguration(char *prefix);

  void AttachRouter(int router_id);

  // Returns true if there is at least a flit waiting into the
  // from_router_buffer that has to be forwarded to another radio hub
  bool ReadyToTransmit();

  // Returns the set of destinations of the flits at the front of
  // from_router_buffers
  set<int> GetDestinations();

  set<int> GetRXChannels();
  set<int> GetTXChannels();

  void AddInternalBuffers(int radio_hub_id);

  void AssignRXChannel(int channel);
  void AssignTXChannel(int channel);
  void AssignRXTXChannel(int channel);

  void SetBuffersSize(int buffers_size);

  bool CanReceive(int router_id);

  void InjectFlit(int router_id, Flit& flit);

  bool FlitAvailable(int router_id);

  Flit GetAndConsumeFlit(int router_id);

  bool CanAcceptFlit(int from_radio_hub_id);

  Flit PopFlit(int to_radio_hub_id,
		    map<int,int>& routers_to_radio_hubs);

  void PushFlit(Flit& flit, int from_radio_hub_id);

  void SetFromRouterBufferSize(int router_id, int buffer_size);
};

#endif
