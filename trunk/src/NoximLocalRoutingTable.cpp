/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the local routing table
 */

#include "NoximLocalRoutingTable.h"

NoximLocalRoutingTable::NoximLocalRoutingTable()
{
}

void NoximLocalRoutingTable::configure(NoximGlobalRoutingTable & rtable,
				       const int _node_id)
{
    rt_node = rtable.getNodeRoutingTable(_node_id);
    node_id = _node_id;
}

NoximAdmissibleOutputs NoximLocalRoutingTable::
getAdmissibleOutputs(const NoximLinkId & in_link, const int destination_id)
{
    return rt_node[in_link][destination_id];
}

NoximAdmissibleOutputs NoximLocalRoutingTable::
getAdmissibleOutputs(const int in_direction, const int destination_id)
{
    NoximLinkId lid = direction2ILinkId(node_id, in_direction);

    return getAdmissibleOutputs(lid, destination_id);
}
