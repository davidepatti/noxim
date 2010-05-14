/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the processing element
 */

#ifndef __NOXIMPROCESSINGELEMENT_H__
#define __NOXIMPROCESSINGELEMENT_H__

#include <queue>
#include <systemc.h>
#include "NoximMain.h"
#include "NoximGlobalTrafficTable.h"
using namespace std;

SC_MODULE(NoximProcessingElement)
{

    // I/O Ports
    sc_in_clk clock;		// The input clock for the PE
    sc_in < bool > reset;	// The reset signal for the PE

    sc_in < NoximFlit > flit_rx;	// The input channel
    sc_in < bool > req_rx;	// The request associated with the input channel
    sc_out < bool > ack_rx;	// The outgoing ack signal associated with the input channel

    sc_out < NoximFlit > flit_tx;	// The output channel
    sc_out < bool > req_tx;	// The request associated with the output channel
    sc_in < bool > ack_tx;	// The outgoing ack signal associated with the output channel

    sc_in < int >free_slots_neighbor;

    // Registers
    int local_id;		// Unique identification number
    bool current_level_rx;	// Current level for Alternating Bit Protocol (ABP)
    bool current_level_tx;	// Current level for Alternating Bit Protocol (ABP)
    queue < NoximPacket > packet_queue;	// Local queue of packets
    bool transmittedAtPreviousCycle;	// Used for distributions with memory

    // Functions
    void rxProcess();		// The receiving process
    void txProcess();		// The transmitting process
    bool canShot(NoximPacket & packet);	// True when the packet must be shot
    NoximFlit nextFlit();	// Take the next flit of the current packet
    NoximPacket trafficRandom();	// Random destination distribution
    NoximPacket trafficTranspose1();	// Transpose 1 destination distribution
    NoximPacket trafficTranspose2();	// Transpose 2 destination distribution
    NoximPacket trafficBitReversal();	// Bit-reversal destination distribution
    NoximPacket trafficShuffle();	// Shuffle destination distribution
    NoximPacket trafficButterfly();	// Butterfly destination distribution


    NoximGlobalTrafficTable *traffic_table;	// Reference to the Global traffic Table
    bool never_transmit;	// true if the PE does not transmit any packet 
    //  (valid only for the table based traffic)

    void fixRanges(const NoximCoord, NoximCoord &);	// Fix the ranges of the destination
    int randInt(int min, int max);	// Extracts a random integer number between min and max
    int getRandomSize();	// Returns a random size in flits for the packet
    void setBit(int &x, int w, int v);
    int getBit(int x, int w);
    double log2ceil(double x);

    // Constructor
    SC_CTOR(NoximProcessingElement) {
	SC_METHOD(rxProcess);
	sensitive << reset;
	sensitive << clock.pos();

	SC_METHOD(txProcess);
	sensitive << reset;
	sensitive << clock.pos();
    }

};

#endif
