/*****************************************************************************

  TGlobalStats.h -- Global Statistics definition

 *****************************************************************************/

#ifndef __TGLOBAL_STATS_H__
#define __TGLOBAL_STATS_H__

//---------------------------------------------------------------------------

#include <iostream>
#include "TNoC.h"
#include "TTile.h"

//---------------------------------------------------------------------------

class TGlobalStats
{
public:
  TGlobalStats(const TNoC* _noc);
  
  // Returns the aggragated average delay (cycles)
  double getAverageDelay();
  
  // Returns the aggragated average delay (cycles) for communication
  // src_id->dst_id
  double getAverageDelay(const int src_id, const int dst_id);

  // Returns the aggragated average throughput (flits/cycles)
  double getAverageThroughput();

  // Returns the aggragated average throughput (flits/cycles) for
  // communication src_id->dst_id
  double getAverageThroughput(const int src_id, const int dst_id);

  // Returns the total number of received packets
  unsigned int getReceivedPackets();

  // Returns the total number of received flits
  unsigned int getReceivedFlits();

  // Shows global statistics
  void showStats(std::ostream& out = std::cout);

private:
  const TNoC* noc;
};

#endif

