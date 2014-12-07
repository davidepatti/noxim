/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the global params needed by Noxim
 * to forward configuration to every sub-block
 */

#ifndef __NOXIMCONFIGURATIONMANAGER_H__
#define __NOXIMCONFIGURATIONMANAGER_H__

#include "yaml-cpp/yaml.h"
#include "GlobalParams.h"

#include <iostream>
#include <utility>

using namespace std;

void configure(int arg_num, char *arg_vet[]);

namespace YAML {
    template<>
    struct convert<HubConfig> {
        static Node encode(const HubConfig& hubConfig) {
            Node node;
            node["attachedNodes"] = hubConfig.attachedNodes;
            node["txChannels"] = hubConfig.txChannels;
            node["rxChannels"] = hubConfig.rxChannels;
            return node;
        }

        static bool decode(const Node& node, HubConfig& hubConfig) {
            hubConfig.attachedNodes = node["attachedNodes"].as<std::vector<int> >();
            hubConfig.txChannels = node["txChannels"].as<std::vector<int> >();
            hubConfig.rxChannels = node["rxChannels"].as<std::vector<int> >();
            return true;
        }
    };
    
    template<>
    struct convert<ChannelConfig> {
        static Node encode(const ChannelConfig& channelConfig) {
            Node node;
            node["ber"] = channelConfig.ber;
            node["delay"] = channelConfig.delay;
            return node;
        }

        static bool decode(const Node& node, ChannelConfig& channelConfig) {
            channelConfig.ber = node["ber"].as<pair<int, int> >();
            channelConfig.delay = node["delay"].as<pair<int, int> >();
            return true;
        }
    };

}
#endif
