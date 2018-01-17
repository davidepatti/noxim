/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the statistics
 */

#include "Stats.h"

// TODO: nan in averageDelay

void Stats::configure(const int node_id, const double _warm_up_time)
{
    id = node_id;
    warm_up_time = _warm_up_time;
}

void Stats::receivedFlit(const double arrival_time,
			      const Flit & flit)
{
    if (arrival_time - GlobalParams::reset_time < warm_up_time)
	return;

    int i = searchCommHistory(flit.src_id);

    if (i == -1) {
	// first flit received from a given source
	// initialize CommHist structure
	CommHistory ch;

	ch.src_id = flit.src_id;
	ch.total_received_flits = 0;
	chist.push_back(ch);

	i = chist.size() - 1;
    }

    if (flit.flit_type == FLIT_TYPE_HEAD)
	chist[i].delays.push_back(arrival_time - flit.timestamp);

    chist[i].total_received_flits++;
    chist[i].last_received_flit_time = arrival_time - warm_up_time;
}

double Stats::getAverageDelay(const int src_id)
{
    double sum = 0.0;

    int i = searchCommHistory(src_id);

    assert(i >= 0);

    for (unsigned int j = 0; j < chist[i].delays.size(); j++)
	sum += chist[i].delays[j];

    return sum / (double) chist[i].delays.size();
}

double Stats::getAverageDelay()
{
    double avg = 0.0;

    for (unsigned int k = 0; k < chist.size(); k++) {
	unsigned int samples = chist[k].delays.size();
	if (samples)
	    avg += (double) samples *getAverageDelay(chist[k].src_id);
    }

    return avg / (double) getReceivedPackets();
}

double Stats::getMaxDelay(const int src_id)
{
    double maxd = -1.0;

    int i = searchCommHistory(src_id);

    assert(i >= 0);

    for (unsigned int j = 0; j < chist[i].delays.size(); j++)
	if (chist[i].delays[j] > maxd) {
	    maxd = chist[i].delays[j];
	}
    return maxd;
}

double Stats::getMaxDelay()
{
    double maxd = -1.0;

    for (unsigned int k = 0; k < chist.size(); k++) {
	unsigned int samples = chist[k].delays.size();
	if (samples) {
	    double m = getMaxDelay(chist[k].src_id);
	    if (m > maxd)
		maxd = m;
	}
    }

    return maxd;
}

double Stats::getAverageThroughput(const int src_id)
{
    int i = searchCommHistory(src_id);

    assert(i >= 0);

    // not using GlobalParams::simulation_time since 
    // the value must takes into account the invokation time
    // (when called before simulation ended, e.g. turi signal)
    int current_sim_cycles = sc_time_stamp().to_double()/GlobalParams::clock_period_ps - warm_up_time - GlobalParams::reset_time;

    if (chist[i].total_received_flits == 0)
	return -1.0;
    else
	return (double) chist[i].total_received_flits / current_sim_cycles;
	    //(double) chist[i].last_received_flit_time;
}

double Stats::getAverageThroughput()
{
    double sum = 0.0;

    for (unsigned int k = 0; k < chist.size(); k++) {
	double avg = getAverageThroughput(chist[k].src_id);
	if (avg > 0.0)
	    sum += avg;
    }

    return sum;
}

unsigned int Stats::getReceivedPackets()
{
    int n = 0;

    for (unsigned int i = 0; i < chist.size(); i++)
	n += chist[i].delays.size();

    return n;
}

unsigned int Stats::getReceivedFlits()
{
    int n = 0;

    for (unsigned int i = 0; i < chist.size(); i++)
	n += chist[i].total_received_flits;

    return n;
}

unsigned int Stats::getTotalCommunications()
{
    return chist.size();
}

double Stats::getCommunicationEnergy(int src_id, int dst_id)
{
  // NOT YET IMPLEMENTED
    // Assumptions: minimal path routing, constant packet size
  /*
    Coord src_coord = id2Coord(src_id);
    Coord dst_coord = id2Coord(dst_id);

    int hops =
	abs(src_coord.x - dst_coord.x) + abs(src_coord.y - dst_coord.y);

    double energy =
	hops * (power.getPwrArbitration() + power.getPwrCrossbar() +
		 power.getPwrBuffering() *
		(GlobalParams::min_packet_size +
		 GlobalParams::max_packet_size) / 2 +
		power.getPwrRouting() + power.getPwrSelection()
	);

    return energy;
  */
  return -1.0;
}

int Stats::searchCommHistory(int src_id)
{
    for (unsigned int i = 0; i < chist.size(); i++)
	if (chist[i].src_id == src_id)
	    return i;

    return -1;
}

void Stats::showStats(int curr_node, std::ostream & out, bool header)
{
    if (header) {
	out << "%"
	    << setw(5) << "src"
	    << setw(5) << "dst"
	    << setw(10) << "delay avg"
	    << setw(10) << "delay max"
	    << setw(15) << "throughput"
	    << setw(13) << "energy"
	    << setw(12) << "received" << setw(12) << "received" << endl;
	out << "%"
	    << setw(5) << ""
	    << setw(5) << ""
	    << setw(10) << "cycles"
	    << setw(10) << "cycles"
	    << setw(15) << "flits/cycle"
	    << setw(13) << "Joule"
	    << setw(12) << "packets" << setw(12) << "flits" << endl;
    }
    for (unsigned int i = 0; i < chist.size(); i++) {
	out << " "
	    << setw(5) << chist[i].src_id
	    << setw(5) << curr_node
	    << setw(10) << getAverageDelay(chist[i].src_id)
	    << setw(10) << getMaxDelay(chist[i].src_id)
	    << setw(15) << getAverageThroughput(chist[i].src_id)
	    << setw(13) << getCommunicationEnergy(chist[i].src_id,
						  curr_node)
	    << setw(12) << chist[i].delays.size()
	    << setw(12) << chist[i].total_received_flits << endl;
    }

    out << "% Aggregated average delay (cycles): " << getAverageDelay() <<
	endl;
    out << "% Aggregated average throughput (flits/cycle): " <<
	getAverageThroughput() << endl;
}
