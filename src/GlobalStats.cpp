/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementaton of the global statistics
 */

#include "GlobalStats.h"
#include "DeftTopology.h"

#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>

using namespace std;

namespace {

bool isMeshStatsTopology()
{
    return GlobalParams::topology == TOPOLOGY_MESH ||
           GlobalParams::topology == TOPOLOGY_DEFT_2_5D;
}

int statsDimX()
{
    if (GlobalParams::topology == TOPOLOGY_DEFT_2_5D)
        return DeftTopology::LayoutWidth;

    return GlobalParams::mesh_dim_x;
}

int statsDimY()
{
    if (GlobalParams::topology == TOPOLOGY_DEFT_2_5D)
        return DeftTopology::LayoutHeight;

    return GlobalParams::mesh_dim_y;
}

int statsIpCount()
{
    if (GlobalParams::topology == TOPOLOGY_DEFT_2_5D)
        return DeftTopology::ChipletRouterCount;

    return GlobalParams::mesh_dim_x * GlobalParams::mesh_dim_y;
}

struct SummaryMetrics {
    unsigned int total_received_packets;
    unsigned int total_received_flits;
    double received_ideal_flits_ratio;
    double average_wireless_utilization;
    double global_average_delay_cycles;
    double max_delay_cycles;
    double network_throughput_flits_per_cycle;
    double average_ip_throughput_flits_per_cycle_per_ip;
    double total_energy_j;
    double dynamic_energy_j;
    double static_energy_j;
    double executed_cycles;
};

SummaryMetrics buildSummaryMetrics(GlobalStats &stats)
{
    SummaryMetrics metrics;
    metrics.total_received_packets = stats.getReceivedPackets();
    metrics.total_received_flits = stats.getReceivedFlits();
    metrics.received_ideal_flits_ratio = stats.getReceivedIdealFlitRatio();
    metrics.average_wireless_utilization =
        stats.getWirelessPackets() / (double)metrics.total_received_packets;
    metrics.global_average_delay_cycles = stats.getAverageDelay();
    metrics.max_delay_cycles = stats.getMaxDelay();
    metrics.network_throughput_flits_per_cycle = stats.getAggregatedThroughput();
    metrics.average_ip_throughput_flits_per_cycle_per_ip = stats.getThroughput();
    metrics.total_energy_j = stats.getTotalPower();
    metrics.dynamic_energy_j = stats.getDynamicPower();
    metrics.static_energy_j = stats.getStaticPower();
    metrics.executed_cycles = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
    return metrics;
}

string csvNumber(double value)
{
    if (!std::isfinite(value))
        return "nan";

    ostringstream out;
    out.precision(17);
    out << value;
    return out.str();
}

string jsonString(const string &value)
{
    ostringstream out;
    out << '"';
    for (size_t i = 0; i < value.size(); ++i) {
        const char ch = value[i];
        switch (ch) {
        case '\\':
            out << "\\\\";
            break;
        case '"':
            out << "\\\"";
            break;
        case '\n':
            out << "\\n";
            break;
        case '\r':
            out << "\\r";
            break;
        case '\t':
            out << "\\t";
            break;
        default:
            out << ch;
            break;
        }
    }
    out << '"';
    return out.str();
}

string jsonNumber(double value)
{
    if (!std::isfinite(value))
        return "null";

    ostringstream out;
    out.precision(17);
    out << value;
    return out.str();
}

void writeCsvSummary(ostream &out, const SummaryMetrics &metrics)
{
    out << "topology"
        << ",mesh_dim_x"
        << ",mesh_dim_y"
        << ",n_delta_tiles"
        << ",simulation_time_cycles"
        << ",reset_time_cycles"
        << ",stats_warm_up_time_cycles"
        << ",executed_cycles"
        << ",rnd_generator_seed"
        << ",total_received_packets"
        << ",total_received_flits"
        << ",received_ideal_flits_ratio"
        << ",average_wireless_utilization"
        << ",global_average_delay_cycles"
        << ",max_delay_cycles"
        << ",network_throughput_flits_per_cycle"
        << ",average_ip_throughput_flits_per_cycle_per_ip"
        << ",total_energy_j"
        << ",dynamic_energy_j"
        << ",static_energy_j"
        << endl;

    out << GlobalParams::topology
        << "," << GlobalParams::mesh_dim_x
        << "," << GlobalParams::mesh_dim_y
        << "," << GlobalParams::n_delta_tiles
        << "," << GlobalParams::simulation_time
        << "," << GlobalParams::reset_time
        << "," << GlobalParams::stats_warm_up_time
        << "," << csvNumber(metrics.executed_cycles)
        << "," << GlobalParams::rnd_generator_seed
        << "," << metrics.total_received_packets
        << "," << metrics.total_received_flits
        << "," << csvNumber(metrics.received_ideal_flits_ratio)
        << "," << csvNumber(metrics.average_wireless_utilization)
        << "," << csvNumber(metrics.global_average_delay_cycles)
        << "," << csvNumber(metrics.max_delay_cycles)
        << "," << csvNumber(metrics.network_throughput_flits_per_cycle)
        << "," << csvNumber(metrics.average_ip_throughput_flits_per_cycle_per_ip)
        << "," << csvNumber(metrics.total_energy_j)
        << "," << csvNumber(metrics.dynamic_energy_j)
        << "," << csvNumber(metrics.static_energy_j)
        << endl;
}

void writeJsonSummary(ostream &out, const SummaryMetrics &metrics)
{
    out << "{" << endl;
    out << "  \"config\": {" << endl;
    out << "    \"topology\": " << jsonString(GlobalParams::topology) << "," << endl;
    out << "    \"mesh_dim_x\": " << GlobalParams::mesh_dim_x << "," << endl;
    out << "    \"mesh_dim_y\": " << GlobalParams::mesh_dim_y << "," << endl;
    out << "    \"n_delta_tiles\": " << GlobalParams::n_delta_tiles << "," << endl;
    out << "    \"simulation_time_cycles\": " << GlobalParams::simulation_time << "," << endl;
    out << "    \"reset_time_cycles\": " << GlobalParams::reset_time << "," << endl;
    out << "    \"stats_warm_up_time_cycles\": " << GlobalParams::stats_warm_up_time << "," << endl;
    out << "    \"rnd_generator_seed\": " << GlobalParams::rnd_generator_seed << endl;
    out << "  }," << endl;
    out << "  \"summary\": {" << endl;
    out << "    \"executed_cycles\": " << jsonNumber(metrics.executed_cycles) << "," << endl;
    out << "    \"total_received_packets\": " << metrics.total_received_packets << "," << endl;
    out << "    \"total_received_flits\": " << metrics.total_received_flits << "," << endl;
    out << "    \"received_ideal_flits_ratio\": " << jsonNumber(metrics.received_ideal_flits_ratio) << "," << endl;
    out << "    \"average_wireless_utilization\": " << jsonNumber(metrics.average_wireless_utilization) << "," << endl;
    out << "    \"global_average_delay_cycles\": " << jsonNumber(metrics.global_average_delay_cycles) << "," << endl;
    out << "    \"max_delay_cycles\": " << jsonNumber(metrics.max_delay_cycles) << "," << endl;
    out << "    \"network_throughput_flits_per_cycle\": " << jsonNumber(metrics.network_throughput_flits_per_cycle) << "," << endl;
    out << "    \"average_ip_throughput_flits_per_cycle_per_ip\": " << jsonNumber(metrics.average_ip_throughput_flits_per_cycle_per_ip) << "," << endl;
    out << "    \"total_energy_j\": " << jsonNumber(metrics.total_energy_j) << "," << endl;
    out << "    \"dynamic_energy_j\": " << jsonNumber(metrics.dynamic_energy_j) << "," << endl;
    out << "    \"static_energy_j\": " << jsonNumber(metrics.static_energy_j) << endl;
    out << "  }" << endl;
    out << "}" << endl;
}

} // namespace

