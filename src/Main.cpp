/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the top-level of Noxim
 */

#include "ConfigurationManager.h"
#include "NoC.h"
#include "GlobalStats.h"
#include "DataStructs.h"
#include "DeftTopology.h"
#include "GlobalParams.h"
#include "Logger.h"

#include <csignal>
#include <set>
#include <sstream>

using namespace std;

// need to be globally visible to allow "-volume" simulation stop
unsigned int drained_volume;
NoC *n;

namespace {

int getDeltaStages()
{
    int stages = 0;
    int tiles = GlobalParams::n_delta_tiles;
    while (tiles > 1) {
        tiles /= 2;
        stages++;
    }
    return stages;
}

int getSwitchDimX()
{
    if (GlobalParams::topology == TOPOLOGY_MESH)
        return GlobalParams::mesh_dim_x + 1;
    if (GlobalParams::topology == TOPOLOGY_DEFT_2_5D)
        return DeftTopology::LayoutWidth;

    return getDeltaStages();
}

int getSwitchDimY()
{
    if (GlobalParams::topology == TOPOLOGY_MESH)
        return GlobalParams::mesh_dim_y + 1;
    if (GlobalParams::topology == TOPOLOGY_DEFT_2_5D)
        return DeftTopology::LayoutHeight;

    return GlobalParams::n_delta_tiles / 2;
}

int getCoreCount()
{
    if (GlobalParams::topology == TOPOLOGY_MESH)
        return GlobalParams::mesh_dim_x * GlobalParams::mesh_dim_y;
    if (GlobalParams::topology == TOPOLOGY_DEFT_2_5D)
        return DeftTopology::TotalRouters;

    return GlobalParams::n_delta_tiles;
}

set<string> parseTraceScopes(const string &trace_scope)
{
    set<string> scopes;
    stringstream ss(trace_scope);
    string token;

    while (getline(ss, token, ',')) {
        if (!token.empty())
            scopes.insert(token);
    }

    if (scopes.empty())
        scopes.insert("basic");

    return scopes;
}

bool traceScopeEnabled(const set<string> &scopes, const string &scope)
{
    return scopes.count("all") != 0 || scopes.count(scope) != 0;
}

template <typename T>
void traceNamed(sc_trace_file *tf, const sc_signal<T> &signal, const string &name)
{
    string label = name;
    const sc_signal_in_if<T> &signal_if = signal;
    sc_trace(tf, signal_if, label);
}

template <typename T>
void traceCardinalSignals(sc_trace_file *tf, sc_signal_NSWEH<T> &signals, const string &prefix)
{
    traceNamed(tf, signals.east, prefix + ".east");
    traceNamed(tf, signals.west, prefix + ".west");
    traceNamed(tf, signals.south, prefix + ".south");
    traceNamed(tf, signals.north, prefix + ".north");
}

template <typename T>
void traceHubSignals(sc_trace_file *tf, sc_signal_NSWEH<T> &signals, const string &prefix)
{
    traceNamed(tf, signals.to_hub, prefix + ".to_hub");
    traceNamed(tf, signals.from_hub, prefix + ".from_hub");
}

template <typename T>
void traceCardinalSignals(sc_trace_file *tf, sc_signal_NSWE<T> &signals, const string &prefix)
{
    traceNamed(tf, signals.east, prefix + ".east");
    traceNamed(tf, signals.west, prefix + ".west");
    traceNamed(tf, signals.south, prefix + ".south");
    traceNamed(tf, signals.north, prefix + ".north");
}

string matrixLabel(const string &prefix, int x, int y)
{
    ostringstream out;
    out << prefix << "(" << x << ")(" << y << ")";
    return out.str();
}

string directionLabel(int direction)
{
    switch (direction) {
    case DIRECTION_NORTH:
        return "north";
    case DIRECTION_EAST:
        return "east";
    case DIRECTION_SOUTH:
        return "south";
    case DIRECTION_WEST:
        return "west";
    default:
        return "unknown";
    }
}

string indexedLabel(const string &prefix, int index)
{
    ostringstream out;
    out << prefix << "(" << index << ")";
    return out.str();
}

void traceBasicSignals(sc_trace_file *tf, NoC *noc, int dim_x, int dim_y)
{
    for (int i = 0; i < dim_x; i++) {
        for (int j = 0; j < dim_y; j++) {
            traceCardinalSignals(tf, noc->req[i][j], matrixLabel("req", i, j));
            traceCardinalSignals(tf, noc->ack[i][j], matrixLabel("ack", i, j));
        }
    }
}

void traceRouterSignals(sc_trace_file *tf, NoC *noc, int dim_x, int dim_y)
{
    for (int i = 0; i < dim_x; i++) {
        for (int j = 0; j < dim_y; j++) {
            traceCardinalSignals(tf, noc->flit[i][j], matrixLabel("flit", i, j));
            traceCardinalSignals(tf, noc->nop_data[i][j], matrixLabel("nop", i, j));
        }
    }
}

void traceBufferSignals(sc_trace_file *tf, NoC *noc, int dim_x, int dim_y)
{
    for (int i = 0; i < dim_x; i++) {
        for (int j = 0; j < dim_y; j++) {
            traceCardinalSignals(tf, noc->buffer_full_status[i][j], matrixLabel("buffer_full_status", i, j));
            traceCardinalSignals(tf, noc->free_slots[i][j], matrixLabel("free_slots", i, j));
        }
    }

    if (GlobalParams::topology != TOPOLOGY_MESH)
        return;

    for (int i = 0; i < GlobalParams::mesh_dim_x; i++) {
        for (int j = 0; j < GlobalParams::mesh_dim_y; j++) {
            Router *router = noc->t[i][j]->r;
            const string prefix = matrixLabel("router_buffer", i, j);

            for (int direction = 0; direction < DIRECTIONS; direction++) {
                const string direction_prefix = prefix + "." + directionLabel(direction);
                for (int vc = 0; vc < GlobalParams::n_virtual_channels; vc++) {
                    const string vc_prefix = direction_prefix + ".vc" + to_string(vc);
                    traceNamed(tf,
                               router->getTracedBufferOccupancy(direction, vc),
                               vc_prefix + ".occupancy");

                    for (int slot = 0; slot < router->getTracedBufferDepth(); slot++) {
                        traceNamed(tf,
                                   router->getTracedBufferSlot(direction, vc, slot),
                                   vc_prefix + ".slot" + to_string(slot));
                    }
                }
            }
        }
    }
}

void traceWirelessSignals(sc_trace_file *tf, NoC *noc, int dim_x, int dim_y)
{
    for (int i = 0; i < dim_x; i++) {
        for (int j = 0; j < dim_y; j++) {
            traceHubSignals(tf, noc->req[i][j], matrixLabel("req", i, j));
            traceHubSignals(tf, noc->ack[i][j], matrixLabel("ack", i, j));
            traceHubSignals(tf, noc->flit[i][j], matrixLabel("flit", i, j));
            traceHubSignals(tf, noc->buffer_full_status[i][j], matrixLabel("buffer_full_status", i, j));
        }
    }

    if (GlobalParams::topology != TOPOLOGY_MESH) {
        const int cores = getCoreCount();
        for (int core = 0; core < cores; core++) {
            traceNamed(tf, noc->req_from_hub[core], indexedLabel("req_from_hub", core));
            traceNamed(tf, noc->req_to_hub[core], indexedLabel("req_to_hub", core));
            traceNamed(tf, noc->ack_from_hub[core], indexedLabel("ack_from_hub", core));
            traceNamed(tf, noc->ack_to_hub[core], indexedLabel("ack_to_hub", core));
            traceNamed(tf, noc->flit_from_hub[core], indexedLabel("flit_from_hub", core));
            traceNamed(tf, noc->flit_to_hub[core], indexedLabel("flit_to_hub", core));
            traceNamed(tf, noc->buffer_full_status_from_hub[core], indexedLabel("buffer_full_status_from_hub", core));
            traceNamed(tf, noc->buffer_full_status_to_hub[core], indexedLabel("buffer_full_status_to_hub", core));
        }
    }

    if (!GlobalParams::use_winoc || noc->token_ring == NULL)
        return;

    for (map<int, sc_signal<int>* >::const_iterator it = noc->token_ring->token_holder_signals.begin();
         it != noc->token_ring->token_holder_signals.end();
         ++it) {
        traceNamed(tf, *(it->second), "tokenring.channel_" + to_string(it->first) + ".holder");
    }

    for (map<int, sc_signal<int>* >::const_iterator it = noc->token_ring->token_expiration_signals.begin();
         it != noc->token_ring->token_expiration_signals.end();
         ++it) {
        traceNamed(tf, *(it->second), "tokenring.channel_" + to_string(it->first) + ".expiration");
    }

    for (map<int, map<int, sc_signal<int>* > >::const_iterator channel_it = noc->token_ring->flag_signals.begin();
         channel_it != noc->token_ring->flag_signals.end();
         ++channel_it) {
        for (map<int, sc_signal<int>* >::const_iterator hub_it = channel_it->second.begin();
             hub_it != channel_it->second.end();
             ++hub_it) {
            traceNamed(tf, *(hub_it->second),
                       "tokenring.channel_" + to_string(channel_it->first) +
                       ".flag_hub_" + to_string(hub_it->first));
        }
    }
}

} // namespace

