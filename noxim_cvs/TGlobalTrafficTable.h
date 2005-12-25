/*****************************************************************************

  TGlobalTrafficTable.h -- Global Traffic Table definition

*****************************************************************************/

#ifndef __TGLOBAL_TRAFFIC_TABLE_H__
#define __TGLOBAL_TRAFFIC_TABLE_H__

//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "NoximDefs.h"
using namespace std;

//---------------------------------------------------------------------------

class TLocalTrafficParams
{
  TCoord src;
  TCoord dst;
  int traffic_distribution;
}

//---------------------------------------------------------------------------

class TGlobalTrafficTable
{

public:

  // The constructor by default sets valid to false
  TGlobalTrafficTable(); 

  // Load traffic table from file. Returns true if OK (and sets valid to true), false otherwise
  bool load(const char* fname);

  // Tell if the couple Source-Destination has traffic or not
  bool hasTraffic(const int src, const int dst);

  // Tell if the current Traffic Table is valid
  bool isValid() { return valid; }


private:

  vector<TLocalTrafficParams> traffic_table;
  bool valid;

};

//---------------------------------------------------------------------------

#endif
