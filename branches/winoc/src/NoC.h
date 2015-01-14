/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file represents the top-level testbench
 */

#ifndef __NOXIMNOC_H__
#define __NOXIMNOC_H__

#include <systemc.h>
#include "Tile.h"
#include "GlobalRoutingTable.h"
#include "GlobalTrafficTable.h"
#include "Hub.h"
#include "Channel.h"

using namespace std;

template <typename T>
struct sc_signal_NSWE
{
    sc_signal<T> east;
    sc_signal<T> west;
    sc_signal<T> south;
    sc_signal<T> north;
};

template <typename T>
struct sc_signal_NSWEH
{
    sc_signal<T> east;
    sc_signal<T> west;
    sc_signal<T> south;
    sc_signal<T> north;
    sc_signal<T> to_hub;
    sc_signal<T> from_hub;
};


SC_MODULE(NoC)
{
    // I/O Ports
    sc_in_clk clock;		// The input clock for the NoC
    sc_in < bool > reset;	// The reset signal for the NoC

    // Signals
    sc_signal_NSWEH<bool> **req;
    sc_signal_NSWEH<bool> **ack;
    sc_signal_NSWEH<Flit> **flit;
    sc_signal_NSWE<int> **free_slots;

    // NoP
    sc_signal_NSWE<NoP_data> **nop_data;

    // Matrix of tiles
    Tile ***t;

    map<int, Hub*> hub;
    map<int, Channel*> channel;

    // Global tables
    GlobalRoutingTable grtable;
    GlobalTrafficTable gttable;

    //---------- Mau experiment <start>
    void flitsMonitor() {

	if (!reset.read()) {
	    //      if ((int)sc_simulation_time() % 5)
	    //        return;

	    unsigned int count = 0;
	    for (int i = 0; i < GlobalParams::mesh_dim_x; i++)
		for (int j = 0; j < GlobalParams::mesh_dim_y; j++)
		    count += t[i][j]->r->getFlitsCount();
	    cout << count << endl;
	}
    }
    //---------- Mau experiment <stop>

    // Constructor

    SC_CTOR(NoC) {

	//---------- Mau experiment <start>
	/*
	   SC_METHOD(flitsMonitor);
	   sensitive(reset);
	   sensitive_pos(clock);
	 */
	//---------- Mau experiment <stop>

	// Build the Mesh
	buildMesh("config.yaml");
    }

    // Support methods
    Tile *searchNode(const int id) const;

  private:

    void buildMesh(char const * cfg_fname);
};

//Hub * dd;

#endif