GlobalStats::GlobalStats(const NoC * _noc)
{
    noc = _noc;

	#ifdef TESTING
    drained_total = 0;
	#endif
}

double GlobalStats::getAverageDelay()
{
    unsigned int total_packets = 0;
    double avg_delay = 0.0;

    if (isMeshStatsTopology())
    {
	for (int y = 0; y < statsDimY(); y++)
	    for (int x = 0; x < statsDimX(); x++)
	    {
		unsigned int received_packets =
		    noc->t[x][y]->r->stats.getReceivedPackets();

		if (received_packets)
		{
		    avg_delay +=
			received_packets *
			noc->t[x][y]->r->stats.getAverageDelay();
		    total_packets += received_packets;
		}
	    }
    }
    else // other delta topologies
    {
	for (int y = 0; y < GlobalParams::n_delta_tiles; y++)
	{
	    unsigned int received_packets =
		noc->core[y]->r->stats.getReceivedPackets();

	    if (received_packets)
	    {
		avg_delay +=
		    received_packets *
		    noc->core[y]->r->stats.getAverageDelay();
		total_packets += received_packets;
	    }
	}

    }


    avg_delay /= (double) total_packets;

    return avg_delay;
}



double GlobalStats::getAverageDelay(const int src_id,
					 const int dst_id)
{
    Tile *tile = noc->searchNode(dst_id);

    assert(tile != NULL);

    return tile->r->stats.getAverageDelay(src_id);
}

