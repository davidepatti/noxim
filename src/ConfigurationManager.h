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
            hubConfig.txChannels = GlobalParams::default_hub_configuration.txChannels;
            
            for(YAML::const_iterator it = node["txChannels"].begin(); 
                    it != node["txChannels"].end();
                    ++it)
            { 
                if (it == node["txChannels"].begin())
                    hubConfig.txChannels.clear();

                int txChannel_id = it->first.as<int>(-1);
                if (txChannel_id < 0)
                    continue;
                
                YAML::Node txChannel_config_node = it->second;

                hubConfig.txChannels[txChannel_id] = txChannel_config_node.as<TxChannelConfig>
                    (GlobalParams::default_hub_configuration.txChannels[txChannel_id]);
            }

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

    template<>
    struct convert<TxChannelConfig> {
        static Node encode(const TxChannelConfig& txChannelConfig) {
            Node node;
            node["max_hold_cycles"] = txChannelConfig.maxHoldCycles;

            return node;
        }

        static bool decode(const Node& node, TxChannelConfig& txChannelConfig) {
            txChannelConfig.maxHoldCycles = node["max_hold_cycles"].as<int>();
            return true;
        }
    };
    
    template<>
    struct convert<BufferPowerConfig> {
        static bool decode(const Node& node, BufferPowerConfig& bufferPowerConfig) {
            for(YAML::const_iterator buffering_it= node.begin(); 
                buffering_it != node.end();
                ++buffering_it)
            {    
                vector<double> v = buffering_it->as<vector<double> >();
                cout << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << " " << v[4] << " " << v[5] << endl;
                bufferPowerConfig.leakage_pm[make_pair(v[0],v[1])] = v[2];
                bufferPowerConfig.push_pm[make_pair(v[0],v[1])] = v[3];
                bufferPowerConfig.front_pm[make_pair(v[0],v[1])] = v[4];
                bufferPowerConfig.pop_pm[make_pair(v[0],v[1])] = v[5];
            }
            return true;
        }
    };


    template<>
    struct convert<RouterPowerConfig> {
        static bool decode(const Node& node, RouterPowerConfig& routerPowerConfig) {
            routerPowerConfig.link_bit_line_pm = node["link_bit_line"].as<pair<double, double> >();
            routerPowerConfig.crossbar_pm = node["crossbar"].as<pair<double, double> >();
            routerPowerConfig.network_interface_pm = node["network_interface"].as<pair<double, double> >();
/*            
            for(YAML::const_iterator buffering_it= node["buffering"].begin(); 
                buffering_it != node["buffering"].end();
                ++buffering_it)
            {    
                vector<double> v = buffering_it->as<vector<double> >();
                //cout << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << " " << v[4] << " " << v[5] << endl;
                routerPowerConfig.buffer_leakage_pm[make_pair(v[0],v[1])] = v[2];
                routerPowerConfig.buffer_push_pm[make_pair(v[0],v[1])] = v[3];
                routerPowerConfig.buffer_front_pm[make_pair(v[0],v[1])] = v[4];
                routerPowerConfig.buffer_pop_pm[make_pair(v[0],v[1])] = v[5];
            }
*/
            for(YAML::const_iterator routing_it = node["routing"].begin(); 
                routing_it != node["routing"].end();
                ++routing_it)
            {    
                routerPowerConfig.routing_algorithm_pm[routing_it->first.as<string>()] = routing_it->second.as<pair<double, double> >();
            }

            for(YAML::const_iterator selection_it= node["selection"].begin(); 
                selection_it != node["selection"].end();
                ++selection_it)
            {    
                routerPowerConfig.selection_strategy_pm[selection_it->first.as<string>()] = selection_it->second.as<pair<double, double> >();
            }

            return true;
        }
    };
    
    template<>
    struct convert<HubPowerConfig> {
        static bool decode(const Node& node, HubPowerConfig& hubPowerConfig) {
            hubPowerConfig.link_bit_line_pm = node["link_bit_line"].as<pair<double, double> >();
            hubPowerConfig.transceiver_leakage_pm = node["transceiver_leakage"].as<double>();
            hubPowerConfig.receiver_pm = node["rx_power"].as<double>();
            hubPowerConfig.default_transmitter_pm = node["default_tx_power"].as<double>();

            for(YAML::const_iterator tx_power_map_it= node["tx_power_map"].begin(); 
                tx_power_map_it != node["tx_power_map"].end();
                ++tx_power_map_it)
            {    
                vector<double> v = tx_power_map_it->as<vector<double> >();
                //cout << v[0] << " " << v[1] << " " << v[2] << endl;
                hubPowerConfig.transmitter_pm[make_pair(v[0],v[1])] = v[2];
            }

            return true;
        }
    };
    
    template<>
    struct convert<PowerConfig> {
        static bool decode(const Node& node, PowerConfig& powerConfig) {
            powerConfig.bufferPowerConfig = node["Buffer"].as<BufferPowerConfig>();
            powerConfig.routerPowerConfig = node["Router"].as<RouterPowerConfig>();
            powerConfig.hubPowerConfig = node["Hub"].as<HubPowerConfig>();
            return true;
        }
    };
}
#endif
