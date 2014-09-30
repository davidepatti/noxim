/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the tile
 */

#ifndef __TOKENRING_H__
#define __TOKENRING_H__

#include <map>
#include <set>

using namespace std;

class TokenRing
{

private:

  map<int,int> ring;  // radio_hub_id --> hold_cycles
  int          token; // radio_hub_id
  int          hold_count;
  

public:

  TokenRing() { }

  void ShowConfiguration(char *prefix);

  void AddElement(int radio_hub_id, int hold_cycles);

  bool HasToken(int radio_hub_id);

  void Update();

  void Update(set<int>& ready_rh);


private:

  void MoveTokenToNextReadyRH(set<int>& ready_rh);
};

#endif