double GlobalStats::getMaxDelay()
{
    double maxd = -1.0;

    if (isMeshStatsTopology())
    {
	for (int y = 0; y < statsDimY(); y++)
	    for (int x = 0; x < statsDimX(); x++)
	    {
		int node_id = noc->t[x][y]->r->local_id;
		double d = getMaxDelay(node_id);
		if (d > maxd)
		    maxd = d;
	    }

    }
    else  // other delta topologies
    {
	for (int y = 0; y < GlobalParams::n_delta_tiles; y++)
	{
	    double d = getMaxDelay(y);
	    if (d > maxd)
		maxd = d;
	}
    }

    return maxd;
}

double GlobalStats::getMaxDelay(const int node_id)
{
    if (isMeshStatsTopology())
    {
	Tile *tile = noc->searchNode(node_id);
	assert(tile != NULL);

	unsigned int received_packets =
	    tile->r->stats.getReceivedPackets();

	if (received_packets)
	    return tile->r->stats.getMaxDelay();
	else
	    return -1.0;
    }
    else // other delta topologies
    {
	unsigned int received_packets =
	    noc->core[node_id]->r->stats.getReceivedPackets();
	if (received_packets)
	    return noc->core[node_id]->r->stats.getMaxDelay();
	else
	    return -1.0;
    }

}

double GlobalStats::getMaxDelay(const int src_id, const int dst_id)
{
    Tile *tile = noc->searchNode(dst_id);

    assert(tile != NULL);

    return tile->r->stats.getMaxDelay(src_id);
}

vector < vector < double > > GlobalStats::getMaxDelayMtx()
{
    vector < vector < double > > mtx;

    assert(isMeshStatsTopology());

    mtx.resize(statsDimY());
    for (int y = 0; y < statsDimY(); y++)
	mtx[y].resize(statsDimX());

    for (int y = 0; y < statsDimY(); y++)
	for (int x = 0; x < statsDimX(); x++)
	{
	    int id = noc->t[x][y]->r->local_id;
	    mtx[y][x] = getMaxDelay(id);
	}

    return mtx;
}

double GlobalStats::getAverageThroughput(const int src_id, const int dst_id)
{
    Tile *tile = noc->searchNode(dst_id);

    assert(tile != NULL);

    return tile->r->stats.getAverageThroughput(src_id);
}

/*
double GlobalStats::getAverageThroughput()
{
    unsigned int total_comms = 0;
    double avg_throughput = 0.0;

    for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < GlobalParams::mesh_dim_x; x++) {
	    unsigned int ncomms =
		noc->t[x][y]->r->stats.getTotalCommunications();

	    if (ncomms) {
		avg_throughput +=
		    ncomms * noc->t[x][y]->r->stats.getAverageThroughput();
		total_comms += ncomms;
	    }
	}

    avg_throughput /= (double) total_comms;

    return avg_throughput;
}
*/

double GlobalStats::getAggregatedThroughput()
{
    int total_cycles = GlobalParams::simulation_time - GlobalParams::stats_warm_up_time;

    return (double)getReceivedFlits()/(double)(total_cycles);
}

unsigned int GlobalStats::getReceivedPackets()
{
    unsigned int n = 0;

    if (isMeshStatsTopology())
    {
	for (int y = 0; y < statsDimY(); y++)
		for (int x = 0; x < statsDimX(); x++)
	    n += noc->t[x][y]->r->stats.getReceivedPackets();
    }
    else // other delta topologies
    {
	for (int y = 0; y < GlobalParams::n_delta_tiles; y++)
	    n += noc->core[y]->r->stats.getReceivedPackets();
    }

    return n;
}

