/*
 * DeFT 2.5D topology mapping helpers.
 */

#include "DeftTopology.h"

#include <cassert>
#include <sstream>

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

bool validChipletId(int chiplet_id)
{
    return chiplet_id >= 0 && chiplet_id < ChipletCount;
}

bool validVerticalLinkSlot(VerticalLinkSlot slot)
{
    return slot >= VL_SLOT_NORTH && slot <= VL_SLOT_WEST;
}

std::vector<VerticalLinkInfo> &mutableVerticalLinks()
{
    static std::vector<VerticalLinkInfo> links = buildVerticalLinks();
    return links;
}

bool failValidation(std::string *error_message, const std::string &message)
{
    if (error_message != 0)
        *error_message = message;
    return false;
}

BoundaryRouterInfo boundaryRouterFromVerticalLink(const VerticalLinkInfo &link)
{
    RouterInfo router = decodeRouterId(link.chiplet_endpoint_router_id);

    BoundaryRouterInfo boundary_router;
    boundary_router.router_id = link.chiplet_endpoint_router_id;
    boundary_router.owner_chiplet_id = link.owner_chiplet_id;
    boundary_router.local_x = router.local_x;
    boundary_router.local_y = router.local_y;
    boundary_router.slot = link.slot;
    boundary_router.vertical_link_id = link.vl_id;
    boundary_router.interposer_endpoint_router_id = link.interposer_endpoint_router_id;
    return boundary_router;
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

std::vector<BoundaryRouterInfo> boundaryRouters()
{
    std::vector<BoundaryRouterInfo> result;
    const std::vector<VerticalLinkInfo> &links = verticalLinks();
    result.reserve(links.size());

    for (std::vector<VerticalLinkInfo>::const_iterator it = links.begin();
         it != links.end();
         ++it) {
        result.push_back(boundaryRouterFromVerticalLink(*it));
    }

    return result;
}

std::vector<BoundaryRouterInfo> boundaryRoutersForChiplet(int chiplet_id)
{
    std::vector<BoundaryRouterInfo> result;
    const std::vector<VerticalLinkInfo> &links = verticalLinks();

    for (std::vector<VerticalLinkInfo>::const_iterator it = links.begin();
         it != links.end();
         ++it) {
        if (it->owner_chiplet_id == chiplet_id)
            result.push_back(boundaryRouterFromVerticalLink(*it));
    }

    return result;
}

bool boundaryRouterById(int router_id, BoundaryRouterInfo *boundary_router)
{
    const VerticalLinkInfo *link = verticalLinkForBoundaryRouter(router_id);
    if (link == 0)
        return false;

    if (boundary_router != 0)
        *boundary_router = boundaryRouterFromVerticalLink(*link);
    return true;
}

bool boundaryRouterForVerticalLink(int vl_id, BoundaryRouterInfo *boundary_router)
{
    const VerticalLinkInfo *link = verticalLinkById(vl_id);
    if (link == 0)
        return false;

    if (boundary_router != 0)
        *boundary_router = boundaryRouterFromVerticalLink(*link);
    return true;
}

const std::vector<VerticalLinkInfo> &verticalLinks()
{
    return mutableVerticalLinks();
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

std::vector<VerticalLinkInfo> functionalVerticalLinksForChiplet(int chiplet_id)
{
    std::vector<VerticalLinkInfo> result;
    const std::vector<VerticalLinkInfo> &links = verticalLinks();

    for (std::vector<VerticalLinkInfo>::const_iterator it = links.begin();
         it != links.end();
         ++it) {
        if (it->owner_chiplet_id == chiplet_id && it->is_functional)
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

const VerticalLinkInfo *verticalLinkBetweenRouters(int router_a_id, int router_b_id)
{
    const std::vector<VerticalLinkInfo> &links = verticalLinks();
    for (std::vector<VerticalLinkInfo>::const_iterator it = links.begin();
         it != links.end();
         ++it) {
        const bool forward =
            it->chiplet_endpoint_router_id == router_a_id &&
            it->interposer_endpoint_router_id == router_b_id;
        const bool reverse =
            it->chiplet_endpoint_router_id == router_b_id &&
            it->interposer_endpoint_router_id == router_a_id;

        if (forward || reverse)
            return &(*it);
    }

    return 0;
}

bool isVerticalLinkFunctional(int vl_id)
{
    const VerticalLinkInfo *link = verticalLinkById(vl_id);
    return link != 0 && link->is_functional;
}

bool setVerticalLinkFunctional(int vl_id, bool is_functional)
{
    std::vector<VerticalLinkInfo> &links = mutableVerticalLinks();
    for (std::vector<VerticalLinkInfo>::iterator it = links.begin();
         it != links.end();
         ++it) {
        if (it->vl_id == vl_id) {
            it->is_functional = is_functional;
            return true;
        }
    }

    return false;
}

void resetVerticalLinkStates()
{
    std::vector<VerticalLinkInfo> &links = mutableVerticalLinks();
    for (std::vector<VerticalLinkInfo>::iterator it = links.begin();
         it != links.end();
         ++it) {
        it->is_functional = true;
    }
}

bool hasFunctionalVerticalLinkForChiplet(int chiplet_id)
{
    if (!validChipletId(chiplet_id))
        return false;

    const std::vector<VerticalLinkInfo> &links = verticalLinks();
    for (std::vector<VerticalLinkInfo>::const_iterator it = links.begin();
         it != links.end();
         ++it) {
        if (it->owner_chiplet_id == chiplet_id && it->is_functional)
            return true;
    }

    return false;
}

bool validateVerticalLinkModel(std::string *error_message)
{
    const std::vector<VerticalLinkInfo> &links = verticalLinks();
    if (links.size() != VerticalLinkCount) {
        std::ostringstream message;
        message << "expected " << VerticalLinkCount << " vertical links, found "
                << links.size();
        return failValidation(error_message, message.str());
    }

    std::vector<bool> seen_ids(VerticalLinkCount, false);
    std::vector<bool> seen_chiplet_endpoints(ChipletRouterCount, false);
    std::vector<bool> seen_interposer_endpoints(InterposerRouterCount, false);
    std::vector<int> links_per_chiplet(ChipletCount, 0);

    for (std::vector<VerticalLinkInfo>::const_iterator it = links.begin();
         it != links.end();
         ++it) {
        if (it->vl_id < 0 || it->vl_id >= VerticalLinkCount) {
            std::ostringstream message;
            message << "vertical link id out of range: " << it->vl_id;
            return failValidation(error_message, message.str());
        }

        if (seen_ids[it->vl_id]) {
            std::ostringstream message;
            message << "duplicate vertical link id: " << it->vl_id;
            return failValidation(error_message, message.str());
        }
        seen_ids[it->vl_id] = true;

        if (!validChipletId(it->owner_chiplet_id)) {
            std::ostringstream message;
            message << "vertical link " << it->vl_id
                    << " has invalid owner chiplet " << it->owner_chiplet_id;
            return failValidation(error_message, message.str());
        }

        if (!validVerticalLinkSlot(it->slot)) {
            std::ostringstream message;
            message << "vertical link " << it->vl_id << " has invalid slot";
            return failValidation(error_message, message.str());
        }

        const int slot = static_cast<int>(it->slot);
        const int expected_vl_id =
            it->owner_chiplet_id * VerticalLinksPerChiplet + slot;
        if (it->vl_id != expected_vl_id) {
            std::ostringstream message;
            message << "vertical link " << it->vl_id
                    << " does not match owner chiplet and slot";
            return failValidation(error_message, message.str());
        }

        const int expected_chiplet_endpoint =
            chipletRouterId(it->owner_chiplet_id, SLOT_LOCAL_X[slot], SLOT_LOCAL_Y[slot]);
        if (it->chiplet_endpoint_router_id != expected_chiplet_endpoint) {
            std::ostringstream message;
            message << "vertical link " << it->vl_id
                    << " has unexpected chiplet endpoint";
            return failValidation(error_message, message.str());
        }

        if (!isChipletRouter(it->chiplet_endpoint_router_id)) {
            std::ostringstream message;
            message << "vertical link " << it->vl_id
                    << " chiplet endpoint is not a chiplet router";
            return failValidation(error_message, message.str());
        }

        const int footprint_x = it->chiplet_endpoint_router_id % FootprintWidth;
        const int footprint_y = it->chiplet_endpoint_router_id / FootprintWidth;
        if (it->footprint_x != footprint_x || it->footprint_y != footprint_y) {
            std::ostringstream message;
            message << "vertical link " << it->vl_id
                    << " footprint does not match its chiplet endpoint";
            return failValidation(error_message, message.str());
        }

        const int expected_interposer_endpoint =
            interposerRouterId(footprint_x, footprint_y);
        if (it->interposer_endpoint_router_id != expected_interposer_endpoint) {
            std::ostringstream message;
            message << "vertical link " << it->vl_id
                    << " has unexpected interposer endpoint";
            return failValidation(error_message, message.str());
        }

        const int interposer_index =
            it->interposer_endpoint_router_id - ChipletRouterCount;
        if (interposer_index < 0 || interposer_index >= InterposerRouterCount) {
            std::ostringstream message;
            message << "vertical link " << it->vl_id
                    << " interposer endpoint is not an interposer router";
            return failValidation(error_message, message.str());
        }

        if (seen_chiplet_endpoints[it->chiplet_endpoint_router_id]) {
            std::ostringstream message;
            message << "duplicate chiplet endpoint for vertical link " << it->vl_id;
            return failValidation(error_message, message.str());
        }
        seen_chiplet_endpoints[it->chiplet_endpoint_router_id] = true;

        if (seen_interposer_endpoints[interposer_index]) {
            std::ostringstream message;
            message << "duplicate interposer endpoint for vertical link " << it->vl_id;
            return failValidation(error_message, message.str());
        }
        seen_interposer_endpoints[interposer_index] = true;

        links_per_chiplet[it->owner_chiplet_id]++;
    }

    for (int chiplet_id = 0; chiplet_id < ChipletCount; chiplet_id++) {
        if (links_per_chiplet[chiplet_id] != VerticalLinksPerChiplet) {
            std::ostringstream message;
            message << "chiplet " << chiplet_id << " has "
                    << links_per_chiplet[chiplet_id]
                    << " vertical links";
            return failValidation(error_message, message.str());
        }
    }

    if (error_message != 0)
        error_message->clear();
    return true;
}

bool validateBoundaryRouterModel(std::string *error_message)
{
    std::string vertical_link_error;
    if (!validateVerticalLinkModel(&vertical_link_error)) {
        std::ostringstream message;
        message << "vertical link model invalid: " << vertical_link_error;
        return failValidation(error_message, message.str());
    }

    const std::vector<BoundaryRouterInfo> boundary_routers = boundaryRouters();
    if (boundary_routers.size() != VerticalLinkCount) {
        std::ostringstream message;
        message << "expected " << VerticalLinkCount
                << " boundary routers, found " << boundary_routers.size();
        return failValidation(error_message, message.str());
    }

    std::vector<bool> seen_router_ids(ChipletRouterCount, false);
    std::vector<bool> seen_vertical_link_ids(VerticalLinkCount, false);
    std::vector<int> boundary_routers_per_chiplet(ChipletCount, 0);

    for (std::vector<BoundaryRouterInfo>::const_iterator it = boundary_routers.begin();
         it != boundary_routers.end();
         ++it) {
        if (!isChipletRouter(it->router_id)) {
            std::ostringstream message;
            message << "boundary router " << it->router_id
                    << " is not a chiplet router";
            return failValidation(error_message, message.str());
        }

        if (seen_router_ids[it->router_id]) {
            std::ostringstream message;
            message << "duplicate boundary router id: " << it->router_id;
            return failValidation(error_message, message.str());
        }
        seen_router_ids[it->router_id] = true;

        if (!validChipletId(it->owner_chiplet_id)) {
            std::ostringstream message;
            message << "boundary router " << it->router_id
                    << " has invalid owner chiplet " << it->owner_chiplet_id;
            return failValidation(error_message, message.str());
        }

        if (it->local_x < 0 || it->local_x >= ChipletLocalWidth ||
            it->local_y < 0 || it->local_y >= ChipletLocalHeight) {
            std::ostringstream message;
            message << "boundary router " << it->router_id
                    << " has invalid local coordinate";
            return failValidation(error_message, message.str());
        }

        RouterInfo router = decodeRouterId(it->router_id);
        if (router.layer != ROUTER_LAYER_CHIPLET ||
            router.chiplet_id != it->owner_chiplet_id ||
            router.local_x != it->local_x ||
            router.local_y != it->local_y ||
            !router.boundary_router) {
            std::ostringstream message;
            message << "boundary router " << it->router_id
                    << " does not match decoded router metadata";
            return failValidation(error_message, message.str());
        }

        if (!validVerticalLinkSlot(it->slot)) {
            std::ostringstream message;
            message << "boundary router " << it->router_id
                    << " has invalid vertical link slot";
            return failValidation(error_message, message.str());
        }

        if (it->vertical_link_id < 0 || it->vertical_link_id >= VerticalLinkCount) {
            std::ostringstream message;
            message << "boundary router " << it->router_id
                    << " has invalid vertical link id " << it->vertical_link_id;
            return failValidation(error_message, message.str());
        }

        if (seen_vertical_link_ids[it->vertical_link_id]) {
            std::ostringstream message;
            message << "duplicate boundary vertical link id: "
                    << it->vertical_link_id;
            return failValidation(error_message, message.str());
        }
        seen_vertical_link_ids[it->vertical_link_id] = true;

        const VerticalLinkInfo *link = verticalLinkById(it->vertical_link_id);
        if (link == 0) {
            std::ostringstream message;
            message << "boundary router " << it->router_id
                    << " references missing vertical link "
                    << it->vertical_link_id;
            return failValidation(error_message, message.str());
        }

        if (link->owner_chiplet_id != it->owner_chiplet_id ||
            link->slot != it->slot ||
            link->chiplet_endpoint_router_id != it->router_id ||
            link->interposer_endpoint_router_id != it->interposer_endpoint_router_id) {
            std::ostringstream message;
            message << "boundary router " << it->router_id
                    << " does not match attached vertical link metadata";
            return failValidation(error_message, message.str());
        }

        RouterInfo interposer_endpoint =
            decodeRouterId(it->interposer_endpoint_router_id);
        if (interposer_endpoint.layer != ROUTER_LAYER_INTERPOSER ||
            interposer_endpoint.footprint_x != router.footprint_x ||
            interposer_endpoint.footprint_y != router.footprint_y) {
            std::ostringstream message;
            message << "boundary router " << it->router_id
                    << " has invalid attached interposer endpoint";
            return failValidation(error_message, message.str());
        }

        BoundaryRouterInfo boundary_router_by_id;
        if (!boundaryRouterById(it->router_id, &boundary_router_by_id) ||
            boundary_router_by_id.vertical_link_id != it->vertical_link_id) {
            std::ostringstream message;
            message << "boundary router " << it->router_id
                    << " is not queryable by router id";
            return failValidation(error_message, message.str());
        }

        BoundaryRouterInfo boundary_router_by_vl;
        if (!boundaryRouterForVerticalLink(it->vertical_link_id,
                                           &boundary_router_by_vl) ||
            boundary_router_by_vl.router_id != it->router_id) {
            std::ostringstream message;
            message << "boundary router " << it->router_id
                    << " is not queryable by vertical link id";
            return failValidation(error_message, message.str());
        }

        boundary_routers_per_chiplet[it->owner_chiplet_id]++;
    }

    for (int chiplet_id = 0; chiplet_id < ChipletCount; chiplet_id++) {
        if (boundary_routers_per_chiplet[chiplet_id] != VerticalLinksPerChiplet) {
            std::ostringstream message;
            message << "chiplet " << chiplet_id << " has "
                    << boundary_routers_per_chiplet[chiplet_id]
                    << " boundary routers";
            return failValidation(error_message, message.str());
        }
    }

    if (error_message != 0)
        error_message->clear();
    return true;
}

int chipletEndpointForVerticalLink(int vl_id)
{
    const VerticalLinkInfo *link = verticalLinkById(vl_id);
    return link == 0 ? -1 : link->chiplet_endpoint_router_id;
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
