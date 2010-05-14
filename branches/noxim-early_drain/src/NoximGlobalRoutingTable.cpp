/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the global routing table
 */

#include "NoximGlobalRoutingTable.h"
using namespace std;

NoximLinkId direction2ILinkId(const int node_id, const int dir)
{
    int node_src;

    switch (dir) {
    case DIRECTION_NORTH:
	node_src = node_id - NoximGlobalParams::mesh_dim_x;
	break;

    case DIRECTION_SOUTH:
	node_src = node_id + NoximGlobalParams::mesh_dim_x;
	break;

    case DIRECTION_EAST:
	node_src = node_id + 1;
	break;

    case DIRECTION_WEST:
	node_src = node_id - 1;
	break;

    case DIRECTION_LOCAL:
	node_src = node_id;
	break;

    default:
	assert(false);
    }


    return NoximLinkId(node_src, node_id);
}

int oLinkId2Direction(const NoximLinkId & out_link)
{
    int src = out_link.first;
    int dst = out_link.second;

    if (dst == src)
	return DIRECTION_LOCAL;
    else if (dst == src + 1)
	return DIRECTION_EAST;
    else if (dst == src - 1)
	return DIRECTION_WEST;
    else if (dst == src - NoximGlobalParams::mesh_dim_x)
	return DIRECTION_NORTH;
    else if (dst == src + NoximGlobalParams::mesh_dim_x)
	return DIRECTION_SOUTH;
    else
	assert(false);
    return 0;
}

vector <
    int >admissibleOutputsSet2Vector(const NoximAdmissibleOutputs & ao)
{
    vector < int >dirs;

    for (NoximAdmissibleOutputs::iterator i = ao.begin(); i != ao.end();
	 i++)
	dirs.push_back(oLinkId2Direction(*i));

    return dirs;
}

NoximGlobalRoutingTable::NoximGlobalRoutingTable()
{
    valid = false;
}

bool NoximGlobalRoutingTable::load(const char *fname)
{
    ifstream fin(fname, ios::in);

    if (!fin)
	return false;

    rt_noc.clear();

    bool stop = false;
    while (!fin.eof() && !stop) {
	char line[128];
	fin.getline(line, sizeof(line) - 1);

	if (line[0] == '\0')
	    stop = true;
	else {
	    if (line[0] != '%') {
		int node_id, in_src, in_dst, dst_id, out_src, out_dst;

		if (sscanf
		    (line + 1, "%d %d->%d %d", &node_id, &in_src, &in_dst,
		     &dst_id) == 4) {
		    NoximLinkId lin(in_src, in_dst);

		    char *pstr = line + COLUMN_AOC;
		    while (sscanf(pstr, "%d->%d", &out_src, &out_dst) == 2) {
			NoximLinkId lout(out_src, out_dst);

			rt_noc[node_id][lin][dst_id].insert(lout);

			pstr = strstr(pstr, ",");
			pstr++;
		    }
		}
	    }
	}
    }

    valid = true;

    return true;
}

NoximRoutingTableNode NoximGlobalRoutingTable::
getNodeRoutingTable(const int node_id)
{
    return rt_noc[node_id];
}
