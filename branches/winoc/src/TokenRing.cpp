/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the processing element
 */

#include "TokenRing.h"


void TokenRing::updateTokens()
{
    if (reset.read()) {

    } else {

        for (map<int,ChannelConfig>::iterator i = GlobalParams::channel_configuration.begin();
                i!=GlobalParams::channel_configuration.end(); 
                i++)
        {
            int token_position = ch_token_position[i->first];
            if (--rings_mapping[i->first][token_position].second == 0)
            {
                rings_mapping[i->first][token_position].second =
                    GlobalParams::hub_configuration[rings_mapping[i->first][token_position].first].txChannels[i->first].maxHoldCycles;
                // number of hubs of the ring
                int num_hubs = rings_mapping[i->first].size();

                ch_token_position[i->first] = (ch_token_position[i->first]+1)%num_hubs;
                LOG << "Token of channel " << i->first << " has been assigned to hub " <<  rings_mapping[i->first][ch_token_position[i->first]].first << endl;
            }
        }
    }
}

int TokenRing::currentTokenHolder(int channel)
{
    int token_position = ch_token_position[channel];
    return rings_mapping[channel][token_position].first;

}

void TokenRing::attachHub(int channel,int hub)
{
    LOG << "Attaching Hub " << hub << " to the token ring for channel " << channel << endl;
    rings_mapping[channel].push_back(pair<int, int>(hub, GlobalParams::hub_configuration[hub].txChannels[channel].maxHoldCycles));
}

