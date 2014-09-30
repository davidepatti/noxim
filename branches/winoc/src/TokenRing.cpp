/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the buffer
 */

#include <iostream>
#include <cassert>
#include "TokenRing.h"


// ---------------------------------------------------------------------------

void TokenRing::AddElement(int radio_hub_id, int hold_cycles)
{
  map<int,int>::iterator i = ring.find(radio_hub_id);

  if (i != ring.end())
    cerr << "Warning: radio_hub " << radio_hub_id << " already present in tokenring" << endl;

  if (hold_cycles < 1)
  {
    cerr << "hold_cycles must be >= 1" << endl;
    assert(false);
  }

  ring[radio_hub_id] = hold_cycles;
  
  // Initialize the token ring with the last element inserted
  token = radio_hub_id;
  hold_count = hold_cycles;
}

// ---------------------------------------------------------------------------

bool TokenRing::HasToken(int radio_hub_id)
{
  return (token == radio_hub_id);
}

// ---------------------------------------------------------------------------

void TokenRing::Update()
{
  hold_count--;

  if (hold_count == 0)
  {
    map<int,int>::iterator i = ring.find(token);
    i++;
    if (i == ring.end())
      i = ring.begin();
    
    token = i->first;
    hold_count = i->second;
  }
}

// ---------------------------------------------------------------------------

void TokenRing::Update(set<int>& ready_rh)
{
  if (ready_rh.find(token) != ready_rh.end())
  {
    hold_count--;

    if (hold_count == 0)
      MoveTokenToNextReadyRH(ready_rh);
  }
  else
    MoveTokenToNextReadyRH(ready_rh);
}

// ---------------------------------------------------------------------------

void TokenRing::MoveTokenToNextReadyRH(set<int>& ready_rh)
{
  map<int,int>::iterator icurrent = ring.find(token);  
  map<int,int>::iterator i = icurrent;
  
  do {
    i++;
    if (i == ring.end())
      i = ring.begin();

    if (ready_rh.find(i->first) != ready_rh.end())
      break;
  } while (i != icurrent);

  token = i->first;
  hold_count = i->second;
}

// ---------------------------------------------------------------------------


void TokenRing::ShowConfiguration(char *prefix)
{
  cout << prefix << "Ring: [rhub_id (hold_cycles)]: ";
  for (map<int,int>::iterator i = ring.begin(); i != ring.end(); i++)
    cout << i->first << " (" << i->second << "), ";
  cout << endl;
  cout << prefix << "\ttoken: " << token << ", hold count: " << hold_count 
       << endl;
}
