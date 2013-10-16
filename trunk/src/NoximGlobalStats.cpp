/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementaton of the global statistics
 */

#include "NoximGlobalStats.h"
using namespace std;

NoximGlobalStats::NoximGlobalStats(const NoximNoC * _noc)
{
    noc = _noc;

#ifdef TESTING
    drained_total = 0;
#endif
}

double NoximGlobalStats::getAverageDelay()
{
    unsigned int total_packets = 0;
    double avg_delay = 0.0;

    for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++) {
	    unsigned int received_packets =
		noc->t[x][y]->r->stats.getReceivedPackets();

	    if (received_packets) {
		avg_delay +=
		    received_packets *
		    noc->t[x][y]->r->stats.getAverageDelay();
		total_packets += received_packets;
	    }
	}

    avg_delay /= (double) total_packets;

    return avg_delay;
}

double NoximGlobalStats::getAverageDelay(const int src_id,
					 const int dst_id)
{
    NoximTile *tile = noc->searchNode(dst_id);

    assert(tile != NULL);

    return tile->r->stats.getAverageDelay(src_id);
}

double NoximGlobalStats::getMaxDelay()
{
    double maxd = -1.0;

    for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++) {
	    NoximCoord coord;
	    coord.x = x;
	    coord.y = y;
	    int node_id = coord2Id(coord);
	    double d = getMaxDelay(node_id);
	    if (d > maxd)
		maxd = d;
	}

    return maxd;
}

double NoximGlobalStats::getMaxDelay(const int node_id)
{
    NoximCoord coord = id2Coord(node_id);

    unsigned int received_packets =
	noc->t[coord.x][coord.y]->r->stats.getReceivedPackets();

    if (received_packets)
	return noc->t[coord.x][coord.y]->r->stats.getMaxDelay();
    else
	return -1.0;
}

double NoximGlobalStats::getMaxDelay(const int src_id, const int dst_id)
{
    NoximTile *tile = noc->searchNode(dst_id);

    assert(tile != NULL);

    return tile->r->stats.getMaxDelay(src_id);
}

vector < vector < double > > NoximGlobalStats::getMaxDelayMtx()
{
    vector < vector < double > > mtx;

    mtx.resize(NoximGlobalParams::mesh_dim_y);
    for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	mtx[y].resize(NoximGlobalParams::mesh_dim_x);

    for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++) {
	    NoximCoord coord;
	    coord.x = x;
	    coord.y = y;
	    int id = coord2Id(coord);
	    mtx[y][x] = getMaxDelay(id);
	}

    return mtx;
}

double NoximGlobalStats::getAverageThroughput(const int src_id,
					      const int dst_id)
{
    NoximTile *tile = noc->searchNode(dst_id);

    assert(tile != NULL);

    return tile->r->stats.getAverageThroughput(src_id);
}

double NoximGlobalStats::getAverageThroughput()
{
    unsigned int total_comms = 0;
    double avg_throughput = 0.0;

    for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++) {
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

unsigned int NoximGlobalStats::getReceivedPackets()
{
    unsigned int n = 0;

    for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++)
	    n += noc->t[x][y]->r->stats.getReceivedPackets();

    return n;
}

unsigned int NoximGlobalStats::getReceivedFlits()
{
    unsigned int n = 0;

    for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++) {
	    n += noc->t[x][y]->r->stats.getReceivedFlits();
#ifdef TESTING
	    drained_total += noc->t[x][y]->r->local_drained;
#endif
	}

    return n;
}

double NoximGlobalStats::getThroughput()
{
    int total_cycles =
	NoximGlobalParams::simulation_time -
	NoximGlobalParams::stats_warm_up_time;

    //  int number_of_ip = NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y;
    //  return (double)getReceivedFlits()/(double)(total_cycles * number_of_ip);

    unsigned int n = 0;
    unsigned int trf = 0;
    for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++) {
	    unsigned int rf = noc->t[x][y]->r->stats.getReceivedFlits();

	    if (rf != 0)
		n++;

	    trf += rf;
	}
    return (double) trf / (double) (total_cycles * n);

}

vector < vector < unsigned long > > NoximGlobalStats::getRoutedFlitsMtx()
{

    vector < vector < unsigned long > > mtx;

    mtx.resize(NoximGlobalParams::mesh_dim_y);
    for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	mtx[y].resize(NoximGlobalParams::mesh_dim_x);

    for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++)
	    mtx[y][x] = noc->t[x][y]->r->getRoutedFlits();

    return mtx;
}

double NoximGlobalStats::getPower()
{
    double power = 0.0;

    for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++)
	    power += noc->t[x][y]->r->getPower();

    return power;
}

void NoximGlobalStats::showStats(std::ostream & out, bool detailed)
{
    out << "% Total received packets: " << getReceivedPackets() << endl;
    out << "% Total received flits: " << getReceivedFlits() << endl;
    out << "% Global average delay (cycles): " << getAverageDelay() <<
	endl;
    out << "% Global average throughput (flits/cycle): " <<
	getAverageThroughput() << endl;
    out << "% Throughput (flits/cycle/IP): " << getThroughput() << endl;
    out << "% Max delay (cycles): " << getMaxDelay() << endl;
    out << "% Total energy (J): " << getPower() << endl;

    if (NoximGlobalParams::show_buffer_stats)
      showBufferStats(out);

    if (detailed) {
	out << endl << "detailed = [" << endl;
	for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	    for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++)
		noc->t[x][y]->r->stats.showStats(y *
						 NoximGlobalParams::
						 mesh_dim_x + x, out,
						 true);
	out << "];" << endl;

	// show MaxDelay matrix
	vector < vector < double > > md_mtx = getMaxDelayMtx();

	out << endl << "max_delay = [" << endl;
	for (unsigned int y = 0; y < md_mtx.size(); y++) {
	    out << "   ";
	    for (unsigned int x = 0; x < md_mtx[y].size(); x++)
		out << setw(6) << md_mtx[y][x];
	    out << endl;
	}
	out << "];" << endl;

	// show RoutedFlits matrix
	vector < vector < unsigned long > > rf_mtx = getRoutedFlitsMtx();

	out << endl << "routed_flits = [" << endl;
	for (unsigned int y = 0; y < rf_mtx.size(); y++) {
	    out << "   ";
	    for (unsigned int x = 0; x < rf_mtx[y].size(); x++)
		out << setw(10) << rf_mtx[y][x];
	    out << endl;
	}
	out << "];" << endl;
    }
}


void NoximGlobalStats::showBufferStats(std::ostream & out)
{
  out << "Router id\tBuffer N\t\tBuffer E\t\tBuffer S\t\tBuffer W\t\tBuffer L" << endl;
  out << "         \tMean\tMax\tMean\tMax\tMean\tMax\tMean\tMax\tMean\tMax" << endl;
  for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
    for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++)
      {
	out << noc->t[x][y]->r->local_id;
	noc->t[x][y]->r->ShowBuffersStats(out);
	out << endl;
      }
}
