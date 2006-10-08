/*****************************************************************************

  TPower.cpp -- Power model

 *****************************************************************************/

#include <cassert>
#include "NoximDefs.h"
#include "TPower.h"

using namespace std;

// ---------------------------------------------------------------------------

TPower::TPower()
{
  pwr = 0.0;

  pwr_standby  = PWR_STANDBY;
  pwr_forward  = PWR_FORWARD_FLIT;
  pwr_incoming = PWR_INCOMING;

  if (TGlobalParams::routing_algorithm == ROUTING_XY) pwr_routing = PWR_ROUTING_XY;
  else if (TGlobalParams::routing_algorithm == ROUTING_XY) pwr_routing = PWR_ROUTING_XY;
  else if (TGlobalParams::routing_algorithm == ROUTING_WEST_FIRST) pwr_routing = PWR_ROUTING_WEST_FIRST;
  else if (TGlobalParams::routing_algorithm == ROUTING_NORTH_LAST) pwr_routing = PWR_ROUTING_NORTH_LAST;
  else if (TGlobalParams::routing_algorithm == ROUTING_NEGATIVE_FIRST) pwr_routing = PWR_ROUTING_NEGATIVE_FIRST;
  else if (TGlobalParams::routing_algorithm == ROUTING_ODD_EVEN) pwr_routing = PWR_ROUTING_ODD_EVEN;
  else if (TGlobalParams::routing_algorithm == ROUTING_DYAD) pwr_routing = PWR_ROUTING_DYAD;
  else if (TGlobalParams::routing_algorithm == ROUTING_FULLY_ADAPTIVE) pwr_routing = PWR_ROUTING_FULLY_ADAPTIVE;
  else if (TGlobalParams::routing_algorithm == ROUTING_TABLE_BASED) pwr_routing = PWR_ROUTING_TABLE_BASED;
  else assert(false);

  if (TGlobalParams::selection_strategy == SEL_RANDOM) pwr_selection = PWR_SEL_RANDOM;
  else if (TGlobalParams::selection_strategy == SEL_BUFFER_LEVEL) pwr_selection = PWR_SEL_BUFFER_LEVEL;
  else if (TGlobalParams::selection_strategy == SEL_NOP) pwr_selection = PWR_SEL_NOP;
  else assert(false);
}

// ---------------------------------------------------------------------------

void TPower::Routing()
{
  pwr += pwr_routing;
}

// ---------------------------------------------------------------------------

void TPower::Selection()
{
  pwr += pwr_selection;
}

// ---------------------------------------------------------------------------

void TPower::Standby()
{
  pwr += pwr_standby;
}

// ---------------------------------------------------------------------------

void TPower::Forward()
{
  pwr += pwr_forward;
}

// ---------------------------------------------------------------------------

void TPower::Incoming()
{
  pwr += pwr_incoming;
}

// ---------------------------------------------------------------------------