void signalHandler( int signum )
{
    cout << "\b\b  " << endl;
    cout << endl;
    cout << "Current Statistics:" << endl;
    cout << "(" << sc_time_stamp().to_double() / GlobalParams::clock_period_ps << " sim cycles executed)" << endl;
    GlobalStats gs(n);
    gs.showStats(std::cout, GlobalParams::detailed);
}

int sc_main(int arg_num, char *arg_vet[])
{
    signal(SIGQUIT, signalHandler);  

    // TEMP
    drained_volume = 0;

    // Handle command-line arguments
    cout << "\t--------------------------------------------" << endl; 
    cout << "\t\tNoxim - the NoC Simulator" << endl;
    cout << "\t\t(C) University of Catania" << endl;
    cout << "\t--------------------------------------------" << endl; 

    cout << "Catania V., Mineo A., Monteleone S., Palesi M., and Patti D. (2016) Cycle-Accurate Network on Chip Simulation with Noxim. ACM Trans. Model. Comput. Simul. 27, 1, Article 4 (August 2016), 25 pages. DOI: https://doi.org/10.1145/2953878" << endl;
    cout << endl;
    cout << endl;

    configure(arg_num, arg_vet);
    noxim::Logger::instance().configure(GlobalParams::log_level,
                                        GlobalParams::log_file,
                                        GlobalParams::log_to_stderr,
                                        GlobalParams::log_components);


    // Signals
    sc_clock clock("clock", GlobalParams::clock_period_ps, SC_PS);
    sc_signal <bool> reset;

    // NoC instance
    n = new NoC("NoC");

    n->clock(clock);
    n->reset(reset);

    // Trace signals
    sc_trace_file *tf = NULL;
    if (GlobalParams::trace_mode) {
	tf = sc_create_vcd_trace_file(GlobalParams::trace_filename.c_str());
	sc_trace(tf, reset, "reset");
	sc_trace(tf, clock, "clock");
        const set<string> scopes = parseTraceScopes(GlobalParams::trace_scope);
        const int dim_x = getSwitchDimX();
        const int dim_y = getSwitchDimY();

        if (traceScopeEnabled(scopes, "basic"))
            traceBasicSignals(tf, n, dim_x, dim_y);

        if (traceScopeEnabled(scopes, "router"))
            traceRouterSignals(tf, n, dim_x, dim_y);

        if (traceScopeEnabled(scopes, "buffers"))
            traceBufferSignals(tf, n, dim_x, dim_y);

        if (traceScopeEnabled(scopes, "wireless"))
            traceWirelessSignals(tf, n, dim_x, dim_y);
    }
    // Reset the chip and run the simulation
    reset.write(1);
    cout << "Reset for " << (int)(GlobalParams::reset_time) << " cycles... ";
    srand(GlobalParams::rnd_generator_seed);

    // fix clock periods different from 1ns
    //sc_start(GlobalParams::reset_time, SC_NS);
    sc_start(sc_time((double)GlobalParams::reset_time * GlobalParams::clock_period_ps, SC_PS));

    reset.write(0);
    cout << " done! " << endl;
    cout << " Now running for " << GlobalParams:: simulation_time << " cycles..." << endl;
    // fix clock periods different from 1ns
    //sc_start(GlobalParams::simulation_time, SC_NS);
    sc_start(sc_time((double)GlobalParams::simulation_time * GlobalParams::clock_period_ps, SC_PS));


    // Close the simulation
    if (GlobalParams::trace_mode) sc_close_vcd_trace_file(tf);
    cout << "Noxim simulation completed.";
    cout << " (" << sc_time_stamp().to_double() / GlobalParams::clock_period_ps << " cycles executed)" << endl;
    cout << endl;
//assert(false);
    // Show statistics
    GlobalStats gs(n);
    gs.showStats(std::cout, GlobalParams::detailed);
    if (!GlobalParams::stats_file.empty())
        gs.exportStats(GlobalParams::stats_format, GlobalParams::stats_file, GlobalParams::detailed);


    if ((GlobalParams::max_volume_to_be_drained > 0) &&
	(sc_time_stamp().to_double() / GlobalParams::clock_period_ps - GlobalParams::reset_time >=
	 GlobalParams::simulation_time)) {
	cout << endl
         << "WARNING! the number of flits specified with -volume option" << endl
	     << "has not been reached. ( " << drained_volume << " instead of " << GlobalParams::max_volume_to_be_drained << " )" << endl
         << "You might want to try an higher value of simulation cycles" << endl
	     << "using -sim option." << endl;

#ifdef TESTING
	cout << endl
         << " Sum of local drained flits: " << gs.drained_total << endl
	     << endl
         << " Effective drained volume: " << drained_volume;
#endif

    }

#ifdef DEADLOCK_AVOIDANCE
	cout << "***** WARNING: DEADLOCK_AVOIDANCE ENABLED!" << endl;
#endif
    noxim::Logger::instance().shutdown();
    return 0;
}
