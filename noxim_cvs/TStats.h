/*****************************************************************************

  TStats.h -- Statistics definition

 *****************************************************************************/

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
