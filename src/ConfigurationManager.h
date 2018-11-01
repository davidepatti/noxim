/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
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
#include <vector>
#include <string>
#include <utility>

using namespace std;

void configure(int arg_num, char *arg_vet[]);

template <typename T> 
T readParam(YAML::Node node, string param, T default_value);

template <typename T> 
T readParam(YAML::Node node, string param);

namespace YAML {
    template<>
    struct convert<HubConfig> {
        static Node encode(const HubConfig& hubConfig) {
            Node node;
            node["attached_nodes"] = hubConfig.attachedNodes;
            node["rx_radio_channels"] = hubConfig.rxChannels;
            node["tx_radio_channels"] = hubConfig.txChannels;
            node["to_tile_buffer_size"] = hubConfig.toTileBufferSize;
            node["from_tile_buffer_size"] = hubConfig.fromTileBufferSize;
            node["tx_buffer_size"] = hubConfig.txBufferSize;
            node["rx_buffer_size"] = hubConfig.rxBufferSize;
            return node;
        }

        static bool decode(const Node& node, HubConfig& hubConfig) {
            hubConfig.attachedNodes = node["attached_nodes"].as<vector<int> >(GlobalParams::default_hub_configuration.attachedNodes);
            hubConfig.rxChannels = node["rx_radio_channels"].as<vector<int> >(GlobalParams::default_hub_configuration.rxChannels);
            hubConfig.txChannels = node["tx_radio_channels"].as<vector<int> >(GlobalParams::default_hub_configuration.txChannels);
            hubConfig.toTileBufferSize = node["to_tile_buffer_size"].as<int>(GlobalParams::default_hub_configuration.toTileBufferSize);
            hubConfig.fromTileBufferSize = node["from_tile_buffer_size"].as<int>(GlobalParams::default_hub_configuration.fromTileBufferSize);
            hubConfig.txBufferSize = node["tx_buffer_size"].as<int>(GlobalParams::default_hub_configuration.txBufferSize);
            hubConfig.rxBufferSize = node["rx_buffer_size"].as<int>(GlobalParams::default_hub_configuration.rxBufferSize);
            return true;
        }
    };
    
    template<>
    struct convert<ChannelConfig> {
        static Node encode(const ChannelConfig& channelConfig) {
            Node node;
            node["ber"] = channelConfig.ber;
            node["data_rate"] = channelConfig.dataRate;
            node["mac_policy"] = channelConfig.macPolicy;
            return node;
        }

        static bool decode(const Node& node, ChannelConfig& channelConfig) {
            channelConfig.ber = node["ber"].as<pair<double, double> >(GlobalParams::default_channel_configuration.ber);
            channelConfig.dataRate = node["data_rate"].as<int>(GlobalParams::default_channel_configuration.dataRate);
            channelConfig.macPolicy = node["mac_policy"].as<vector<string> >(GlobalParams::default_channel_configuration.macPolicy);
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
                //cout << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << " " << v[4] << " " << v[5] << endl;
                bufferPowerConfig.leakage[make_pair(v[0],v[1])] = v[2];
                bufferPowerConfig.push[make_pair(v[0],v[1])] = v[3];
                bufferPowerConfig.front[make_pair(v[0],v[1])] = v[4];
                bufferPowerConfig.pop[make_pair(v[0],v[1])] = v[5];
            }
            return true;
        }
    };   

    template<>
    struct convert<LinkBitLinePowerConfig> {
        static bool decode(const Node& node, LinkBitLinePowerConfig& linkBitLinePowerConfig) {
            for(YAML::const_iterator link_bit_line_pc_it= node.begin();
                link_bit_line_pc_it != node.end();
                ++link_bit_line_pc_it)
            {    
                vector<double> v = link_bit_line_pc_it->as<vector<double> >();
                //cout << v[0] << " " << v[1] << " " << v[2] << endl;
                linkBitLinePowerConfig[v[0]] = make_pair(v[1], v[2]);
            }
            return true;
        }
    };


    template<>
    struct convert<RouterPowerConfig> {
        static bool decode(const Node& node, RouterPowerConfig& routerPowerConfig) {

            for(YAML::const_iterator crossbar_it = node["crossbar"].begin();
                crossbar_it != node["crossbar"].end();
                ++crossbar_it)
            {    
                vector<double> v = crossbar_it->as<vector<double> >();
                //cout << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << endl;
                routerPowerConfig.crossbar_pm[make_pair(v[0], v[1])] = make_pair(v[2], v[3]);
            }

            for(YAML::const_iterator network_interface_it = node["network_interface"].begin(); 
                network_interface_it != node["network_interface"].end();
                ++network_interface_it)
            {    
                vector<double> v = network_interface_it->as<vector<double> >();
                //cout << v[0] << " " << v[1] << " " << v[2] << endl;
                routerPowerConfig.network_interface[v[0]] = make_pair(v[1],v[2]);
            }
            
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
            hubPowerConfig.transceiver_leakage = node["transceiver_leakage"].as<pair<double, double> >();
            hubPowerConfig.transceiver_biasing = node["transceiver_biasing"].as<pair<double, double> >();
            hubPowerConfig.rx_dynamic = node["rx_dynamic"].as<double>();
            hubPowerConfig.rx_snooping = node["rx_snooping"].as<double>();
            hubPowerConfig.default_tx_energy = node["default_tx_energy"].as<double>();

            for(YAML::const_iterator tx_attenuation_map_it= node["tx_attenuation_map"].begin(); 
                tx_attenuation_map_it != node["tx_attenuation_map"].end();
                ++tx_attenuation_map_it)
            {    
                vector<double> v = tx_attenuation_map_it->as<vector<double> >();
                //cout << v[0] << " " << v[1] << " " << v[2] << endl;
                hubPowerConfig.transmitter_attenuation_map[make_pair(v[0],v[1])] = v[2];
            }

            return true;
        }
    };
    
    template<>
    struct convert<PowerConfig> {
        static bool decode(const Node& node, PowerConfig& powerConfig) {
            powerConfig.bufferPowerConfig = node["Buffer"].as<BufferPowerConfig>();
            powerConfig.linkBitLinePowerConfig = node["LinkBitLine"].as<LinkBitLinePowerConfig>();
            powerConfig.routerPowerConfig = node["Router"].as<RouterPowerConfig>();
            powerConfig.hubPowerConfig = node["Hub"].as<HubPowerConfig>();
            return true;
        }
    };
}
#endif
