/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the power model
 */

#include "NoximPower.h"
using namespace std;

NoximPower::NoximPower()
{
    pwr = 0.0;

    pwr_standby = PWR_STANDBY;
    pwr_forward = PWR_FORWARD_FLIT;
    pwr_incoming = PWR_INCOMING;

    if (NoximGlobalParams::routing_algorithm == ROUTING_XY)
	pwr_routing = PWR_ROUTING_XY;
    else if (NoximGlobalParams::routing_algorithm == ROUTING_XY)
	pwr_routing = PWR_ROUTING_XY;
    else if (NoximGlobalParams::routing_algorithm == ROUTING_WEST_FIRST)
	pwr_routing = PWR_ROUTING_WEST_FIRST;
    else if (NoximGlobalParams::routing_algorithm == ROUTING_NORTH_LAST)
	pwr_routing = PWR_ROUTING_NORTH_LAST;
    else if (NoximGlobalParams::routing_algorithm ==
	     ROUTING_NEGATIVE_FIRST)
	pwr_routing = PWR_ROUTING_NEGATIVE_FIRST;
    else if (NoximGlobalParams::routing_algorithm == ROUTING_ODD_EVEN)
	pwr_routing = PWR_ROUTING_ODD_EVEN;
    else if (NoximGlobalParams::routing_algorithm == ROUTING_DYAD)
	pwr_routing = PWR_ROUTING_DYAD;
    else if (NoximGlobalParams::routing_algorithm ==
	     ROUTING_FULLY_ADAPTIVE)
	pwr_routing = PWR_ROUTING_FULLY_ADAPTIVE;
    else if (NoximGlobalParams::routing_algorithm == ROUTING_TABLE_BASED)
	pwr_routing = PWR_ROUTING_TABLE_BASED;
    else
	assert(false);

    if (NoximGlobalParams::selection_strategy == SEL_RANDOM)
	pwr_selection = PWR_SEL_RANDOM;
    else if (NoximGlobalParams::selection_strategy == SEL_BUFFER_LEVEL)
	pwr_selection = PWR_SEL_BUFFER_LEVEL;
    else if (NoximGlobalParams::selection_strategy == SEL_NOP)
	pwr_selection = PWR_SEL_NOP;
    else
	assert(false);
}

void NoximPower::Routing()
{
    pwr += pwr_routing;
}

void NoximPower::Selection()
{
    pwr += pwr_selection;
}

void NoximPower::Standby()
{
    pwr += pwr_standby;
}

void NoximPower::Forward()
{
    pwr += pwr_forward;
}

void NoximPower::Incoming()
{
    pwr += pwr_incoming;
}
