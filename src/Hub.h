/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
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
    sc_out<TBufferFullStatus>* buffer_full_status_rx;

    sc_out<Flit>* flit_tx;
    sc_out<bool>* req_tx;	   
    sc_in<bool>* ack_tx;	  
    sc_in<TBufferFullStatus>* buffer_full_status_tx;

    BufferBank* buffer_from_tile;   // Buffer for each port
    BufferBank* buffer_to_tile;     // Buffer for each port
    bool* current_level_rx;	// Current level for ABP
    bool* current_level_tx;	// Current level for ABP


    map<int, sc_in<int>* > current_token_holder;
    map<int, sc_in<int>* > current_token_expiration;
    map<int, sc_inout<int>* > flag;
    bool * transmission_in_progress;

    map<int, Initiator*> init;
    map<int, Target*> target;

    map<int, int> tile2port_mapping;
    map<int, int> tile2hub_mapping;

    int start_from_port; // Port from which to start the reservation cycle
    int * start_from_vc; // VC from which to start the reservation cycle for the specific port

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
    map<int,int> analogtxoff_cycles; // analog TX power off cycles
    map<int,int> buffer_to_tile_poweroff_cycles;

    int wireless_communications_counter;

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

	antenna2tile_reservation_table.setSize(num_ports);
	// fix this
	//tile2antenna_reservation_table.setSize(txChannels.size());
#define STATIC_MAX_CHANNELS 100
      tile2antenna_reservation_table.setSize(STATIC_MAX_CHANNELS);

        flit_rx = new sc_in<Flit>[num_ports];
        req_rx = new sc_in<bool>[num_ports];
        ack_rx = new sc_out<bool>[num_ports];
        buffer_full_status_rx = new sc_out<TBufferFullStatus>[num_ports];

        flit_tx = new sc_out<Flit>[num_ports];
        req_tx = new sc_out<bool>[num_ports];
        ack_tx = new sc_in<bool>[num_ports];
        buffer_full_status_tx = new sc_in<TBufferFullStatus>[num_ports];

        buffer_from_tile = new BufferBank[num_ports];
        buffer_to_tile = new BufferBank[num_ports];
        
	start_from_vc = new int[num_ports];
	transmission_in_progress = new bool[num_ports];

        current_level_rx = new bool[num_ports];
        current_level_tx = new bool[num_ports];

        start_from_port = 0;

        for(int i = 0; i < num_ports; i++)
        {
            transmission_in_progress[i] = false;
            for (int vc = 0;vc<GlobalParams::n_virtual_channels; vc++)
            {
                buffer_from_tile[i][vc].SetMaxBufferSize(GlobalParams::hub_configuration[local_id].fromTileBufferSize);
                buffer_to_tile[i][vc].SetMaxBufferSize(GlobalParams::hub_configuration[local_id].toTileBufferSize);
                buffer_from_tile[i][vc].setLabel(string(name())+"->bft["+i_to_string(i)+"]["+i_to_string(vc)+"]");
                buffer_to_tile[i][vc].setLabel(string(name())+"->btt["+i_to_string(i)+"]["+i_to_string(vc)+"]");
            }
            start_from_vc[i] = 0;
        }

        for (unsigned int i = 0; i < txChannels.size(); i++) {
            char txt[20];
            int ch = txChannels[i];
            sprintf(txt, "init_%d", ch);
            init[ch] = new Initiator(txt,this);
            init[ch]->buffer_tx.SetMaxBufferSize(GlobalParams::hub_configuration[local_id].txBufferSize);
            init[ch]->buffer_tx.setLabel(string(name())+"->abtx["+i_to_string(i)+"]");
            current_token_holder[ch] = new sc_in<int>();
            current_token_expiration[ch] = new sc_in<int>();
            flag[ch] = new sc_inout<int>();
            token_ring->attachHub(ch,local_id, current_token_holder[ch],current_token_expiration[ch],flag[ch]);
            // power manager currently assumes TOKEN_PACKET mac policy
            if (GlobalParams::use_powermanager)
                assert(token_ring->getPolicy(ch).first==TOKEN_PACKET);
        }

        for (unsigned int i = 0; i < rxChannels.size(); i++) {
            char txt[20];
            sprintf(txt, "target_%d", rxChannels[i]);
            target[rxChannels[i]] = new Target(txt, rxChannels[i], this);
            target[rxChannels[i]]->buffer_rx.SetMaxBufferSize(GlobalParams::hub_configuration[local_id].rxBufferSize);
            target[rxChannels[i]]->buffer_rx.setLabel(string(name())+"->abrx["+i_to_string(i)+"]");
        }

	start_from_port = 0;
	total_sleep_cycles = 0;
	total_ttxoff_cycles = 0;
	wireless_communications_counter = 0;
    }


    int getID() { return local_id;}

    private:
    map<int,int> flit_transmission_cycles;

    void txRadioProcessTokenPacket(int channel);
    void txRadioProcessTokenHold(int channel);
    void txRadioProcessTokenMaxHold(int channel);

    void rxPowerManager();
    void txPowerManager();

    int selectChannel(int src, int dst) const ;
};

#endif
