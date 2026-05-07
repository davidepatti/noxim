/*
 * DeFT runtime Vertical Link lookup table loader.
 */

#ifndef __NOXIMDEFTVERTICALLINKLUT_H__
#define __NOXIMDEFTVERTICALLINKLUT_H__

#include <cstddef>
#include <string>
#include <vector>

namespace DeftVerticalLinkLut {

struct VerticalLinkSelection {
    VerticalLinkSelection();

    int selected_vl_id;
    int boundary_router_id;
    int interposer_endpoint_router_id;
    std::vector<int> ranked_vl_ids;
};

struct Entry {
    Entry();

    int fault_mask;
    int source_chiplet_id;
    int source_router_id;
    int destination_chiplet_id;
    VerticalLinkSelection source_exit;
    VerticalLinkSelection destination_entry;
};

bool loadFromFile(const std::string &filename,
                  std::string *error_message = 0);
bool isLoaded();
std::string loadedFilename();
std::size_t entryCount();
std::size_t activeFaultMaskEntryCount();
int currentFaultMask();
std::string currentFaultMaskId();
std::string formatFaultMask(int fault_mask);

bool lookup(int source_chiplet_id,
            int source_router_id,
            int destination_chiplet_id,
            Entry *entry,
            std::string *error_message = 0);

} // namespace DeftVerticalLinkLut

#endif
