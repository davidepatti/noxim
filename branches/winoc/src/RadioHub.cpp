/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the buffer
 */

#include <vector>
#include "RadioHub.h"


// ---------------------------------------------------------------------------

void RadioHub::AttachRouter(int router_id)
{
  if (from_router_buffers.find(router_id) != from_router_buffers.end())
  {
    cerr << "Warning: (radio_hub " << id << ") "
	 << "from_router_buffers[" << router_id << "]"
	 << " already present" << endl;
    return;
  }

  Buffer buffer;
  from_router_buffers[router_id] = buffer;
}

// ---------------------------------------------------------------------------

bool RadioHub::ReadyToTransmit()
{
  for (map<int,Buffer>::iterator i = from_router_buffers.begin();
       i != from_router_buffers.end(); i++)
    if (!i->second.IsEmpty())
      return true;

  return false;
}

// ---------------------------------------------------------------------------

set<int> RadioHub::GetDestinations()
{
  set<int> dst_set;

  for (map<int,Buffer>::iterator i = from_router_buffers.begin();
       i != from_router_buffers.end(); i++)
  {
    Buffer buffer = i->second;

    if (!buffer.IsEmpty())
    {
      Flit flit = buffer.Front();
      dst_set.insert(flit.dst_id);
    }
  }

  return dst_set;
}

// ---------------------------------------------------------------------------

set<int> RadioHub::GetTXChannels()
{
  return tx_channels;
}

set<int> RadioHub::GetRXChannels()
{
  return rx_channels;
}

// ---------------------------------------------------------------------------

void RadioHub::AddInternalBuffers(int radio_hub_id)
{
  if (from_rhub_buffers.find(radio_hub_id) != from_rhub_buffers.end())
  {
    cerr << "Warning: (radio_hub " << id << ") "
	 << "from_rhub_buffers[" << radio_hub_id << "]"
	 << " already present" << endl;
    return;
  }

  Buffer buffer;
  from_rhub_buffers[radio_hub_id] = buffer;
}

// ---------------------------------------------------------------------------

void RadioHub::AssignRXChannel(int channel)
{
  rx_channels.insert(channel);
}

void RadioHub::AssignTXChannel(int channel)
{
  tx_channels.insert(channel);
}

void RadioHub::AssignRXTXChannel(int channel)
{
  AssignRXChannel(channel);
  AssignTXChannel(channel);
}

// ---------------------------------------------------------------------------

void RadioHub::SetBuffersSize(int buffers_size)
{
  for (map<int,Buffer>::iterator i = from_rhub_buffers.begin();
       i != from_rhub_buffers.end(); i++)
    i->second.SetMaxBufferSize(buffers_size);
}

// ---------------------------------------------------------------------------

bool RadioHub::CanReceive(int router_id)
{
  map<int,Buffer>::iterator i = from_router_buffers.find(router_id);

  assert(i != from_router_buffers.end());

  return !(i->second.IsFull());
}

// ---------------------------------------------------------------------------

void RadioHub::InjectFlit(int router_id, Flit& flit)
{
  map<int,Buffer>::iterator i = from_router_buffers.find(router_id);

  assert(i != from_router_buffers.end());
  assert(!i->second.IsFull());

  i->second.Push(flit);
}

// ---------------------------------------------------------------------------

bool RadioHub::FlitAvailable(int router_id)
{
  for (map<int,Buffer>::iterator i = from_rhub_buffers.begin();
       i != from_rhub_buffers.end(); i++)
  {
    if (!i->second.IsEmpty())
      if (i->second.Front().dst_id == router_id)
	return true;
  }

  return false;
}

// ---------------------------------------------------------------------------

