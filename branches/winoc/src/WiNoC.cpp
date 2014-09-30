/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the buffer
 */

#include <cstdio>
#include <algorithm>
#include "WiNoC.h"

// ---------------------------------------------------------------------------

#define RADIOHUB_SECTION_START       "[radiohub]"
#define RADIOHUB_SECTION_STOP        "[/radiohub]"

#define RADIO_CHANNELS_SECTION_START "[channels]"
#define RADIO_CHANNELS_SECTION_STOP  "[/channels]"

#define BINDING_SECTION_START        "[binding]"
#define BINDING_SECTION_STOP         "[/binding]"

#define BUFFERS_SECTION_START        "[buffers]"
#define BUFFERS_SECTION_STOP         "[/buffers]"

#define TOKEN_RING_SECTION_START     "[tokenring]"
#define TOKEN_RING_SECTION_STOP      "[/tokenring]"

// ---------------------------------------------------------------------------

bool WiNoC::SameRadioHub(int router_id1, int router_id2)
{
  map<int,int>::iterator it1 = routers_to_radio_hubs.find(router_id1);
  map<int,int>::iterator it2 = routers_to_radio_hubs.find(router_id2);
  if ( it1 == routers_to_radio_hubs.end() ||
       it2 == routers_to_radio_hubs.end() )
    return false;

  return (it1->second == it2->second);
}

// ---------------------------------------------------------------------------

void WiNoC::SetFromRouterBufferSize(int router_id, int buffer_size)
{
  map<int,int>::iterator it_r = routers_to_radio_hubs.find(router_id);
  assert(it_r != routers_to_radio_hubs.end());

  map<int,RadioHub>::iterator it_rh = radio_hubs.find(it_r->second);
  assert(it_rh != radio_hubs.end());

  it_rh->second.SetFromRouterBufferSize(router_id, buffer_size);
}

// ---------------------------------------------------------------------------

void WiNoC::Configure(char* cfg_fname)
{
  FILE *f = fopen(cfg_fname, "r");

  if (f == NULL)
  {
    cerr << "Cannot open winoc configuration file '" << cfg_fname << "'" << endl;
    assert(false);
  }

  // DO NOT CHANGE THE FOLLOWING ORDER!!!

  // Determine how many radio_hubs form the WiNoC
  CfgGetRadioHubsInfo(f);

  // Set RX/TX channels
  CfgGetRadioChannelsInfo(f);

  // Set internal buffers size of the radio_hubs
  CfgGetBuffersInfo(f);

  // Connect routers to radio_hub
  CfgGetBindingInfo(f);

  // Configure token rings
  CfgGetTokenRingInfo(f);

  fclose(f);
}

// ---------------------------------------------------------------------------

bool WiNoC::IsComment(char* line)
{
  return (line[0] == '%');
}

bool WiNoC::IsEmptyLine(char *line)
{
  if (strlen(line) == 0)
    return true;

  int i=0;
  while (line[i] != '\0')
  {
    if (line[i] != ' ' && line[i] != '\n')
      return false;
    i++;
  }

  return true;
}

bool WiNoC::SeekToSection(FILE* f, char* section_name)
{
  fseek(f, 0, SEEK_SET);

  bool section_found = false;

  while (!feof(f) && !section_found)
  {
    char line[256];

    if (fgets(line, sizeof(line), f))
    {
      if (strncmp(line, section_name, strlen(section_name)) == 0)
	section_found = true;
    }
  }

  return section_found;
}

// ---------------------------------------------------------------------------

void WiNoC::CfgGetRadioHubsInfo(FILE* f)
{
  assert(SeekToSection(f, (char*)RADIOHUB_SECTION_START));

  bool end_of_section = false;
  while (!end_of_section && !feof(f))
  {
    char line[256];

    if (fgets(line, sizeof(line), f))
    {
      if (!IsComment(line) && !IsEmptyLine(line))
      {
	if (strncmp(line, RADIOHUB_SECTION_STOP, strlen(RADIOHUB_SECTION_STOP)) == 0)
	  end_of_section = true;
	else
	{
	  int radio_hub_id, buffers_size;

	  if (sscanf(line, "%d", &radio_hub_id) != 1)
	  {
	    cerr << "Error reading WiNoC configuration file (section radiohub)" << endl;
	    assert(false);
	  }
	  AddRadioHub(radio_hub_id);
	}
      }
    }
  }
}

