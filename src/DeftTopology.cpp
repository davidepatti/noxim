/*
 * DeFT 2.5D topology mapping helpers.
 */

#include "DeftTopology.h"

#include <cassert>

namespace DeftTopology {

namespace {

const int SLOT_LOCAL_X[VerticalLinksPerChiplet] = {1, 3, 2, 0};
const int SLOT_LOCAL_Y[VerticalLinksPerChiplet] = {0, 1, 3, 2};

std::vector<VerticalLinkInfo> buildVerticalLinks()
{
    std::vector<VerticalLinkInfo> links;
    links.reserve(VerticalLinkCount);

    for (int chiplet_id = 0; chiplet_id < ChipletGridWidth * ChipletGridHeight; chiplet_id++) {
        for (int slot = 0; slot < VerticalLinksPerChiplet; slot++) {
            VerticalLinkInfo link;
            link.vl_id = chiplet_id * VerticalLinksPerChiplet + slot;
            link.owner_chiplet_id = chiplet_id;
            link.slot = static_cast<VerticalLinkSlot>(slot);
            link.chiplet_endpoint_router_id =
                chipletRouterId(chiplet_id, SLOT_LOCAL_X[slot], SLOT_LOCAL_Y[slot]);

            link.footprint_x = link.chiplet_endpoint_router_id % FootprintWidth;
            link.footprint_y = link.chiplet_endpoint_router_id / FootprintWidth;
            link.interposer_endpoint_router_id =
                interposerRouterId(link.footprint_x, link.footprint_y);
            link.is_functional = true;

            links.push_back(link);
        }
    }

    return links;
}

bool validFootprint(int x, int y)
{
    return x >= 0 && x < FootprintWidth && y >= 0 && y < FootprintHeight;
}

} // namespace

bool isChipletRouter(int id)
{
    return id >= 0 && id < ChipletRouterCount;
}

bool isInterposerRouter(int id)
{
    return id >= ChipletRouterCount && id < TotalRouters;
}

RouterInfo decodeRouterId(int id)
{
    RouterInfo info;
    info.id = id;
    info.layer = ROUTER_LAYER_INVALID;
    info.footprint_x = -1;
    info.footprint_y = -1;
    info.chiplet_id = -1;
    info.local_x = -1;
    info.local_y = -1;
    info.boundary_router = false;
    info.vertical_link_id = -1;

    int footprint_index = -1;
    if (isChipletRouter(id)) {
        info.layer = ROUTER_LAYER_CHIPLET;
        footprint_index = id;
    } else if (isInterposerRouter(id)) {
        info.layer = ROUTER_LAYER_INTERPOSER;
        footprint_index = id - ChipletRouterCount;
    } else {
        return info;
    }

    info.footprint_x = footprint_index % FootprintWidth;
    info.footprint_y = footprint_index / FootprintWidth;
    int chiplet_x = info.footprint_x / ChipletLocalWidth;
    int chiplet_y = info.footprint_y / ChipletLocalHeight;
    info.chiplet_id = chiplet_y * ChipletGridWidth + chiplet_x;
    info.local_x = info.footprint_x % ChipletLocalWidth;
    info.local_y = info.footprint_y % ChipletLocalHeight;

    const VerticalLinkInfo *boundary_link = verticalLinkForBoundaryRouter(id);
    if (boundary_link != 0) {
        info.boundary_router = true;
        info.vertical_link_id = boundary_link->vl_id;
    } else {
        const VerticalLinkInfo *interposer_link = verticalLinkForInterposerRouter(id);
        if (interposer_link != 0)
            info.vertical_link_id = interposer_link->vl_id;
    }

    return info;
}

int chipletRouterId(int chiplet_id, int local_x, int local_y)
{
    assert(chiplet_id >= 0);
    assert(chiplet_id < ChipletGridWidth * ChipletGridHeight);
    assert(local_x >= 0 && local_x < ChipletLocalWidth);
    assert(local_y >= 0 && local_y < ChipletLocalHeight);

    int origin_x = (chiplet_id % ChipletGridWidth) * ChipletLocalWidth;
    int origin_y = (chiplet_id / ChipletGridWidth) * ChipletLocalHeight;
    return (origin_y + local_y) * FootprintWidth + origin_x + local_x;
}

int interposerRouterId(int footprint_x, int footprint_y)
{
    assert(validFootprint(footprint_x, footprint_y));
    return ChipletRouterCount + footprint_y * FootprintWidth + footprint_x;
}

bool isBoundaryRouter(int id)
{
    return verticalLinkForBoundaryRouter(id) != 0;
}

const std::vector<VerticalLinkInfo> &verticalLinks()
{
    static std::vector<VerticalLinkInfo> links = buildVerticalLinks();
    return links;
}

std::vector<VerticalLinkInfo> verticalLinksForChiplet(int chiplet_id)
{
    std::vector<VerticalLinkInfo> result;
    const std::vector<VerticalLinkInfo> &links = verticalLinks();

    for (std::vector<VerticalLinkInfo>::const_iterator it = links.begin();
         it != links.end();
         ++it) {
        if (it->owner_chiplet_id == chiplet_id)
            result.push_back(*it);
    }

    return result;
}

const VerticalLinkInfo *verticalLinkById(int vl_id)
{
    const std::vector<VerticalLinkInfo> &links = verticalLinks();
    for (std::vector<VerticalLinkInfo>::const_iterator it = links.begin();
         it != links.end();
         ++it) {
        if (it->vl_id == vl_id)
            return &(*it);
    }

    return 0;
}

const VerticalLinkInfo *verticalLinkForBoundaryRouter(int id)
{
    if (!isChipletRouter(id))
        return 0;

    const std::vector<VerticalLinkInfo> &links = verticalLinks();
    for (std::vector<VerticalLinkInfo>::const_iterator it = links.begin();
         it != links.end();
         ++it) {
        if (it->chiplet_endpoint_router_id == id)
            return &(*it);
    }

    return 0;
}

const VerticalLinkInfo *verticalLinkForInterposerRouter(int id)
{
    if (!isInterposerRouter(id))
        return 0;

    const std::vector<VerticalLinkInfo> &links = verticalLinks();
    for (std::vector<VerticalLinkInfo>::const_iterator it = links.begin();
         it != links.end();
         ++it) {
        if (it->interposer_endpoint_router_id == id)
            return &(*it);
    }

    return 0;
}

int interposerEndpointForVerticalLink(int vl_id)
{
    const VerticalLinkInfo *link = verticalLinkById(vl_id);
    return link == 0 ? -1 : link->interposer_endpoint_router_id;
}

std::string layerName(RouterLayer layer)
{
    if (layer == ROUTER_LAYER_CHIPLET)
        return "chiplet";
    if (layer == ROUTER_LAYER_INTERPOSER)
        return "interposer";
    return "invalid";
}

std::string slotName(VerticalLinkSlot slot)
{
    switch (slot) {
    case VL_SLOT_NORTH:
        return "NORTH";
    case VL_SLOT_EAST:
        return "EAST";
    case VL_SLOT_SOUTH:
        return "SOUTH";
    case VL_SLOT_WEST:
        return "WEST";
    }

    return "UNKNOWN";
}

} // namespace DeftTopology