Flit RadioHub::GetAndConsumeFlit(int router_id)
{
  vector<int> waiting_head;

  for (map<int,Buffer>::iterator i = from_rhub_buffers.begin();
       i != from_rhub_buffers.end(); i++)
  {
    if (!i->second.IsEmpty())
    {
      Flit flit = i->second.Front();

      if (flit.dst_id == router_id)
      {
	if (flit.flit_type != FLIT_TYPE_HEAD)
	  return i->second.Pop();

	if (flit.flit_type == FLIT_TYPE_HEAD)
	  waiting_head.push_back(i->first);
      }
    }
  }

  // No body flit waiting... select a random waiting head flit
  assert(!waiting_head.empty());
  int buffer_id;
  buffer_id = waiting_head[rand() % waiting_head.size()];

  return from_rhub_buffers[buffer_id].Pop();
}

// ---------------------------------------------------------------------------

bool RadioHub::CanAcceptFlit(int from_radio_hub_id)
{
  map<int,Buffer>::iterator i = from_rhub_buffers.find(from_radio_hub_id);
  assert(i != from_rhub_buffers.end());

  return !(i->second.IsFull());
}

// ---------------------------------------------------------------------------

Flit RadioHub::PopFlit(int to_radio_hub_id,
				 map<int,int>& routers_to_radio_hubs)
{
  vector<int> waiting_head;

  for (map<int,Buffer>::iterator i = from_router_buffers.begin();
       i != from_router_buffers.end(); i++)
  {
    if (!i->second.IsEmpty())
    {
      Flit flit = i->second.Front();

      map<int,int>::iterator j = routers_to_radio_hubs.find(flit.dst_id);
      assert(j != routers_to_radio_hubs.end());

      if (j->second == to_radio_hub_id)
      {
	if (flit.flit_type != FLIT_TYPE_HEAD)
	  return i->second.Pop();
      
	if (flit.flit_type == FLIT_TYPE_HEAD)
	  waiting_head.push_back(i->first);
      }
    }
  }

  // No body flit waiting... select a random waiting head flit
  assert(!waiting_head.empty());
  int buffer_id = waiting_head[rand() % waiting_head.size()];

  return from_router_buffers[buffer_id].Pop();
}

// ---------------------------------------------------------------------------

void RadioHub::PushFlit(Flit& flit,
			     int from_radio_hub_id)
{
  map<int,Buffer>::iterator i = from_rhub_buffers.find(from_radio_hub_id);
  assert(i != from_rhub_buffers.end());

  assert(!i->second.IsFull());
  i->second.Push(flit);
}

// ---------------------------------------------------------------------------

void RadioHub::SetFromRouterBufferSize(int router_id, int buffer_size)
{
  map<int,Buffer>::iterator it = from_router_buffers.find(router_id);
  assert(it != from_router_buffers.end());

  it->second.SetMaxBufferSize(buffer_size);
}

// ---------------------------------------------------------------------------

void RadioHub::ShowConfiguration(char *prefix)
{
  cout << prefix
       << "Radio Hub ID " << id << endl;

  cout << prefix << "TX channels: ";
  for (set<int>::iterator i = tx_channels.begin(); i != tx_channels.end(); i++)
    cout << (*i) << ", ";
  cout << endl;

  cout << prefix << "RX channels: ";
  for (set<int>::iterator i = rx_channels.begin(); i != rx_channels.end(); i++)
    cout << (*i) << ", ";
  cout << endl;


  char prefix_indent[64];
  sprintf(prefix_indent, "%s\t ", prefix);

  cout << prefix << "Buffers from Routers" << endl
       << prefix << "--------------------" << endl;
  for (map<int,Buffer>::iterator i = from_router_buffers.begin();
       i != from_router_buffers.end(); i++)
    cout << prefix_indent 
	 << "router id " << i->first 
	 << ", buffer size " << i->second.GetMaxBufferSize() << endl;

  cout << prefix << "Buffers from Radio Hubs" << endl
       << prefix << "-----------------------" << endl;
  for (map<int,Buffer>::iterator i = from_rhub_buffers.begin();
       i != from_rhub_buffers.end(); i++)
    cout << prefix_indent 
	 << "radio hub id " << i->first 
	 << ", buffer size " << i->second.GetMaxBufferSize() << endl;
}
