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
            node["toTileBufferSize"] = hubConfig.toTileBufferSize;
            node["fromTileBufferSize"] = hubConfig.fromTileBufferSize;
            node["txBufferSize"] = hubConfig.txBufferSize;
            node["rxBufferSize"] = hubConfig.rxBufferSize;
            return node;
        }

        static bool decode(const Node& node, HubConfig& hubConfig) {
            hubConfig.attachedNodes = node["attachedNodes"].as<std::vector<int> >(GlobalParams::default_hub_configuration.attachedNodes);
            hubConfig.txChannels = node["txChannels"].as<std::vector<int> >(GlobalParams::default_hub_configuration.txChannels);
            hubConfig.rxChannels = node["rxChannels"].as<std::vector<int> >(GlobalParams::default_hub_configuration.rxChannels);
            hubConfig.toTileBufferSize = node["toTileBufferSize"].as<int>(GlobalParams::default_hub_configuration.toTileBufferSize);
            hubConfig.fromTileBufferSize = node["fromTileBufferSize"].as<int>(GlobalParams::default_hub_configuration.fromTileBufferSize);
            hubConfig.txBufferSize = node["txBufferSize"].as<int>(GlobalParams::default_hub_configuration.txBufferSize);
            hubConfig.rxBufferSize = node["rxBufferSize"].as<int>(GlobalParams::default_hub_configuration.rxBufferSize);
            return true;
        }
    };
    
    template<>
    struct convert<ChannelConfig> {
        static Node encode(const ChannelConfig& channelConfig) {
            Node node;
            node["ber"] = channelConfig.ber;
            node["dataRate"] = channelConfig.dataRate;
            return node;
        }

        static bool decode(const Node& node, ChannelConfig& channelConfig) {
            channelConfig.ber = node["ber"].as<pair<int, int> >(GlobalParams::default_channel_configuration.ber);
            channelConfig.dataRate = node["dataRate"].as<int>(GlobalParams::default_channel_configuration.dataRate);
            return true;
        }
    };

}
#endif
