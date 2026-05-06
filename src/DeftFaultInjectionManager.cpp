/*
 * DeFT startup-time Vertical Link fault injection.
 */

#include "DeftFaultInjectionManager.h"
#include "DeftTopology.h"

#include <algorithm>
#include <random>
#include <sstream>

namespace DeftFaultInjection {

namespace {

bool fail(std::string *error_message, const std::string &message)
{
    if (error_message != 0)
        *error_message = message;
    return false;
}

bool validateFaultIds(const std::vector<int> &fault_ids,
                      std::string *error_message)
{
    std::vector<bool> seen_ids(DeftTopology::VerticalLinkCount, false);
    for (std::vector<int>::const_iterator it = fault_ids.begin();
         it != fault_ids.end();
         ++it) {
        if (*it < 0 || *it >= DeftTopology::VerticalLinkCount) {
            std::ostringstream message;
            message << "vertical link fault id out of range: " << *it;
            return fail(error_message, message.str());
        }

        if (seen_ids[*it]) {
            std::ostringstream message;
            message << "duplicate vertical link fault id: " << *it;
            return fail(error_message, message.str());
        }
        seen_ids[*it] = true;
    }

    return true;
}

std::vector<int> generateRandomFaultIds(int random_fault_count, int random_seed)
{
    std::vector<int> candidates;
    candidates.reserve(DeftTopology::VerticalLinkCount);

    const std::vector<DeftTopology::VerticalLinkInfo> &links =
        DeftTopology::verticalLinks();
    for (std::vector<DeftTopology::VerticalLinkInfo>::const_iterator it =
             links.begin();
         it != links.end();
         ++it) {
        candidates.push_back(it->vl_id);
    }

    std::mt19937 generator(static_cast<unsigned int>(random_seed));
    std::shuffle(candidates.begin(), candidates.end(), generator);

    std::vector<int> selected_faults;
    selected_faults.reserve(random_fault_count);
    std::vector<int> functional_links_per_chiplet(
        DeftTopology::ChipletCount,
        DeftTopology::VerticalLinksPerChiplet);

    for (std::vector<int>::const_iterator it = candidates.begin();
         it != candidates.end() &&
             selected_faults.size() < static_cast<size_t>(random_fault_count);
         ++it) {
        const DeftTopology::VerticalLinkInfo *link =
            DeftTopology::verticalLinkById(*it);
        if (link == 0)
            continue;

        if (functional_links_per_chiplet[link->owner_chiplet_id] <= 1)
            continue;

        selected_faults.push_back(*it);
        functional_links_per_chiplet[link->owner_chiplet_id]--;
    }

    std::sort(selected_faults.begin(), selected_faults.end());
    return selected_faults;
}

} // namespace

bool applyStartupFaults(const StartupFaultConfig &config,
                        StartupFaultReport *report,
                        std::string *error_message)
{
    DeftTopology::resetVerticalLinkStates();

    StartupFaultReport local_report;
    local_report.requested_fault_count =
        config.explicit_faulty_vertical_links.empty()
            ? config.random_fault_count
            : static_cast<int>(config.explicit_faulty_vertical_links.size());
    local_report.used_random_selection =
        config.explicit_faulty_vertical_links.empty() &&
        config.random_fault_count > 0;

    std::vector<int> selected_faults;
    if (!config.explicit_faulty_vertical_links.empty()) {
        selected_faults = config.explicit_faulty_vertical_links;
        std::sort(selected_faults.begin(), selected_faults.end());
    } else if (config.random_fault_count > 0) {
        selected_faults = generateRandomFaultIds(config.random_fault_count,
                                                 config.random_seed);
        if (selected_faults.size() !=
            static_cast<size_t>(config.random_fault_count)) {
            std::ostringstream message;
            message << "could not generate " << config.random_fault_count
                    << " vertical link faults without disconnecting a chiplet";
            return fail(error_message, message.str());
        }
    }

    if (!validateFaultIds(selected_faults, error_message)) {
        DeftTopology::resetVerticalLinkStates();
        return false;
    }

    for (std::vector<int>::const_iterator it = selected_faults.begin();
         it != selected_faults.end();
         ++it) {
        if (!DeftTopology::setVerticalLinkFunctional(*it, false)) {
            std::ostringstream message;
            message << "failed to mark vertical link " << *it << " faulty";
            DeftTopology::resetVerticalLinkStates();
            return fail(error_message, message.str());
        }
    }

    if (!validateChipletConnectivity(error_message)) {
        DeftTopology::resetVerticalLinkStates();
        return false;
    }

    local_report.faulty_vertical_links = faultyVerticalLinkIds();
    if (report != 0)
        *report = local_report;
    if (error_message != 0)
        error_message->clear();
    return true;
}

std::vector<int> faultyVerticalLinkIds()
{
    std::vector<int> result;
    const std::vector<DeftTopology::VerticalLinkInfo> &links =
        DeftTopology::verticalLinks();

    for (std::vector<DeftTopology::VerticalLinkInfo>::const_iterator it =
             links.begin();
         it != links.end();
         ++it) {
        if (!it->is_functional)
            result.push_back(it->vl_id);
    }

    return result;
}

int functionalVerticalLinkCountForChiplet(int chiplet_id)
{
    return static_cast<int>(
        DeftTopology::functionalVerticalLinksForChiplet(chiplet_id).size());
}

bool validateChipletConnectivity(std::string *error_message)
{
    for (int chiplet_id = 0;
         chiplet_id < DeftTopology::ChipletCount;
         chiplet_id++) {
        if (!DeftTopology::hasFunctionalVerticalLinkForChiplet(chiplet_id)) {
            std::ostringstream message;
            message << "chiplet " << chiplet_id
                    << " has no functional vertical links";
            return fail(error_message, message.str());
        }
    }

    if (error_message != 0)
        error_message->clear();
    return true;
}

std::string formatVerticalLinkList(const std::vector<int> &vertical_link_ids)
{
    std::ostringstream out;
    out << "[";
    for (std::vector<int>::const_iterator it = vertical_link_ids.begin();
         it != vertical_link_ids.end();
         ++it) {
        if (it != vertical_link_ids.begin())
            out << ",";
        out << *it;
    }
    out << "]";
    return out.str();
}

} // namespace DeftFaultInjection
