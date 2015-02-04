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

    map<int, Initiator*> init;
    map<int, Target*> target;

    map<int, int> tile2port_mapping;
    map<int, int> tile2hub_mapping;

    int start_from_port; // Port from which to start the reservation cycle

    ReservationTable reservation_table;	// Switch reservation table
    ReservationTable in_reservation_table;	// Switch reservation table
    ReservationTable wireless_reservation_table;// Wireless reservation table

    void rxProcess(); // The receiving process
    void txProcess(); // The transmitting process
    void rxRadioProcess(); // The radio transceiver process
    void txRadioProcess(); // The radio transceiver process

    int route(Flit&);
    int tile2Port(int);

    // Constructor

    Hub(sc_module_name nm, int id, TokenRing * tr): sc_module(nm) {
        SC_METHOD(rxProcess);
        sensitive << reset;
        sensitive << clock.pos();

        SC_METHOD(txProcess);
        sensitive << reset;
        sensitive << clock.pos();

        SC_METHOD(rxRadioProcess);
        sensitive << reset;
        sensitive << clock.pos();

        SC_METHOD(txRadioProcess);
        sensitive << reset;
        sensitive << clock.pos();

        local_id = id;
	token_ring = tr;
        num_ports = GlobalParams::hub_configuration[local_id].attachedNodes.size();
        attachedNodes = GlobalParams::hub_configuration[local_id].attachedNodes;

        for(map<int, TxChannelConfig>::iterator it = GlobalParams::hub_configuration[local_id].txChannels.begin();
                it != GlobalParams::hub_configuration[local_id].txChannels.end(); ++it) {
            txChannels.push_back(it->first);
        }
        rxChannels = GlobalParams::hub_configuration[local_id].rxChannels;

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
        LOG << "Size of buffers for data from tiles = " << buffer_from_tile[0].GetMaxBufferSize() << endl; 
        LOG << "size of buffer for data to tiles = " << buffer_to_tile[0].GetMaxBufferSize() << endl; 

        current_level_rx = new bool[num_ports];
        current_level_tx = new bool[num_ports];

        start_from_port = 0;

        for (unsigned int i = 0; i < txChannels.size(); i++) {
            char txt[20];
            sprintf(txt, "init_%d", txChannels[i]);
            init[txChannels[i]] = new Initiator(txt);
            init[txChannels[i]]->buffer_tx.SetMaxBufferSize(GlobalParams::hub_configuration[local_id].txBufferSize);
            LOG << "Size of buffer_tx = " << init[txChannels[i]]->buffer_tx.GetMaxBufferSize() << " for Channel_"<< txChannels[i] << endl;
            token_ring->attachHub(txChannels[i],local_id);
        }

        for (unsigned int i = 0; i < rxChannels.size(); i++) {
            char txt[20];
            sprintf(txt, "target_%d", rxChannels[i]);
            target[rxChannels[i]] = new Target(txt, rxChannels[i], this);
            target[rxChannels[i]]->buffer_rx.SetMaxBufferSize(GlobalParams::hub_configuration[local_id].rxBufferSize);
            LOG << "Size of buffer_rx = " << target[rxChannels[i]]->buffer_rx.GetMaxBufferSize() << " for Channel_"<< rxChannels[i] << endl;
        }

        start_from_port = 0;
    }
};

#endif
