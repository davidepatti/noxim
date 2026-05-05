/*
 * DeFT 2.5D topology mapping helpers.
 */

#ifndef __NOXIMDEFTTOPOLOGY_H__
#define __NOXIMDEFTTOPOLOGY_H__

#include <string>
#include <vector>

namespace DeftTopology {

static const int ChipletGridWidth = 2;
static const int ChipletGridHeight = 2;
static const int ChipletLocalWidth = 4;
static const int ChipletLocalHeight = 4;
static const int FootprintWidth = ChipletGridWidth * ChipletLocalWidth;
static const int FootprintHeight = ChipletGridHeight * ChipletLocalHeight;
static const int ChipletRouterCount = FootprintWidth * FootprintHeight;
static const int InterposerRouterCount = FootprintWidth * FootprintHeight;
static const int TotalRouters = ChipletRouterCount + InterposerRouterCount;
static const int VerticalLinksPerChiplet = 4;
static const int VerticalLinkCount =
    ChipletGridWidth * ChipletGridHeight * VerticalLinksPerChiplet;
static const int LayoutWidth = FootprintWidth;
static const int LayoutHeight = FootprintHeight * 2;

enum RouterLayer {
    ROUTER_LAYER_INVALID = -1,
    ROUTER_LAYER_CHIPLET = 0,
    ROUTER_LAYER_INTERPOSER = 1
};

enum VerticalLinkSlot {
    VL_SLOT_NORTH = 0,
    VL_SLOT_EAST = 1,
    VL_SLOT_SOUTH = 2,
    VL_SLOT_WEST = 3
};

struct RouterInfo {
    int id;
    RouterLayer layer;
    int footprint_x;
    int footprint_y;
    int chiplet_id;
    int local_x;
    int local_y;
    bool boundary_router;
    int vertical_link_id;
};

struct VerticalLinkInfo {
    int vl_id;
    int owner_chiplet_id;
    VerticalLinkSlot slot;
    int chiplet_endpoint_router_id;
    int interposer_endpoint_router_id;
    int footprint_x;
    int footprint_y;
    bool is_functional;
};

bool isChipletRouter(int id);
bool isInterposerRouter(int id);
RouterInfo decodeRouterId(int id);
int chipletRouterId(int chiplet_id, int local_x, int local_y);
int interposerRouterId(int footprint_x, int footprint_y);
bool isBoundaryRouter(int id);
const std::vector<VerticalLinkInfo> &verticalLinks();
std::vector<VerticalLinkInfo> verticalLinksForChiplet(int chiplet_id);
const VerticalLinkInfo *verticalLinkById(int vl_id);
const VerticalLinkInfo *verticalLinkForBoundaryRouter(int id);
const VerticalLinkInfo *verticalLinkForInterposerRouter(int id);
int interposerEndpointForVerticalLink(int vl_id);
std::string layerName(RouterLayer layer);
std::string slotName(VerticalLinkSlot slot);

} // namespace DeftTopology

#endif
