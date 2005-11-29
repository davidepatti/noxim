#ifndef __TSTATS_H__
#define __TSTATS_H__

//---------------------------------------------------------------------------

#include <vector>
#include "NoximDefs.h"

using namespace std;

//---------------------------------------------------------------------------

struct CommHistory
{
  int            src_id;
  vector<double> delays;
};

//---------------------------------------------------------------------------

class TStats
{
public:

  TStats() {}

  void setId(int node_id);

  void receivedFlit(const double arrival_time,
		    const TFlit& flit);

  double getAverageDelay(const int src_id);

  double getAverageDelay();

  unsigned int getReceivedPackets();

  void showStats(std::ostream& out = std::cout);

private:

  int                 id;
  vector<CommHistory> chist;

  int searchCommHistory(int src_id);
};

//---------------------------------------------------------------------------

#endif
