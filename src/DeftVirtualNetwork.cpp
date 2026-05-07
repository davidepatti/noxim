/*
 * DeFT virtual-network assignment helpers.
 */

#include "DeftVirtualNetwork.h"

#include "DeftTopology.h"
#include "GlobalParams.h"

namespace {

int next_source_vn = DeftVirtualNetwork::VN0;
int next_boundary_vn = DeftVirtualNetwork::VN0;

int peekRoundRobin(int next_vn)
{
    return next_vn;
}

void advanceRoundRobin(int &next_vn)
{
    next_vn =
        (next_vn == DeftVirtualNetwork::VN0) ?
        DeftVirtualNetwork::VN1 :
        DeftVirtualNetwork::VN0;
}

int takeRoundRobin(int &next_vn)
{
    int selected_vn = peekRoundRobin(next_vn);
    advanceRoundRobin(next_vn);
    return selected_vn;
}

bool isSameChiplet(int router_a_id, int router_b_id)
{
    if (!DeftTopology::isChipletRouter(router_a_id) ||
        !DeftTopology::isChipletRouter(router_b_id))
        return false;

    DeftTopology::RouterInfo router_a =
        DeftTopology::decodeRouterId(router_a_id);
    DeftTopology::RouterInfo router_b =
        DeftTopology::decodeRouterId(router_b_id);

    return router_a.chiplet_id == router_b.chiplet_id;
}

bool isInterChipletTraffic(int src_id, int dst_id)
{
    if (!DeftTopology::isChipletRouter(src_id) ||
        !DeftTopology::isChipletRouter(dst_id))
        return false;

    return !isSameChiplet(src_id, dst_id);
}

bool sourceCanUseEitherVirtualNetwork(int src_id, int dst_id)
{
    if (DeftTopology::isInterposerRouter(src_id))
        return true;

    if (DeftTopology::isBoundaryRouter(src_id))
        return true;

    return !isInterChipletTraffic(src_id, dst_id);
}

bool isChipletBoundaryToInterposer(const RouteData &route_data,
                                   int output_direction)
{
    if (output_direction != DIRECTION_HUB)
        return false;

    DeftTopology::RouterInfo current =
        DeftTopology::decodeRouterId(route_data.current_id);

    return current.layer == DeftTopology::ROUTER_LAYER_CHIPLET &&
           current.boundary_router &&
           route_data.dir_in != DIRECTION_LOCAL;
}

bool isInterposerToChiplet(const RouteData &route_data,
                           int output_direction)
{
    if (output_direction != DIRECTION_HUB)
        return false;

    DeftTopology::RouterInfo current =
        DeftTopology::decodeRouterId(route_data.current_id);

    return current.layer == DeftTopology::ROUTER_LAYER_INTERPOSER &&
           DeftTopology::verticalLinkForInterposerRouter(route_data.current_id) != 0;
}

bool isFromInterposerInsideChiplet(const RouteData &route_data)
{
    DeftTopology::RouterInfo current =
        DeftTopology::decodeRouterId(route_data.current_id);

    return current.layer == DeftTopology::ROUTER_LAYER_CHIPLET &&
           current.boundary_router &&
           route_data.dir_in == DIRECTION_HUB;
}

} // namespace

namespace DeftVirtualNetwork {

bool isEnabled()
{
    return GlobalParams::topology == TOPOLOGY_DEFT_2_5D;
}

bool isValidVirtualNetwork(int vc_id)
{
    return vc_id == VN0 || vc_id == VN1;
}

bool canTransition(int from_vn, int to_vn)
{
    if (!isValidVirtualNetwork(from_vn) ||
        !isValidVirtualNetwork(to_vn))
        return false;

    return from_vn == to_vn || (from_vn == VN0 && to_vn == VN1);
}

int assignSourceVirtualNetwork(int src_id, int dst_id)
{
    if (!isEnabled())
        return DEFAULT_VC;

    if (sourceCanUseEitherVirtualNetwork(src_id, dst_id))
        return takeRoundRobin(next_source_vn);

    return VN0;
}

int selectOutputVirtualNetwork(const RouteData &route_data,
                               int output_direction,
                               bool *uses_round_robin_reassignment)
{
    if (uses_round_robin_reassignment != 0)
        *uses_round_robin_reassignment = false;

    const int input_vn = route_data.vc_id;
    if (!isEnabled() || !isValidVirtualNetwork(input_vn))
        return input_vn;

    int output_vn = input_vn;

    if (isChipletBoundaryToInterposer(route_data, output_direction))
    {
        if (input_vn == VN0)
        {
            output_vn = peekRoundRobin(next_boundary_vn);
            if (uses_round_robin_reassignment != 0)
                *uses_round_robin_reassignment = true;
        }
        else
        {
            output_vn = VN1;
        }
    }
    else if (isInterposerToChiplet(route_data, output_direction) ||
             isFromInterposerInsideChiplet(route_data))
    {
        output_vn = VN1;
    }

    if (!canTransition(input_vn, output_vn))
        output_vn = input_vn;

    return output_vn;
}

void commitBoundaryReassignmentRoundRobin()
{
    advanceRoundRobin(next_boundary_vn);
}

} // namespace DeftVirtualNetwork