unsigned int GlobalStats::getReceivedFlits()
{
    unsigned int n = 0;
    if (isMeshStatsTopology())
    {
	for (int y = 0; y < statsDimY(); y++)
	    for (int x = 0; x < statsDimX(); x++) {
		n += noc->t[x][y]->r->stats.getReceivedFlits();
#ifdef TESTING
		drained_total += noc->t[x][y]->r->local_drained;
#endif
	    }
    }
    else // other delta topologies
    {
	for (int y = 0; y < GlobalParams::n_delta_tiles; y++)
	{
	    n += noc->core[y]->r->stats.getReceivedFlits();
#ifdef TESTING
	    drained_total += noc->core[y]->r->local_drained;
#endif
	}
    }

    return n;
}

double GlobalStats::getThroughput()
{
    if (isMeshStatsTopology())
    {
	int number_of_ip = statsIpCount();
	return (double)getAggregatedThroughput()/(double)(number_of_ip);
    }
    else // other delta topologies
    {
	int number_of_ip = GlobalParams::n_delta_tiles;
	return (double)getAggregatedThroughput()/(double)(number_of_ip);
    }
}

// Only accounting IP that received at least one flit
double GlobalStats::getActiveThroughput()
{
    int total_cycles =
	GlobalParams::simulation_time -
	GlobalParams::stats_warm_up_time;
    unsigned int n = 0;
    unsigned int trf = 0;
    unsigned int rf ;
    if (isMeshStatsTopology())
    {
	for (int y = 0; y < statsDimY(); y++)
	    for (int x = 0; x < statsDimX(); x++)
	    {
		rf = noc->t[x][y]->r->stats.getReceivedFlits();

		if (rf != 0)
		    n++;

		trf += rf;
	    }
    }
    else // other delta topologies
    {
	for (int y = 0; y < GlobalParams::n_delta_tiles; y++)
	{
	    rf = noc->core[y]->r->stats.getReceivedFlits();

	    if (rf != 0)
		n++;

	    trf += rf;
	}
    }

    return (double) trf / (double) (total_cycles * n);

}

vector < vector < unsigned long > > GlobalStats::getRoutedFlitsMtx()
{

    vector < vector < unsigned long > > mtx;
    assert (isMeshStatsTopology());

    mtx.resize(statsDimY());
    for (int y = 0; y < statsDimY(); y++)
	mtx[y].resize(statsDimX());

    for (int y = 0; y < statsDimY(); y++)
	for (int x = 0; x < statsDimX(); x++)
	    mtx[y][x] = noc->t[x][y]->r->getRoutedFlits();


    return mtx;
}

unsigned int GlobalStats::getWirelessPackets()
{
    unsigned int packets = 0;

    // Wireless noc
    for (map<int, HubConfig>::iterator it = GlobalParams::hub_configuration.begin();
            it != GlobalParams::hub_configuration.end();
            ++it)
    {
	int hub_id = it->first;

	map<int,Hub*>::const_iterator i = noc->hub.find(hub_id);
	Hub * h = i->second;

	packets+= h->wireless_communications_counter;
    }
    return packets;
}

double GlobalStats::getDynamicPower()
{
    double power = 0.0;

    // Electric noc
    if (isMeshStatsTopology())
    {
	for (int y = 0; y < statsDimY(); y++)
	    for (int x = 0; x < statsDimX(); x++)
		power += noc->t[x][y]->r->power.getDynamicPower();
    }
    else // other delta topologies
    {
	int stg = log2(GlobalParams::n_delta_tiles);
	int sw = GlobalParams::n_delta_tiles/2; //sw: switch number in each stage
	// Dimensions of the delta switch block network
	int dimX = stg;
	int dimY = sw;

	// power for delta topologies cores
	for (int y = 0; y < GlobalParams::n_delta_tiles; y++)
	    power += noc->core[y]->r->power.getDynamicPower();

	// power for delta topologies switches
	for (int y = 0; y < dimY; y++)
	    for (int x = 0; x < dimX; x++)
		power += noc->t[x][y]->r->power.getDynamicPower();
    }

    // Wireless noc
    for (map<int, HubConfig>::iterator it = GlobalParams::hub_configuration.begin();
	    it != GlobalParams::hub_configuration.end();
	    ++it)
    {
	int hub_id = it->first;

	map<int,Hub*>::const_iterator i = noc->hub.find(hub_id);
	Hub * h = i->second;

	power+= h->power.getDynamicPower();
    }
    return power;
}

