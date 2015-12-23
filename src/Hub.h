/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the tile
 */

#ifndef __NOXIMHUB_H__
#define __NOXIMHUB_H__

#include <map>
#include <systemc.h>
#include "DataStructs.h"
#include "Buffer.h"
#include "ReservationTable.h"

#include "Initiator.h"
#include "Target.h"
#include "TokenRing.h"
#include "Power.h"

using namespace std;

SC_MODULE(Hub)
{
    SC_HAS_PROCESS(Hub);
    
    // I/O Ports
    sc_in_clk clock; // The input clock for the tile
    sc_in <bool> reset; // The reset signal for the tile

    int local_id; // Unique ID
    TokenRing* token_ring;
    int num_ports;
    vector<int> attachedNodes;
    vector<int> txChannels;
    vector<int> rxChannels;

    sc_in<Flit>* flit_rx;
    sc_in<bool>* req_rx;
    sc_out<bool>* ack_rx;

    sc_out<Flit>* flit_tx;
    sc_out<bool>* req_tx;	   
    sc_in<bool>* ack_tx;	  

    Buffer* buffer_from_tile;   // Buffer for each port
    Buffer* buffer_to_tile;     // Buffer for each port
    bool* current_level_rx;	// Current level for ABP
    bool* current_level_tx;	// Current level for ABP


    map<int, sc_in<int>* > current_token_holder;
    map<int, sc_in<int>* > current_token_expiration;
    map<int, sc_out<int>* > flag;


    map<int, Initiator*> init;
    map<int, Target*> target;

    map<int, int> tile2port_mapping;
    map<int, int> tile2hub_mapping;

    int start_from_port; // Port from which to start the reservation cycle

    ReservationTable antenna2tile_reservation_table;	// Switch reservation table
    ReservationTable tile2antenna_reservation_table;// Wireless reservation table

    void updateRxPower();
    void updateTxPower();
    void antennaToTileProcess();
    void tileToAntennaProcess();

    int route(Flit&);
    int tile2Port(int);

    void setFlitTransmissionCycles(int cycles,int ch_id) {flit_transmission_cycles[ch_id]=cycles;}

    // Power stats
    Power power;

    int total_sleep_cycles;
    int total_ttxoff_cycles;
    map<int,int> buffer_rx_sleep_cycles; // antenna buffer RX power off cycles
    map<int,int> abtxoff_cycles; // antenna buffer TX power off cycles
    map<int,int> buffer_to_tile_poweroff_cycles;

    // Constructor

    Hub(sc_module_name nm, int id, TokenRing * tr): sc_module(nm) {

	if (GlobalParams::use_winoc)
	{
	    SC_METHOD(tileToAntennaProcess);
	    sensitive << reset;
	    sensitive << clock.pos();

	    SC_METHOD(antennaToTileProcess);
	    sensitive << reset;
	    sensitive << clock.pos();

	}


        local_id = id;
	token_ring = tr;
        num_ports = GlobalParams::hub_configuration[local_id].attachedNodes.size();
        attachedNodes = GlobalParams::hub_configuration[local_id].attachedNodes;
        rxChannels = GlobalParams::hub_configuration[local_id].rxChannels;
        txChannels = GlobalParams::hub_configuration[local_id].txChannels;

        flit_rx = new sc_in<Flit>[num_ports];
        req_rx = new sc_in<bool>[num_ports];
        ack_rx = new sc_out<bool>[num_ports];

        flit_tx = new sc_out<Flit>[num_ports];
        req_tx = new sc_out<bool>[num_ports];
        ack_tx = new sc_in<bool>[num_ports];

        buffer_from_tile = new Buffer[num_ports];
        buffer_to_tile = new Buffer[num_ports];
        
        for(int i = 0; i < num_ports; i++)
        {
            buffer_from_tile[i].SetMaxBufferSize(GlobalParams::hub_configuration[local_id].fromTileBufferSize);
            buffer_to_tile[i].SetMaxBufferSize(GlobalParams::hub_configuration[local_id].toTileBufferSize);
        }

        current_level_rx = new bool[num_ports];
        current_level_tx = new bool[num_ports];

        start_from_port = 0;

        for (unsigned int i = 0; i < txChannels.size(); i++) {
            char txt[20];
            sprintf(txt, "init_%d", txChannels[i]);
            init[txChannels[i]] = new Initiator(txt,this);
            init[txChannels[i]]->buffer_tx.SetMaxBufferSize(GlobalParams::hub_configuration[local_id].txBufferSize);
	    current_token_holder[txChannels[i]] = new sc_in<int>();
	    current_token_expiration[txChannels[i]] = new sc_in<int>();
	    flag[txChannels[i]] = new sc_out<int>();
            token_ring->attachHub(txChannels[i],local_id, current_token_holder[txChannels[i]],current_token_expiration[txChannels[i]],flag[txChannels[i]]);
	    // wirxsleep currently assumes TOKEN_PACKET mac policy
	    if (GlobalParams::use_wirxsleep)
		assert(token_ring->getPolicy(txChannels[i]).first==TOKEN_PACKET);
        }

        for (unsigned int i = 0; i < rxChannels.size(); i++) {
            char txt[20];
            sprintf(txt, "target_%d", rxChannels[i]);
            target[rxChannels[i]] = new Target(txt, rxChannels[i], this);
            target[rxChannels[i]]->buffer_rx.SetMaxBufferSize(GlobalParams::hub_configuration[local_id].rxBufferSize);
        }

        start_from_port = 0;
	total_sleep_cycles = 0;
	total_ttxoff_cycles = 0;
    }


    int getID() { return local_id;}

    private:
    map<int,int> flit_transmission_cycles;

    void txRadioProcessTokenPacket(int channel);
    void txRadioProcessTokenHold(int channel);
    void txRadioProcessTokenMaxHold(int channel);

    void wirxPowerManager();
    void txPowerManager();
};

#endif
