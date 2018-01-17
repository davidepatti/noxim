/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the local routing table
 */

#ifndef __NOXIMLOCALROUTINGTABLE_H__
#define __NOXIMLOCALROUTINGTABLE_H__

#include "GlobalRoutingTable.h"
#include "DataStructs.h"

class LocalRoutingTable {

  public:

    // Constructor
    LocalRoutingTable();

    // Extracts the routing table of node _node_id from the global
    // routing table rtable
    void configure(GlobalRoutingTable & rtable, const int _node_id);

    // Returns the set of admissible output channels for destination
    // destination_id and input channel in_link
    AdmissibleOutputs getAdmissibleOutputs(const LinkId &
						in_link,
						const int destination_id);

    // Returns the set of admissible output channels for a destination
    // destination_id and a given input direction
    AdmissibleOutputs getAdmissibleOutputs(const int in_direction,
						const int destination_id);

  private:

     RoutingTableNode rt_node;
    int node_id;
};

#endif
