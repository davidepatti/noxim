/*
 * DeFT startup-time Vertical Link fault injection.
 */

#ifndef __NOXIMDEFTFAULTINJECTIONMANAGER_H__
#define __NOXIMDEFTFAULTINJECTIONMANAGER_H__

#include <string>
#include <vector>

namespace DeftFaultInjection {

struct StartupFaultConfig {
    std::vector<int> explicit_faulty_vertical_links;
    int random_fault_count;
    int random_seed;
};

struct FaultMaskValidationReport {
    std::vector<int> normalized_faulty_vertical_links;
    std::vector<int> faulty_vertical_links_per_chiplet;
    std::vector<int> functional_vertical_links_per_chiplet;
    int physical_fault_count;
    int physical_vertical_link_count;
    bool matches_current_physical_25_percent_target;
};

struct StartupFaultReport {
    std::vector<int> faulty_vertical_links;
    int requested_fault_count;
    bool used_random_selection;
    FaultMaskValidationReport mask_validation;
};

bool validateFaultMask(const std::vector<int> &fault_ids,
                       FaultMaskValidationReport *report = 0,
                       std::string *error_message = 0);
bool applyStartupFaults(const StartupFaultConfig &config,
                        StartupFaultReport *report,
                        std::string *error_message = 0);
std::vector<int> faultyVerticalLinkIds();
int functionalVerticalLinkCountForChiplet(int chiplet_id);
bool validateChipletConnectivity(std::string *error_message = 0);
std::string formatVerticalLinkList(const std::vector<int> &vertical_link_ids);

} // namespace DeftFaultInjection

#endif
