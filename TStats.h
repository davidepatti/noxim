/*****************************************************************************

  TStats.h -- Statistics definition

 *****************************************************************************/
/* Copyright 2005-2007  
    Fabrizio Fazzino <fabrizio.fazzino@diit.unict.it>
    Maurizio Palesi <mpalesi@diit.unict.it>
    Davide Patti <dpatti@diit.unict.it>

 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef __TSTATS_H__
#define __TSTATS_H__

//---------------------------------------------------------------------------

#include <vector>
#include "NoximDefs.h"
#include "TPower.h"

using namespace std;

//---------------------------------------------------------------------------

struct CommHistory
{
  int            src_id;
  vector<double> delays;
  unsigned int   total_received_flits;
  double         last_received_flit_time;

};

//---------------------------------------------------------------------------

class TStats
{
public:

  TStats() {}

  void configure(const int node_id, const double _warm_up_time);

  // Access point for stats update
  void receivedFlit(const double arrival_time,
		    const TFlit& flit);

  // Returns the average delay (cycles) for the current node as
  // regards to the communication whose source is src_id
  double getAverageDelay(const int src_id);

  // Returns the average delay (cycles) for the current node
  double getAverageDelay();

  // Returns the max delay for the current node as regards the
  // communication whose source node is src_id
  double getMaxDelay(const int src_id);

  // Returns the max delay (cycles) for the current node
  double getMaxDelay();

  // Returns the average throughput (flits/cycle) for the current node
  // and for the communication whose source is src_id
  double getAverageThroughput(const int src_id);

  // Returns the average throughput (flits/cycle) for the current node
  double getAverageThroughput();

  // Returns the number of received packets from current node
  unsigned int getReceivedPackets();

  // Returns the number of received flits from current node
  unsigned int getReceivedFlits();

  // Returns the number of communications whose destination is the
  // current node
  unsigned int getTotalCommunications();

  // Returns the energy consumed for communication src_id-->dst_id
  // under the following assumptions: (i) Minimal routing is
  // considered, (ii) constant packet size is considered (as the
  // average between the minimum and the maximum packet size).
  double getCommunicationEnergy(int src_id, int dst_id);

  // Shows statistics for the current node
  void showStats(int curr_node, std::ostream& out = std::cout, bool header = false);


public:
  
  TPower              power;


private:

  int                 id;
  vector<CommHistory> chist;  
  double              warm_up_time;

  int searchCommHistory(int src_id);
};

//---------------------------------------------------------------------------

#endif