double GlobalStats::getStaticPower()
{
    double power = 0.0;

    if (isMeshStatsTopology())
    {
	for (int y = 0; y < statsDimY(); y++)
		for (int x = 0; x < statsDimX(); x++)
	    power += noc->t[x][y]->r->power.getStaticPower();
    }
    else // other delta topologies
    {
	int stg = log2(GlobalParams::n_delta_tiles);
	int sw = GlobalParams::n_delta_tiles/2; //sw: switch number in each stage
	// Dimensions of the delta switch block network
	int dimX = stg;
	int dimY = sw;
	// power for delta topologies switches
	for (int y = 0; y < dimY; y++)
	    for (int x = 0; x < dimX; x++)
		power += noc->t[x][y]->r->power.getDynamicPower();

	// delta cores
	for (int y = 0; y < GlobalParams::n_delta_tiles; y++)
	    power += noc->core[y]->r->power.getStaticPower();
    }

    // Wireless noc
    for (map<int, HubConfig>::iterator it = GlobalParams::hub_configuration.begin();
            it != GlobalParams::hub_configuration.end();
            ++it)
    {
	int hub_id = it->first;

	map<int,Hub*>::const_iterator i = noc->hub.find(hub_id);
	Hub * h = i->second;

	power+= h->power.getStaticPower();
    }
    return power;
}

void GlobalStats::showStats(std::ostream & out, bool detailed)
{
    if (detailed)
    {
	if (isMeshStatsTopology())
    {
	out << endl << "detailed = [" << endl;

	for (int y = 0; y < statsDimY(); y++)
	    for (int x = 0; x < statsDimX(); x++)
		noc->t[x][y]->r->stats.showStats(noc->t[x][y]->r->local_id, out, true);
	out << "];" << endl;

	// show MaxDelay matrix
	vector < vector < double > > md_mtx = getMaxDelayMtx();

	out << endl << "max_delay = [" << endl;
	for (unsigned int y = 0; y < md_mtx.size(); y++)
	{
	    out << "   ";
	    for (unsigned int x = 0; x < md_mtx[y].size(); x++)
		out << setw(6) << md_mtx[y][x];
	    out << endl;
	}
	out << "];" << endl;

	// show RoutedFlits matrix
	vector < vector < unsigned long > > rf_mtx = getRoutedFlitsMtx();

	out << endl << "routed_flits = [" << endl;
	for (unsigned int y = 0; y < rf_mtx.size(); y++)
	{
	    out << "   ";
	    for (unsigned int x = 0; x < rf_mtx[y].size(); x++)
		out << setw(10) << rf_mtx[y][x];
	    out << endl;
	}
	out << "];" << endl;
    }
    else //other delta topologies
    {
    out << endl << "detailed = [" << endl;

    for (int y = 0; y < GlobalParams::n_delta_tiles; y++)
    noc->core[y]->r->stats.showStats(y, out, true);
    out << "];" << endl;

    // For delta topologies, we can't show matrix format stats
    // since they don't have a 2D structure
    out << endl << "% Note: Matrix format stats not available for delta topologies" << endl;

    }
	showPowerBreakDown(out);
	showPowerManagerStats(out);
    }

#ifdef DEBUG

    if (isMeshStatsTopology())
    {
	for (int y = 0; y < statsDimY(); y++)
	    for (int x = 0; x < statsDimX(); x++)
		out << "PE["<<x << "," << y<< "]" << noc->t[x][y]->pe->getQueueSize()<< ",";
    }
    else // other delta topologies
    {
	out << "Queue sizes: " ;
	for (int i=0;i<GlobalParams::n_delta_tiles;i++)
		out << "PE"<<i << ": " << noc->core[i]->pe->getQueueSize()<< ",";
	out << endl;
    }

    out << endl;
#endif

    //int total_cycles = GlobalParams::simulation_time - GlobalParams::stats_warm_up_time;
    SummaryMetrics metrics = buildSummaryMetrics(*this);
    out << "% Total received packets: " << metrics.total_received_packets << endl;
    out << "% Total received flits: " << metrics.total_received_flits << endl;
    out << "% Received/Ideal flits Ratio: " << metrics.received_ideal_flits_ratio << endl;
    out << "% Average wireless utilization: " << metrics.average_wireless_utilization << endl;
    out << "% Global average delay (cycles): " << metrics.global_average_delay_cycles << endl;
    out << "% Max delay (cycles): " << metrics.max_delay_cycles << endl;
    out << "% Network throughput (flits/cycle): " << metrics.network_throughput_flits_per_cycle << endl;
    out << "% Average IP throughput (flits/cycle/IP): " << metrics.average_ip_throughput_flits_per_cycle_per_ip << endl;
    out << "% Total energy (J): " << metrics.total_energy_j << endl;
    out << "% \tDynamic energy (J): " << metrics.dynamic_energy_j << endl;
    out << "% \tStatic energy (J): " << metrics.static_energy_j << endl;

    if (GlobalParams::show_buffer_stats)
      showBufferStats(out);

}

