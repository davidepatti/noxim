/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 */

#ifndef __NOXIMGLOBALROUTINGTABLE_H__
#define __NOXIMGLOBALROUTINGTABLE_H__

#define COLUMN_AOC 22

#include <vector>
#include <map>
#include <set>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <fstream>
#include "DataStructs.h"
using namespace std;

// Pair of source, destination node
typedef pair < int, int >LinkId;

// Routing table
typedef set < LinkId > AdmissibleOutputs;

// Map a destination to a set of admissible outputs
typedef map < int, AdmissibleOutputs > RoutingTableLink;

// Map an input link to its routing table
typedef map < LinkId, RoutingTableLink > RoutingTableNode;

// Map a node of the network to its routing table
typedef map < int, RoutingTableNode > RoutingTableNoC;

// Converts an input direction to a link 
LinkId direction2ILinkId(const int node_id, const int dir);

// Converts an input direction to a link
int oLinkId2Direction(const LinkId & out_link);

// Converts a set of output links to a set of directions
vector <
    int >admissibleOutputsSet2Vector(const AdmissibleOutputs & ao);

class GlobalRoutingTable {

  public:

    GlobalRoutingTable();

    // Load routing table from file. Returns true if ok, false otherwise
    bool load(const char *fname);

    RoutingTableNode getNodeRoutingTable(const int node_id);

    bool isValid() {
	return valid;
  } private:

     RoutingTableNoC rt_noc;
    bool valid;

};

#endif
