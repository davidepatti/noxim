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
  
  double getAverageDelay();
  
  double getAverageDelay(const int src_id, const int dst_id);

  void showStats(std::ostream& out = std::cout);

private:
  const TNoC* noc;
};

#endif