// ---------------------------------------------------------------------------

void WiNoC::CfgGetBindingInfo(FILE* f)
{
  assert(SeekToSection(f, (char *)BINDING_SECTION_START));

  bool end_of_section = false;
  while (!end_of_section && !feof(f))
  {
    char line[256];

    if (fgets(line, sizeof(line), f))
    {
      if (!IsComment(line) && !IsEmptyLine(line))
      {
	if (strncmp(line, BINDING_SECTION_STOP, strlen(BINDING_SECTION_STOP)) == 0)
	  end_of_section = true;
	else
	{
	  int router_id, radio_hub_id;

	  if (sscanf(line, "%d %d", &router_id, &radio_hub_id) != 2)
	  {
	    cerr << "Error reading WiNoC configuration file (section binding)" << endl;
	    assert(false);
	  }
	  AttachRouterToRadioHub(router_id, radio_hub_id);
	}
      }
    }
  }
}

// ---------------------------------------------------------------------------

void WiNoC::CfgGetRadioChannelsInfo(FILE* f)
{
  assert(SeekToSection(f, (char *)RADIO_CHANNELS_SECTION_START));

  bool end_of_section = false;
  while (!end_of_section && !feof(f))
  {
    char line[256];

    if (fgets(line, sizeof(line), f))
    {
      if (!IsComment(line) && !IsEmptyLine(line))
      {
	if (strncmp(line, RADIO_CHANNELS_SECTION_STOP, strlen(RADIO_CHANNELS_SECTION_STOP)) == 0)
	  end_of_section = true;
	else
	{
	  int  radio_hub_id, channel;
	  char mode[16];

	  if (sscanf(line, "%d %d %s", &radio_hub_id, &channel, mode) != 3)
	  {
	    cerr << "Error reading WiNoC configuration file (section radio_channels)" << endl;
	    assert(false);
	  }
	  AssignChannelToRadioHub(radio_hub_id, channel, mode);
	}
      }
    }
  }
}

// ---------------------------------------------------------------------------

void WiNoC::CfgGetBuffersInfo(FILE* f)
{
  assert(SeekToSection(f, (char *)BUFFERS_SECTION_START));

  bool end_of_section = false;
  while (!end_of_section && !feof(f))
  {
    char line[256];

    if (fgets(line, sizeof(line), f))
    {
      if (!IsComment(line) && !IsEmptyLine(line))
      {
	if (strncmp(line, BUFFERS_SECTION_STOP, strlen(BUFFERS_SECTION_STOP)) == 0)
	  end_of_section = true;
	else
	{
	  int radio_hub_id, buffers_size;

	  if (sscanf(line, "%d %d", &radio_hub_id, &buffers_size) != 2)
	  {
	    cerr << "Error reading WiNoC configuration file (section buffers)" << endl;
	    assert(false);
	  }
	  SetRadioHubBuffersSize(radio_hub_id, buffers_size);
	}
      }
    }
  }
}

// ---------------------------------------------------------------------------

void WiNoC::CfgGetTokenRingInfo(FILE* f)
{
  assert(SeekToSection(f, (char *)TOKEN_RING_SECTION_START));

  bool end_of_section = false;
  while (!end_of_section && !feof(f))
  {
    char line[256];

    if (fgets(line, sizeof(line), f))
    {
      if (!IsComment(line) && !IsEmptyLine(line))
      {
	if (strncmp(line, TOKEN_RING_SECTION_STOP, strlen(TOKEN_RING_SECTION_STOP)) == 0)
	  end_of_section = true;
	else
	{
	  int radio_hub_id, token_cycles;

	  if (sscanf(line, "%d %d", &radio_hub_id, &token_cycles) != 2)
	  {
	    cerr << "Error reading WiNoC configuration file (section tokenring)" << endl;
	    assert(false);
	  }
	  SetTokenRing(radio_hub_id, token_cycles);
	}
      }
    }
  }
}

