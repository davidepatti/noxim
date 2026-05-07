/*
 * DeFT virtual-network assignment helpers.
 */

#ifndef __NOXIMDEFTVIRTUALNETWORK_H__
#define __NOXIMDEFTVIRTUALNETWORK_H__

#include "DataStructs.h"

namespace DeftVirtualNetwork {

enum {
    VN0 = 0,
    VN1 = 1,
    VirtualNetworkCount = 2
};

bool isEnabled();
bool isValidVirtualNetwork(int vc_id);
bool canTransition(int from_vn, int to_vn);

int assignSourceVirtualNetwork(int src_id, int dst_id);
int selectOutputVirtualNetwork(const RouteData &route_data,
                               int output_direction,
                               bool *uses_round_robin_reassignment);
void commitBoundaryReassignmentRoundRobin();

} // namespace DeftVirtualNetwork

#endif
