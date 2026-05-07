/*
 * DeFT runtime Vertical Link lookup table loader.
 */

#include "DeftVerticalLinkLut.h"

#include "DeftTopology.h"
#include "GlobalParams.h"

#include <yaml-cpp/yaml.h>

#include <iomanip>
#include <map>
#include <set>
#include <sstream>

namespace {

struct LookupKey {
    int fault_mask;
    int source_chiplet_id;
    int source_router_id;
    int destination_chiplet_id;

    bool operator<(const LookupKey &other) const
    {
        if (fault_mask != other.fault_mask)
            return fault_mask < other.fault_mask;
        if (source_chiplet_id != other.source_chiplet_id)
            return source_chiplet_id < other.source_chiplet_id;
        if (source_router_id != other.source_router_id)
            return source_router_id < other.source_router_id;
        return destination_chiplet_id < other.destination_chiplet_id;
    }
};

typedef std::map<LookupKey, DeftVerticalLinkLut::Entry> EntryMap;

EntryMap loaded_entries;
std::string loaded_filename;
bool loaded = false;

bool fail(std::string *error_message, const std::string &message)
{
    if (error_message != 0)
        *error_message = message;
    return false;
}

bool validChipletId(int chiplet_id)
{
    return chiplet_id >= 0 && chiplet_id < DeftTopology::ChipletCount;
}

bool parseFaultMaskId(const std::string &text,
                      int *fault_mask,
                      std::string *error_message)
{
    if (text.size() != 6 || text[0] != '0' ||
        (text[1] != 'x' && text[1] != 'X')) {
        std::ostringstream message;
        message << "fault_mask_id must use fixed-width hex form 0x0000, got "
                << text;
        return fail(error_message, message.str());
    }

    int value = 0;
    for (std::string::const_iterator it = text.begin() + 2;
         it != text.end();
         ++it) {
        int digit = -1;
        if (*it >= '0' && *it <= '9')
            digit = *it - '0';
        else if (*it >= 'a' && *it <= 'f')
            digit = 10 + (*it - 'a');
        else if (*it >= 'A' && *it <= 'F')
            digit = 10 + (*it - 'A');

        if (digit < 0) {
            std::ostringstream message;
            message << "fault_mask_id contains a non-hex digit: " << text;
            return fail(error_message, message.str());
        }
        value = (value << 4) | digit;
    }

    if (fault_mask != 0)
        *fault_mask = value;
    return true;
}

std::string requiredContext(const std::string &context,
                            const std::string &field)
{
    std::ostringstream message;
    message << context << " missing required field " << field;
    return message.str();
}

bool readString(const YAML::Node &node,
                const std::string &field,
                const std::string &context,
                std::string *value,
                std::string *error_message)
{
    if (!node[field])
        return fail(error_message, requiredContext(context, field));

    *value = node[field].as<std::string>();
    return true;
}

bool readInt(const YAML::Node &node,
             const std::string &field,
             const std::string &context,
             int *value,
             std::string *error_message)
{
    if (!node[field])
        return fail(error_message, requiredContext(context, field));

    *value = node[field].as<int>();
    return true;
}

bool readIntVector(const YAML::Node &node,
                   const std::string &field,
                   const std::string &context,
                   std::vector<int> *value,
                   std::string *error_message)
{
    if (!node[field])
        return fail(error_message, requiredContext(context, field));

    *value = node[field].as<std::vector<int> >();
    return true;
}

bool isFaultyInMask(int fault_mask, int vl_id)
{
    if (vl_id < 0 || vl_id >= DeftTopology::VerticalLinkCount)
        return true;

    return (fault_mask & (1 << vl_id)) != 0;
}

bool parseFaultScenarios(const YAML::Node &root,
                         std::set<int> *scenario_masks,
                         std::string *error_message)
{
    if (!root["fault_scenarios"])
        return fail(error_message, "LUT missing required field fault_scenarios");

    const YAML::Node scenarios = root["fault_scenarios"];
    if (!scenarios.IsSequence())
        return fail(error_message, "fault_scenarios must be a sequence");

    for (YAML::const_iterator it = scenarios.begin();
         it != scenarios.end();
         ++it) {
        std::string mask_text;
        if (!readString(*it,
                        "fault_mask_id",
                        "fault_scenarios entry",
                        &mask_text,
                        error_message))
            return false;

        int mask = 0;
        if (!parseFaultMaskId(mask_text, &mask, error_message))
            return false;

        if (!scenario_masks->insert(mask).second) {
            std::ostringstream message;
            message << "duplicate fault scenario " << mask_text;
            return fail(error_message, message.str());
        }
    }

    return true;
}

bool validateTopologySignature(const YAML::Node &root,
                               std::string *error_message)
{
    if (!root["topology_signature"])
        return fail(error_message, "LUT missing required field topology_signature");

    const YAML::Node signature = root["topology_signature"];
    int value = 0;
    if (!readInt(signature,
                 "chiplet_count",
                 "topology_signature",
                 &value,
                 error_message) ||
        value != DeftTopology::ChipletCount)
        return fail(error_message, "LUT chiplet_count does not match DEFT_2_5D");

    if (!readInt(signature,
                 "chiplet_router_count",
                 "topology_signature",
                 &value,
                 error_message) ||
        value != DeftTopology::ChipletRouterCount)
        return fail(error_message,
                    "LUT chiplet_router_count does not match DEFT_2_5D");

    if (!readInt(signature,
                 "interposer_router_count",
                 "topology_signature",
                 &value,
                 error_message) ||
        value != DeftTopology::InterposerRouterCount)
        return fail(error_message,
                    "LUT interposer_router_count does not match DEFT_2_5D");

    if (!readInt(signature,
                 "physical_vertical_link_count",
                 "topology_signature",
                 &value,
                 error_message) ||
        value != DeftTopology::VerticalLinkCount)
        return fail(error_message,
                    "LUT physical_vertical_link_count does not match DEFT_2_5D");

    if (!readInt(signature,
                 "vertical_links_per_chiplet",
                 "topology_signature",
                 &value,
                 error_message) ||
        value != DeftTopology::VerticalLinksPerChiplet)
        return fail(error_message,
                    "LUT vertical_links_per_chiplet does not match DEFT_2_5D");

    return true;
}

bool validateRankedCandidates(const std::vector<int> &ranked_vl_ids,
                              int selected_vl_id,
                              int owner_chiplet_id,
                              int fault_mask,
                              const std::string &context,
                              std::string *error_message)
{
    if (ranked_vl_ids.empty())
        return fail(error_message, context + " ranked_vl_ids must not be empty");

    if (ranked_vl_ids.front() != selected_vl_id) {
        std::ostringstream message;
        message << context << " selected_vl_id must be first in ranked_vl_ids";
        return fail(error_message, message.str());
    }

    std::set<int> seen;
    for (std::vector<int>::const_iterator it = ranked_vl_ids.begin();
         it != ranked_vl_ids.end();
         ++it) {
        const DeftTopology::VerticalLinkInfo *link =
            DeftTopology::verticalLinkById(*it);
        if (link == 0) {
            std::ostringstream message;
            message << context << " ranked_vl_ids references missing VL "
                    << *it;
            return fail(error_message, message.str());
        }

        if (link->owner_chiplet_id != owner_chiplet_id) {
            std::ostringstream message;
            message << context << " ranked VL " << *it
                    << " is not owned by chiplet " << owner_chiplet_id;
            return fail(error_message, message.str());
        }

        if (isFaultyInMask(fault_mask, *it)) {
            std::ostringstream message;
            message << context << " ranked VL " << *it
                    << " is faulty in the entry fault mask";
            return fail(error_message, message.str());
        }

        if (!seen.insert(*it).second) {
            std::ostringstream message;
            message << context << " ranked_vl_ids has duplicate VL " << *it;
            return fail(error_message, message.str());
        }
    }

    return true;
}

bool parseSelection(const YAML::Node &node,
                    int owner_chiplet_id,
                    int fault_mask,
                    const std::string &context,
                    DeftVerticalLinkLut::VerticalLinkSelection *selection,
                    std::string *error_message)
{
    if (!readInt(node,
                 "selected_vl_id",
                 context,
                 &selection->selected_vl_id,
                 error_message) ||
        !readInt(node,
                 "boundary_router_id",
                 context,
                 &selection->boundary_router_id,
                 error_message) ||
        !readInt(node,
                 "interposer_endpoint_router_id",
                 context,
                 &selection->interposer_endpoint_router_id,
                 error_message) ||
        !readIntVector(node,
                       "ranked_vl_ids",
                       context,
                       &selection->ranked_vl_ids,
                       error_message))
        return false;

    const DeftTopology::VerticalLinkInfo *link =
        DeftTopology::verticalLinkById(selection->selected_vl_id);
    if (link == 0) {
        std::ostringstream message;
        message << context << " selected_vl_id references missing VL "
                << selection->selected_vl_id;
        return fail(error_message, message.str());
    }

    if (link->owner_chiplet_id != owner_chiplet_id) {
        std::ostringstream message;
        message << context << " selected_vl_id "
                << selection->selected_vl_id
                << " is not owned by chiplet " << owner_chiplet_id;
        return fail(error_message, message.str());
    }

    if (isFaultyInMask(fault_mask, selection->selected_vl_id)) {
        std::ostringstream message;
        message << context << " selected_vl_id "
                << selection->selected_vl_id
                << " is faulty in the entry fault mask";
        return fail(error_message, message.str());
    }

    if (link->chiplet_endpoint_router_id != selection->boundary_router_id ||
        link->interposer_endpoint_router_id !=
            selection->interposer_endpoint_router_id) {
        std::ostringstream message;
        message << context << " endpoint fields do not match selected_vl_id "
                << selection->selected_vl_id;
        return fail(error_message, message.str());
    }

    return validateRankedCandidates(selection->ranked_vl_ids,
                                    selection->selected_vl_id,
                                    owner_chiplet_id,
                                    fault_mask,
                                    context,
                                    error_message);
}

bool parseEntry(const YAML::Node &node,
                const std::set<int> &scenario_masks,
                DeftVerticalLinkLut::Entry *entry,
                std::string *error_message)
{
    if (!node["key"])
        return fail(error_message, "LUT entry missing key");
    if (!node["value"])
        return fail(error_message, "LUT entry missing value");

    const YAML::Node key = node["key"];
    const YAML::Node value = node["value"];

    std::string mask_text;
    if (!readString(key,
                    "fault_mask_id",
                    "entry key",
                    &mask_text,
                    error_message) ||
        !parseFaultMaskId(mask_text, &entry->fault_mask, error_message) ||
        !readInt(key,
                 "source_chiplet_id",
                 "entry key",
                 &entry->source_chiplet_id,
                 error_message) ||
        !readInt(key,
                 "source_router_id",
                 "entry key",
                 &entry->source_router_id,
                 error_message) ||
        !readInt(key,
                 "destination_chiplet_id",
                 "entry key",
                 &entry->destination_chiplet_id,
                 error_message))
        return false;

    if (scenario_masks.find(entry->fault_mask) == scenario_masks.end()) {
        std::ostringstream message;
        message << "entry references fault scenario " << mask_text
                << " not listed in fault_scenarios";
        return fail(error_message, message.str());
    }

    if (!validChipletId(entry->source_chiplet_id) ||
        !validChipletId(entry->destination_chiplet_id)) {
        std::ostringstream message;
        message << "entry key has invalid source or destination chiplet id";
        return fail(error_message, message.str());
    }

    if (entry->source_chiplet_id == entry->destination_chiplet_id)
        return fail(error_message, "entry key is for intra-chiplet traffic");

    DeftTopology::RouterInfo source_router =
        DeftTopology::decodeRouterId(entry->source_router_id);
    if (source_router.layer != DeftTopology::ROUTER_LAYER_CHIPLET ||
        source_router.chiplet_id != entry->source_chiplet_id) {
        std::ostringstream message;
        message << "entry source_router_id " << entry->source_router_id
                << " does not belong to source_chiplet_id "
                << entry->source_chiplet_id;
        return fail(error_message, message.str());
    }

    if (!value["source_exit"])
        return fail(error_message, "entry value missing source_exit");
    if (!value["destination_entry"])
        return fail(error_message, "entry value missing destination_entry");

    if (!parseSelection(value["source_exit"],
                        entry->source_chiplet_id,
                        entry->fault_mask,
                        "source_exit",
                        &entry->source_exit,
                        error_message) ||
        !parseSelection(value["destination_entry"],
                        entry->destination_chiplet_id,
                        entry->fault_mask,
                        "destination_entry",
                        &entry->destination_entry,
                        error_message))
        return false;

    return true;
}

bool selectedLinkIsCurrentlyFunctional(
    const DeftVerticalLinkLut::VerticalLinkSelection &selection,
    int owner_chiplet_id,
    const std::string &context,
    std::string *error_message)
{
    const DeftTopology::VerticalLinkInfo *link =
        DeftTopology::verticalLinkById(selection.selected_vl_id);
    if (link == 0)
        return fail(error_message, context + " selected VL is missing");

    if (link->owner_chiplet_id != owner_chiplet_id ||
        link->chiplet_endpoint_router_id != selection.boundary_router_id ||
        link->interposer_endpoint_router_id !=
            selection.interposer_endpoint_router_id)
        return fail(error_message,
                    context + " selected VL no longer matches topology");

    if (!link->is_functional)
        return fail(error_message, context + " selected VL is currently faulty");

    return true;
}

} // namespace