// ---------------------------------------------------------------------------

void WiNoC::Run()
{
  TransmitCycle();
  UpdateTokens();
}

// ---------------------------------------------------------------------------

void WiNoC::TransmitCycle()
{
  for (map<int,RadioHub>::iterator i = radio_hubs.begin();
       i != radio_hubs.end(); i++)
  {
    int radio_hub_src_id = i->first;
    RadioHub *p_radio_hub_src = &(i->second);

    if (p_radio_hub_src->ReadyToTransmit())
    {
      set<int> router_destinations = p_radio_hub_src->GetDestinations();
      set<int> radio_hub_destinations = RouterID2RadioHubID(router_destinations);

      // destination radio_hub_id  --> channel to be used
      map<int,int> tx_channels = AllocateChannels(radio_hub_src_id,
						  radio_hub_destinations);

      for (set<int>::iterator j = radio_hub_destinations.begin();
	   j != radio_hub_destinations.end(); j++)
      {
	int radio_hub_dst_id = *j;

	map<int,int>::iterator tx_channels_i = tx_channels.find(radio_hub_dst_id);
	if (tx_channels_i == tx_channels.end())
	  continue;

	int radio_channel = tx_channels_i->second;

	map<int,TokenRing>::iterator token_rings_i = token_rings.find(radio_channel);
	assert(token_rings_i != token_rings.end());

	if (token_rings_i->second.HasToken(radio_hub_src_id))
	{
	  map<int,RadioHub>::iterator radio_hubs_i = radio_hubs.find(radio_hub_dst_id);
	  assert(radio_hubs_i != radio_hubs.end());

	  RadioHub *p_radio_hub_dst = &(radio_hubs_i->second);
	  TransmitFlit(p_radio_hub_src, p_radio_hub_dst);
	}
      }
    }
  }
}

// ---------------------------------------------------------------------------

set<int> WiNoC::RouterID2RadioHubID(set<int>& router_dst)
{
  set<int> rh_dst;

  for (set<int>::iterator i = router_dst.begin();
       i != router_dst.end(); i++)
  {
    map<int,int>::iterator j = routers_to_radio_hubs.find(*i);

    if (j == routers_to_radio_hubs.end())
    {
      cerr << "Router id " << (*i) << " is not connected to any radio_hub" << endl;
      assert(false);
    }

    rh_dst.insert(j->second);
  }

  return rh_dst;
}

// ---------------------------------------------------------------------------

int WiNoC::GetRandomElement(set<int>& myset)
{
  int size = myset.size();
  int n = rand() % size;
  set<int>::iterator it(myset.begin());
  advance(it, n);

  return *it;
}

// ---------------------------------------------------------------------------

// Returns a map: dst_radio_hub_id  --> channel_to_be_used
map<int,int> WiNoC::AllocateChannels(int radio_hub_src_id,
					  set<int>& radio_hub_destinations)
{
  map<int,int> tx_channels_map;

  set<int> tx_channels = radio_hubs[radio_hub_src_id].GetTXChannels();

  for (set<int>::iterator i=radio_hub_destinations.begin();
       i != radio_hub_destinations.end(); i++)
  {
    int radio_hub_dst_id = *i;

    set<int> rx_channels = radio_hubs[radio_hub_dst_id].GetRXChannels();

    set<int> ch_intersection;
    set_intersection(tx_channels.begin(), tx_channels.end(), 
		     rx_channels.begin(), rx_channels.end(), 
		     inserter(ch_intersection, ch_intersection.end()));

    if (!ch_intersection.empty())
    {
      int channel_to_be_used = GetRandomElement(ch_intersection);
      tx_channels_map[radio_hub_dst_id]  = channel_to_be_used;
      tx_channels.erase(channel_to_be_used);
    }
  }

  return tx_channels_map;
}

// ---------------------------------------------------------------------------