void GlobalStats::exportStats(const string &format, const string &filename, bool detailed)
{
    ofstream out(filename.c_str(), ios::out | ios::trunc);
    if (!out.good()) {
        cerr << "Error: Cannot open stats file " << filename << endl;
        exit(1);
    }

    if (format == "text") {
        showStats(out, detailed);
        return;
    }

    SummaryMetrics metrics = buildSummaryMetrics(*this);

    if (format == "csv")
        writeCsvSummary(out, metrics);
    else if (format == "json")
        writeJsonSummary(out, metrics);
    else {
        cerr << "Error: Unsupported stats format " << format << endl;
        exit(1);
    }
}

void GlobalStats::updatePowerBreakDown(map<string,double> &dst,PowerBreakdown* src)
{
    for (int i=0;i!=src->size;i++)
    {
		dst[src->breakdown[i].label]+=src->breakdown[i].value;
    }
}

void GlobalStats::showPowerManagerStats(std::ostream & out)
{
    std::streamsize p = out.precision();
    int total_cycles = sc_time_stamp().to_double() / GlobalParams::clock_period_ps - GlobalParams::reset_time;

    out.precision(4);

    out << "powermanager_stats_tx = [" << endl;
    out << "%\tFraction of: TX Transceiver off (TTXoff), AntennaBufferTX off (ABTXoff) " << endl;
    out << "%\tHUB\tTTXoff\tABTXoff\t" << endl;

    for (map<int, HubConfig>::iterator it = GlobalParams::hub_configuration.begin();
            it != GlobalParams::hub_configuration.end();
            ++it)
    {
	int hub_id = it->first;

	map<int,Hub*>::const_iterator i = noc->hub.find(hub_id);
	Hub * h = i->second;

	out << "\t" << hub_id << "\t" << std::fixed << (double)h->total_ttxoff_cycles/total_cycles << "\t";

	int s = 0;
	for (map<int,int>::iterator i = h->abtxoff_cycles.begin(); i!=h->abtxoff_cycles.end();i++) s+=i->second;

	out << (double)s/h->abtxoff_cycles.size()/total_cycles << endl;
    }

    out << "];" << endl;



    out << "powermanager_stats_rx = [" << endl;
    out << "%\tFraction of: RX Transceiver off (TRXoff), AntennaBufferRX off (ABRXoff), BufferToTile off (BTToff) " << endl;
    out << "%\tHUB\tTRXoff\tABRXoff\tBTToff\t" << endl;



    for (map<int, HubConfig>::iterator it = GlobalParams::hub_configuration.begin();
            it != GlobalParams::hub_configuration.end();
            ++it)
    {
	string bttoff_str;

	out.precision(4);

	int hub_id = it->first;

	map<int,Hub*>::const_iterator i = noc->hub.find(hub_id);
	Hub * h = i->second;

	out << "\t" << hub_id << "\t" << std::fixed << (double)h->total_sleep_cycles/total_cycles << "\t";

	int s = 0;
	for (map<int,int>::iterator i = h->buffer_rx_sleep_cycles.begin();
		i!=h->buffer_rx_sleep_cycles.end();i++)
	    s+=i->second;

	out << (double)s/h->buffer_rx_sleep_cycles.size()/total_cycles << "\t";

	s = 0;
	for (map<int,int>::iterator i = h->buffer_to_tile_poweroff_cycles.begin();
		i!=h->buffer_to_tile_poweroff_cycles.end();i++)
	{
	    double bttoff_fraction = i->second/(double)total_cycles;
	    s+=i->second;
	    if (bttoff_fraction<0.25)
		bttoff_str+=" ";
	    else if (bttoff_fraction<0.5)
		    bttoff_str+=".";
	    else if (bttoff_fraction<0.75)
		    bttoff_str+="o";
	    else if (bttoff_fraction<0.90)
		    bttoff_str+="O";
	    else
		bttoff_str+="0";


	}
	out << (double)s/h->buffer_to_tile_poweroff_cycles.size()/total_cycles << "\t" << bttoff_str << endl;
    }

    out << "];" << endl;

    out.unsetf(std::ios::fixed);

    out.precision(p);

}

