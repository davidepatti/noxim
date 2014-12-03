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

using namespace std;

SC_MODULE(Hub)
{
    SC_HAS_PROCESS(Hub);
    
    // I/O Ports
    sc_in_clk clock; // The input clock for the tile
    sc_in <bool> reset; // The reset signal for the tile

    int local_id; // Unique ID
    int num_ports;
    int num_rx_channels;
    int num_tx_channels;

    sc_in<Flit>* flit_rx;
    sc_in<bool>* req_rx;
    sc_out<bool>* ack_rx;

    sc_out<Flit>* flit_tx;
    sc_out<bool>* req_tx;	   
    sc_in<bool>* ack_tx;	  

    Buffer* buffer;	        // Buffer for each port
    bool* current_level_rx;	// Current level for ABP
    bool* current_level_tx;	// Current level for ABP

    Initiator** init;
    Target** target;

    map<int, int> tile2port_mapping;
    map<int, int> tile2hub_mapping;

    int start_from_port; // Port from which to start the reservation cycle

    ReservationTable reservation_table;	// Switch reservation table
    ReservationTable wireless_reservation_table;// Wireless reservation table

    void rxProcess(); // The receiving process
    void txProcess(); // The transmitting process
    void radioProcess(); // The radio transceiver process

    void setup();
    int route(Flit&);
    int tile2Port(int);
    int tile2Hub(int);

    // Constructor

    Hub(sc_module_name nm, int id): sc_module(nm) {
        SC_METHOD(rxProcess);
        sensitive << reset;
        sensitive << clock.pos();

        SC_METHOD(txProcess);
        sensitive << reset;
        sensitive << clock.pos();

        SC_METHOD(radioProcess);
        sensitive << reset;
        sensitive << clock.pos();

        local_id = id;
        num_ports = GlobalParams::hub_conf[local_id].attachedNodes_num;
        num_rx_channels = GlobalParams::hub_conf[local_id].rxChannels_num;
        num_tx_channels = GlobalParams::hub_conf[local_id].txChannels_num;

        flit_rx = new sc_in<Flit>[num_ports];
        req_rx = new sc_in<bool>[num_ports];
        ack_rx = new sc_out<bool>[num_ports];

        flit_tx = new sc_out<Flit>[num_ports];
        req_tx = new sc_out<bool>[num_ports];
        ack_tx = new sc_in<bool>[num_ports];

        buffer = new Buffer[num_ports];
        current_level_rx = new bool[num_ports];
        current_level_tx = new bool[num_ports];

        init = new Initiator*[num_tx_channels];
        target = new Target*[num_rx_channels];

	start_from_port = 0;

        for (int i = 0; i < num_tx_channels; i++) {
            char txt[20];
            sprintf(txt, "init_%d", i);
            init[i] = new Initiator(txt);
            // actual bus binding in buildMesh()
            //init[i]->socket.bind( bus->targ_socket );
        }

        for (int i = 0; i < num_rx_channels; i++) {
            char txt[20];
            sprintf(txt, "target_%d", i);
            target[i] = new Target(txt);
        }

        start_from_port = 0;
        reservation_table.init(num_ports);
        wireless_reservation_table.init(num_tx_channels);
    }
};

#endif