void WiNoC::UpdateTokens()
{
  // Token should be assigned to the first radio hub having something
  // to transmit.

  // Create a map: channel --> set(ready radio_hub_id)
  map<int,set<int> > ready_rhubs;
  for (map<int, RadioHub>::iterator i = radio_hubs.begin();
       i != radio_hubs.end(); i++)
  {
    if (i->second.ReadyToTransmit())
    {
      set<int> tx_channels = i->second.GetTXChannels();
      for (set<int>::iterator j = tx_channels.begin();
	   j != tx_channels.end(); j++)
	ready_rhubs[*j].insert(i->second.GetID());
    }
  }

  // Update tokens
  for (map<int,TokenRing>::iterator i = token_rings.begin();
       i != token_rings.end(); i++)
  {
    int ch = i->first;
    if (ready_rhubs.find(ch) != ready_rhubs.end())
      i->second.Update(ready_rhubs[ch]);
    else
      i->second.Update();
  }
}

// ---------------------------------------------------------------------------

void WiNoC::AddRadioHub(int radio_hub_id)
{
  map<int, RadioHub>::iterator i = radio_hubs.find(radio_hub_id);

  if (i != radio_hubs.end())
  {
    cerr << "Warning: radio_hub " << radio_hub_id << " already present" << endl;
    return;
  }

  RadioHub rh(radio_hub_id);
  radio_hubs[radio_hub_id] = rh;

  UpdateRadioHubsInternalBuffers(radio_hub_id);
}

// ---------------------------------------------------------------------------

void WiNoC::UpdateRadioHubsInternalBuffers(int radio_hub_id)
{
  // Add internal buffer for radio_hubs != radio_hub_id
  for (map<int, RadioHub>::iterator i = radio_hubs.begin();
       i != radio_hubs.end(); i++)
  {
    if (radio_hub_id != i->first)
    {
      i->second.AddInternalBuffers(radio_hub_id);
    }
  }

  // Add internal buffers to radio_hub_id
  for (map<int, RadioHub>::iterator i = radio_hubs.begin();
       i != radio_hubs.end(); i++)
  {
    if (radio_hub_id != i->first)
    {
      radio_hubs[radio_hub_id].AddInternalBuffers(i->first);
    }
  }
}

// ---------------------------------------------------------------------------

void WiNoC::AttachRouterToRadioHub(int router_id, int radio_hub_id)
{
  map<int, RadioHub>::iterator radio_hubs_i = radio_hubs.find(radio_hub_id);
  if (radio_hubs_i == radio_hubs.end())
  {
    cerr << "Undefined radio_hub_id " << radio_hub_id << endl;
    assert(false);
  }
  
  routers_to_radio_hubs[router_id] = radio_hub_id;

  radio_hubs_i->second.AttachRouter(router_id);
}

// ---------------------------------------------------------------------------

void WiNoC::AssignChannelToRadioHub(int radio_hub_id, int channel, 
					 char* mode)
{
  if (radio_hubs.find(radio_hub_id) == radio_hubs.end())
  {
    cerr << "Undefined radio_hub_id " << radio_hub_id << endl;
    assert(false);
  }

  if (strcmp(mode, "rx") == 0)
    radio_hubs[radio_hub_id].AssignRXChannel(channel);
  else if (strcmp(mode, "tx") == 0)
    radio_hubs[radio_hub_id].AssignTXChannel(channel);
  else if (strcmp(mode, "rxtx") == 0)
    radio_hubs[radio_hub_id].AssignRXTXChannel(channel);
  else
  {
    cerr << "Invalid channel type '" << mode << "'" << endl;
    assert(false);
  }
}

// ---------------------------------------------------------------------------

void WiNoC::SetRadioHubBuffersSize(int radio_hub_id, int buffers_size)
{
  if (radio_hubs.find(radio_hub_id) == radio_hubs.end())
  {
    cerr << "Undefined radio_hub_id " << radio_hub_id << endl;
    assert(false);
  }

  radio_hubs[radio_hub_id].SetBuffersSize(buffers_size);
}

// ---------------------------------------------------------------------------