void GlobalStats::showPowerBreakDown(std::ostream & out)
{
    map<string,double> power_dynamic;
    map<string,double> power_static;

    if (isMeshStatsTopology())
    {
	for (int y = 0; y < statsDimY(); y++)
	    for (int x = 0; x < statsDimX(); x++)
	    {
		updatePowerBreakDown(power_dynamic, noc->t[x][y]->r->power.getDynamicPowerBreakDown());
		updatePowerBreakDown(power_static, noc->t[x][y]->r->power.getStaticPowerBreakDown());
	    }
    }
    else // other delta topologies
    {
	for (int y = 0; y < GlobalParams::n_delta_tiles; y++)
	{
	    updatePowerBreakDown(power_dynamic, noc->core[y]->r->power.getDynamicPowerBreakDown());
	    updatePowerBreakDown(power_static, noc->core[y]->r->power.getStaticPowerBreakDown());
	}
    }

    for (map<int, HubConfig>::iterator it = GlobalParams::hub_configuration.begin();
	    it != GlobalParams::hub_configuration.end();
	    ++it)
    {
	int hub_id = it->first;

	map<int,Hub*>::const_iterator i = noc->hub.find(hub_id);
	Hub * h = i->second;

	updatePowerBreakDown(power_dynamic,
		h->power.getDynamicPowerBreakDown());

	updatePowerBreakDown(power_static,
		h->power.getStaticPowerBreakDown());
    }

    printMap("power_dynamic",power_dynamic,out);
    printMap("power_static",power_static,out);

}



void GlobalStats::showBufferStats(std::ostream & out)
{
  out << "Router id\tBuffer N\t\tBuffer E\t\tBuffer S\t\tBuffer W\t\tBuffer L" << endl;
  out << "         \tMean\tMax\tMean\tMax\tMean\tMax\tMean\tMax\tMean\tMax" << endl;

  if (isMeshStatsTopology())
    {
	for (int y = 0; y < statsDimY(); y++)
	for (int x = 0; x < statsDimX(); x++)
	{
			out << noc->t[x][y]->r->local_id;
			noc->t[x][y]->r->ShowBuffersStats(out);
			out << endl;
	}
    }
    else // other delta topologies
    {
	for (int y = 0; y < GlobalParams::n_delta_tiles; y++)
	{
			out << noc->core[y]->r->local_id;
			noc->core[y]->r->ShowBuffersStats(out);
			out << endl;
	}
    }

}

double GlobalStats::getReceivedIdealFlitRatio()
{
    int total_cycles;
    total_cycles= GlobalParams::simulation_time - GlobalParams::stats_warm_up_time;
    double ratio;
    if (isMeshStatsTopology())
    {
	ratio = getReceivedFlits() /(GlobalParams::packet_injection_rate * (GlobalParams::min_packet_size +
		    GlobalParams::max_packet_size)/2 * total_cycles * statsIpCount());
    }
    else // other delta topologies
    {
	ratio = getReceivedFlits() /(GlobalParams::packet_injection_rate * (GlobalParams::min_packet_size +
		    GlobalParams::max_packet_size)/2 * total_cycles * GlobalParams::n_delta_tiles);
    }
    return ratio;
}
