/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the global traffic hardcoding
 */

#include "GlobalTrafficHardcoding.h"
#include <fstream>
#include <iostream>

using namespace std;

GlobalTrafficHardcoding::GlobalTrafficHardcoding()
{
}

bool GlobalTrafficHardcoding::load(const char *fname)
{
  // Open file
  ifstream fin(fname, ios::in);
  if (!fin)
    return false;

  // Initialize variables
  traffic_list.clear();
  
  vector<HardcodedTrafficEntry> current_cycle_traffic;
  
  // Cycle reading file
  while (!fin.eof()) {
    char line[512];
    fin.getline(line, sizeof(line) - 1);

    if (line[0] != '\0') {
      // Skip comment lines
      if (line[0] != '%' && line[0] != '#') {
        int src, dst;
        
        int params = sscanf(line, "%d %d", &src, &dst);
        
        if (params == 2) {
          // Check for end-of-cycle marker
          if (src == -1) {
            // End of current cycle - add the accumulated traffic to the list
            traffic_list.push_back(current_cycle_traffic);
            current_cycle_traffic.clear();
          } else {
            // Valid traffic entry - add to current cycle
            HardcodedTrafficEntry entry;
            entry.src = src;
            entry.dst = dst;
            current_cycle_traffic.push_back(entry);
          }
        } else if (params == 1 && src == -1) {
          // Single -1 on a line (end of cycle or dead cycle)
          traffic_list.push_back(current_cycle_traffic);
          current_cycle_traffic.clear();
        }
      }
    }
  }
  
  // If there's remaining traffic in current_cycle_traffic, add it
  if (!current_cycle_traffic.empty()) {
    traffic_list.push_back(current_cycle_traffic);
  }

  return true;
}

vector < HardcodedTrafficEntry > const& GlobalTrafficHardcoding::traffic_at_cycle(int cycle) const {
  return traffic_list[cycle];
}

size_t GlobalTrafficHardcoding::num_cycles() const {
  return traffic_list.size();
}