void WiNoC::SetTokenRing(int radio_hub_id, int token_cycles)
{
  if (radio_hubs.find(radio_hub_id) == radio_hubs.end())
  {
    cerr << "Undefined radio_hub_id " << radio_hub_id << endl;
    assert(false);
  }

  set<int> tx_channels = radio_hubs[radio_hub_id].GetTXChannels();
  for (set<int>::iterator i = tx_channels.begin();
       i != tx_channels.end(); i++)
    token_rings[*i].AddElement(radio_hub_id, token_cycles);
}

// ---------------------------------------------------------------------------

bool WiNoC::CanTransmit(int router_id)
{
  map<int, int>::iterator i = routers_to_radio_hubs.find(router_id);
  assert(i != routers_to_radio_hubs.end());

  int radio_hub_id = i->second;

  map<int, RadioHub>::iterator j = radio_hubs.find(radio_hub_id);
  assert(j != radio_hubs.end());

  return j->second.CanReceive(router_id);
}

// ---------------------------------------------------------------------------

void WiNoC::InjectToRadioHub(int router_id, Flit& flit)
{
  map<int, int>::iterator i = routers_to_radio_hubs.find(router_id);
  assert(i != routers_to_radio_hubs.end());

  int radio_hub_id = i->second;

  map<int, RadioHub>::iterator j = radio_hubs.find(radio_hub_id);
  assert(j != radio_hubs.end());

  j->second.InjectFlit(router_id, flit);
}

// ---------------------------------------------------------------------------

bool WiNoC::FlitAvailable(int router_id)
{
  map<int, int>::iterator i = routers_to_radio_hubs.find(router_id);
  assert(i != routers_to_radio_hubs.end());

  int radio_hub_id = i->second;

  map<int, RadioHub>::iterator j = radio_hubs.find(radio_hub_id);
  assert(j != radio_hubs.end());
  
  return j->second.FlitAvailable(router_id);
}

// ---------------------------------------------------------------------------

Flit WiNoC::GetFlit(int router_id)
{
  map<int, int>::iterator i = routers_to_radio_hubs.find(router_id);
  assert(i != routers_to_radio_hubs.end());

  int radio_hub_id = i->second;

  map<int, RadioHub>::iterator j = radio_hubs.find(radio_hub_id);
  assert(j != radio_hubs.end());
  
  power.RHTransmitElec();

  return j->second.GetAndConsumeFlit(router_id);  
}

// ---------------------------------------------------------------------------

void WiNoC::TransmitFlit(RadioHub *p_radio_hub_src, 
			      RadioHub *p_radio_hub_dst) 
{
  if (p_radio_hub_dst->CanAcceptFlit(p_radio_hub_src->GetID()))
  {
    Flit flit = p_radio_hub_src->PopFlit(p_radio_hub_dst->GetID(), 
					      routers_to_radio_hubs);
    p_radio_hub_dst->PushFlit(flit, p_radio_hub_src->GetID());

    power.RHTransmitWiFi(p_radio_hub_src->GetID(), p_radio_hub_dst->GetID());
  }
}

// ---------------------------------------------------------------------------

void WiNoC::ShowConfiguration(char *prefix)
{
  cout << prefix 
       << "Router ID --> Radio Hub ID" << endl
       << prefix
       << "--------------------------" << endl;
  for (map<int, int>::iterator i = routers_to_radio_hubs.begin();
       i != routers_to_radio_hubs.end(); i++)
    cout << prefix
	 << i->first << " --> " << i->second << endl;
  cout << endl;


  char prefix_indent[64];
  sprintf(prefix_indent, "%s\t ", prefix);

  cout << prefix 
       << "Radio Hubs Configuration" << endl
       << prefix
       << "------------------------" << endl;

  for (map<int, RadioHub>::iterator i = radio_hubs.begin();
       i != radio_hubs.end(); i++)
  {
    i->second.ShowConfiguration(prefix_indent);
    cout << endl;
  }


  cout << prefix 
       << "Token Rings Configuration" << endl
       << prefix
       << "-------------------------" << endl;

  for (map<int, TokenRing>::iterator i = token_rings.begin();
       i != token_rings.end(); i++)
  {
    cout << prefix << "Channel " << i->first << endl;
    i->second.ShowConfiguration(prefix_indent);
  }
}
