#include "Routing_DEFT.h"

#include "../DeftTopology.h"
#include "../DeftVerticalLinkLut.h"
#include "../GlobalParams.h"
#include "../Utils.h"

RoutingAlgorithmsRegister Routing_DEFT::routingAlgorithmsRegister("DEFT", getInstance());

Routing_DEFT * Routing_DEFT::routing_DEFT = 0;

namespace {

void pushXyDirection(int current_id, int target_id, vector<int> *directions)
{
    Coord current = id2Coord(current_id);
    Coord target = id2Coord(target_id);

    if (target.x > current.x)
        directions->push_back(DIRECTION_EAST);
    else if (target.x < current.x)
        directions->push_back(DIRECTION_WEST);
    else if (target.y > current.y)
        directions->push_back(DIRECTION_SOUTH);
    else if (target.y < current.y)
        directions->push_back(DIRECTION_NORTH);
}

bool sameChiplet(const DeftTopology::RouterInfo &a,
                 const DeftTopology::RouterInfo &b)
{
    return a.layer == DeftTopology::ROUTER_LAYER_CHIPLET &&
           b.layer == DeftTopology::ROUTER_LAYER_CHIPLET &&
           a.chiplet_id == b.chiplet_id;
}

bool canTraverseSelectedSourceExit(
    int current_id,
    const DeftVerticalLinkLut::Entry &entry)
{
    const DeftTopology::VerticalLinkInfo *link =
        DeftTopology::verticalLinkById(entry.source_exit.selected_vl_id);
    return link != 0 &&
           link->is_functional &&
           link->chiplet_endpoint_router_id == current_id &&
           link->interposer_endpoint_router_id ==
               entry.source_exit.interposer_endpoint_router_id;
}

bool canTraverseSelectedDestinationEntry(
    int current_id,
    const DeftVerticalLinkLut::Entry &entry)
{
    const DeftTopology::VerticalLinkInfo *link =
        DeftTopology::verticalLinkById(
            entry.destination_entry.selected_vl_id);
    return link != 0 &&
           link->is_functional &&
           link->interposer_endpoint_router_id == current_id &&
           link->chiplet_endpoint_router_id ==
               entry.destination_entry.boundary_router_id;
}

} // namespace

Routing_DEFT * Routing_DEFT::getInstance() {
    if ( routing_DEFT == 0 )
        routing_DEFT = new Routing_DEFT();

    return routing_DEFT;
}

vector<int> Routing_DEFT::route(Router * router, const RouteData & routeData)
{
    vector<int> directions;

    if (GlobalParams::topology != TOPOLOGY_DEFT_2_5D) {
        pushXyDirection(routeData.current_id, routeData.dst_id, &directions);
        return directions;
    }

    DeftTopology::RouterInfo current =
        DeftTopology::decodeRouterId(routeData.current_id);
    DeftTopology::RouterInfo source =
        DeftTopology::decodeRouterId(routeData.src_id);
    DeftTopology::RouterInfo destination =
        DeftTopology::decodeRouterId(routeData.dst_id);

    if (current.layer == DeftTopology::ROUTER_LAYER_INVALID ||
        source.layer != DeftTopology::ROUTER_LAYER_CHIPLET ||
        destination.layer != DeftTopology::ROUTER_LAYER_CHIPLET)
        return directions;

    if (sameChiplet(source, destination)) {
        pushXyDirection(routeData.current_id, routeData.dst_id, &directions);
        return directions;
    }

    DeftVerticalLinkLut::Entry entry;
    if (!DeftVerticalLinkLut::lookup(source.chiplet_id,
                                     routeData.src_id,
                                     destination.chiplet_id,
                                     &entry))
        return directions;

    if (current.layer == DeftTopology::ROUTER_LAYER_CHIPLET) {
        if (current.chiplet_id == source.chiplet_id) {
            if (routeData.current_id == entry.source_exit.boundary_router_id) {
                if (canTraverseSelectedSourceExit(routeData.current_id, entry))
                    directions.push_back(DIRECTION_HUB);
                return directions;
            }

            pushXyDirection(routeData.current_id,
                            entry.source_exit.boundary_router_id,
                            &directions);
            return directions;
        }

        if (current.chiplet_id == destination.chiplet_id) {
            pushXyDirection(routeData.current_id, routeData.dst_id, &directions);
            return directions;
        }

        return directions;
    }

    if (current.layer == DeftTopology::ROUTER_LAYER_INTERPOSER) {
        if (routeData.current_id ==
            entry.destination_entry.interposer_endpoint_router_id) {
            if (canTraverseSelectedDestinationEntry(routeData.current_id,
                                                    entry))
                directions.push_back(DIRECTION_HUB);
            return directions;
        }

        pushXyDirection(routeData.current_id,
                        entry.destination_entry.interposer_endpoint_router_id,
                        &directions);
    }

    return directions;
}