namespace DeftVerticalLinkLut {

VerticalLinkSelection::VerticalLinkSelection()
    : selected_vl_id(-1),
      boundary_router_id(-1),
      interposer_endpoint_router_id(-1)
{
}

Entry::Entry()
    : fault_mask(0),
      source_chiplet_id(-1),
      source_router_id(-1),
      destination_chiplet_id(-1)
{
}

bool loadFromFile(const std::string &filename,
                  std::string *error_message)
{
    loaded_entries.clear();
    loaded_filename.clear();
    loaded = false;

    if (filename.empty()) {
        if (error_message != 0)
            error_message->clear();
        return true;
    }

    try {
        YAML::Node root = YAML::LoadFile(filename);

        std::string schema;
        if (!readString(root, "schema", "LUT root", &schema, error_message))
            return false;
        if (schema != "deft_vl_lut.v1") {
            std::ostringstream message;
            message << "unsupported VL LUT schema " << schema;
            return fail(error_message, message.str());
        }

        std::string topology;
        if (!readString(root, "topology", "LUT root", &topology, error_message))
            return false;
        if (topology != TOPOLOGY_DEFT_2_5D) {
            std::ostringstream message;
            message << "VL LUT topology " << topology
                    << " does not match " << TOPOLOGY_DEFT_2_5D;
            return fail(error_message, message.str());
        }

        if (!validateTopologySignature(root, error_message))
            return false;

        std::set<int> scenario_masks;
        if (!parseFaultScenarios(root, &scenario_masks, error_message))
            return false;

        if (!root["entries"])
            return fail(error_message, "LUT missing required field entries");
        const YAML::Node entries = root["entries"];
        if (!entries.IsSequence())
            return fail(error_message, "entries must be a sequence");

        for (YAML::const_iterator it = entries.begin();
             it != entries.end();
             ++it) {
            Entry entry;
            if (!parseEntry(*it, scenario_masks, &entry, error_message))
                return false;

            LookupKey key;
            key.fault_mask = entry.fault_mask;
            key.source_chiplet_id = entry.source_chiplet_id;
            key.source_router_id = entry.source_router_id;
            key.destination_chiplet_id = entry.destination_chiplet_id;

            if (!loaded_entries.insert(std::make_pair(key, entry)).second) {
                std::ostringstream message;
                message << "duplicate LUT entry for "
                        << formatFaultMask(key.fault_mask)
                        << " source_chiplet_id=" << key.source_chiplet_id
                        << " source_router_id=" << key.source_router_id
                        << " destination_chiplet_id="
                        << key.destination_chiplet_id;
                return fail(error_message, message.str());
            }
        }

        if (loaded_entries.empty())
            return fail(error_message, "VL LUT contains no entries");

        loaded_filename = filename;
        loaded = true;
        if (error_message != 0)
            error_message->clear();
        return true;
    } catch (const YAML::Exception &e) {
        std::ostringstream message;
        message << "failed to load VL LUT " << filename << ": " << e.what();
        return fail(error_message, message.str());
    }
}

bool isLoaded()
{
    return loaded;
}

std::string loadedFilename()
{
    return loaded_filename;
}

std::size_t entryCount()
{
    return loaded_entries.size();
}

std::size_t activeFaultMaskEntryCount()
{
    const int mask = currentFaultMask();
    std::size_t count = 0;

    for (EntryMap::const_iterator it = loaded_entries.begin();
         it != loaded_entries.end();
         ++it) {
        if (it->first.fault_mask == mask)
            count++;
    }

    return count;
}

int currentFaultMask()
{
    int mask = 0;
    const std::vector<DeftTopology::VerticalLinkInfo> &links =
        DeftTopology::verticalLinks();

    for (std::vector<DeftTopology::VerticalLinkInfo>::const_iterator it =
             links.begin();
         it != links.end();
         ++it) {
        if (!it->is_functional)
            mask |= (1 << it->vl_id);
    }

    return mask;
}

std::string currentFaultMaskId()
{
    return formatFaultMask(currentFaultMask());
}

std::string formatFaultMask(int fault_mask)
{
    std::ostringstream out;
    out << "0x"
        << std::hex
        << std::nouppercase
        << std::setw(4)
        << std::setfill('0')
        << (fault_mask & 0xffff);
    return out.str();
}

bool lookup(int source_chiplet_id,
            int source_router_id,
            int destination_chiplet_id,
            Entry *entry,
            std::string *error_message)
{
    if (!loaded)
        return fail(error_message, "VL LUT is not loaded");

    LookupKey key;
    key.fault_mask = currentFaultMask();
    key.source_chiplet_id = source_chiplet_id;
    key.source_router_id = source_router_id;
    key.destination_chiplet_id = destination_chiplet_id;

    EntryMap::const_iterator found = loaded_entries.find(key);
    if (found == loaded_entries.end()) {
        std::ostringstream message;
        message << "missing VL LUT entry for fault_mask_id="
                << formatFaultMask(key.fault_mask)
                << " source_chiplet_id=" << key.source_chiplet_id
                << " source_router_id=" << key.source_router_id
                << " destination_chiplet_id=" << key.destination_chiplet_id;
        return fail(error_message, message.str());
    }

    if (!selectedLinkIsCurrentlyFunctional(found->second.source_exit,
                                           source_chiplet_id,
                                           "source_exit",
                                           error_message) ||
        !selectedLinkIsCurrentlyFunctional(found->second.destination_entry,
                                           destination_chiplet_id,
                                           "destination_entry",
                                           error_message))
        return false;

    if (entry != 0)
        *entry = found->second;

    if (error_message != 0)
        error_message->clear();
    return true;
}

} // namespace DeftVerticalLinkLut
