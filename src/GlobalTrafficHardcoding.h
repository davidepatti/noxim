/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the definition of the global traffic hardcoding
 */

#ifndef __NOXIMGLOBALTRAFFIC_HARDCODING_H__
#define __NOXIMGLOBALTRAFFIC_HARDCODING_H__

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "DataStructs.h"

using namespace std;

// Structure used to store information into the table
struct HardcodedTrafficEntry {
  int src;			// ID of the source node (PE)
  int dst;			// ID of the destination node (PE)
};

class GlobalTrafficHardcoding {

  public:

    GlobalTrafficHardcoding();

    // Load traffic table from file. Returns true if ok, false otherwise
    bool load(const char *fname);

    vector < HardcodedTrafficEntry > const& traffic_at_cycle(int cycle) const;

    size_t num_cycles() const;

  private:

    // Outer vector: Which cycle the traffic occurs at
    // Inner vector: Attempted packets at given cycle
    vector < vector < HardcodedTrafficEntry > > traffic_list;
};

#endif
