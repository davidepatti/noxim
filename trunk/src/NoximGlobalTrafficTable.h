/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the definition of the global traffic table
 */

#ifndef __NOXIMGLOBALTRAFFIC_TABLE_H__
#define __NOXIMGLOBALTRAFFIC_TABLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "NoximMain.h"
using namespace std;

// Structure used to store information into the table
struct NoximCommunication {
    int src;			// ID of the source node (PE)
    int dst;			// ID of the destination node (PE)
    float pir;			// Packet Injection Rate for the link
    float por;			// Probability Of Retransmission for the link
    int t_on;			// Time (in cycles) at which activity begins
    int t_off;			// Time (in cycles) at which activity ends
    int t_period;		// Period after which activity starts again
};

class NoximGlobalTrafficTable {

  public:

    NoximGlobalTrafficTable();

    // Load traffic table from file. Returns true if ok, false otherwise
    bool load(const char *fname);

    // Returns the cumulative pir por along with a vector of pairs. The
    // first component of the pair is the destination. The second
    // component is the cumulative shotting probability.
    double getCumulativePirPor(const int src_id,
			       const int ccycle,
			       const bool pir_not_por,
			       vector < pair < int, double > > &dst_prob);

    // Returns the number of occurrences of soruce src_id in the traffic
    // table
    int occurrencesAsSource(const int src_id);

  private:

     vector < NoximCommunication > traffic_table;
};

#endif
