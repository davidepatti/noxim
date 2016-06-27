/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2015 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementaton of the global statistics
 */

#include "GlobalStats.h"
#include "ConfigurationManager.h"

using namespace std;

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

    for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < GlobalParams::mesh_dim_x; x++) {
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

    for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < GlobalParams::mesh_dim_x; x++) {
	    Coord coord;
	    coord.x = x;
	    coord.y = y;
	    int node_id = coord2Id(coord);
	    double d = getMaxDelay(node_id);
	    if (d > maxd)
		maxd = d;
	}

    return maxd;
}

double GlobalStats::getMaxDelay(const int node_id)
{
    Coord coord = id2Coord(node_id);

    unsigned int received_packets =
	noc->t[coord.x][coord.y]->r->stats.getReceivedPackets();

    if (received_packets)
	return noc->t[coord.x][coord.y]->r->stats.getMaxDelay();
    else
	return -1.0;
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

    mtx.resize(GlobalParams::mesh_dim_y);
    for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
	mtx[y].resize(GlobalParams::mesh_dim_x);

    for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < GlobalParams::mesh_dim_x; x++) {
	    Coord coord;
	    coord.x = x;
	    coord.y = y;
	    int id = coord2Id(coord);
	    mtx[y][x] = getMaxDelay(id);
	}

    return mtx;
}

double GlobalStats::getAverageThroughput(const int src_id,
					      const int dst_id)
{
    Tile *tile = noc->searchNode(dst_id);

    assert(tile != NULL);

    return tile->r->stats.getAverageThroughput(src_id);
}

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

unsigned int GlobalStats::getReceivedPackets()
{
    unsigned int n = 0;

    for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < GlobalParams::mesh_dim_x; x++)
	    n += noc->t[x][y]->r->stats.getReceivedPackets();

    return n;
}

unsigned int GlobalStats::getReceivedFlits()
{
    unsigned int n = 0;

    for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < GlobalParams::mesh_dim_x; x++) {
	    n += noc->t[x][y]->r->stats.getReceivedFlits();
#ifdef TESTING
	    drained_total += noc->t[x][y]->r->local_drained;
#endif
	}

    return n;
}

double GlobalStats::getThroughput()
{
    //EM change int type to double
    
    double total_cycles =
	GlobalParams::simulation_time -
	(double)GlobalParams::stats_warm_up_time;

    //  int number_of_ip = GlobalParams::mesh_dim_x * GlobalParams::mesh_dim_y;
    //  return (double)getReceivedFlits()/(double)(total_cycles * number_of_ip);

    unsigned int n = 0;
    unsigned int trf = 0;
    for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < GlobalParams::mesh_dim_x; x++) {
	    unsigned int rf = noc->t[x][y]->r->stats.getReceivedFlits();

	    if (rf != 0)
		n++;

	    trf += rf;
	}
    return (double) trf / (double) (total_cycles * n);

}

vector < vector < unsigned long > > GlobalStats::getRoutedFlitsMtx()
{

    vector < vector < unsigned long > > mtx;

    mtx.resize(GlobalParams::mesh_dim_y);
    for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
	mtx[y].resize(GlobalParams::mesh_dim_x);

    for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < GlobalParams::mesh_dim_x; x++)
	    mtx[y][x] = noc->t[x][y]->r->getRoutedFlits();

    return mtx;
}

double GlobalStats::getDynamicPower()
{
    double power = 0.0;

    // Electric noc
    for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < GlobalParams::mesh_dim_x; x++)
	    power += noc->t[x][y]->r->power.getDynamicPower();

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

    for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < GlobalParams::mesh_dim_x; x++)
	    power += noc->t[x][y]->r->power.getStaticPower();

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
    if(!GlobalParams::use_script_mode){ 
 
    // Show configuration
    if (GlobalParams::detailed)
    showConfig();
        
    out << "% Total received packets: " << getReceivedPackets() << endl;
    out << "% Total received flits: " << getReceivedFlits() << endl;
    out << "% Global average delay (cycles): " << getAverageDelay() <<
	endl;
    out << "% Global average throughput (flits/cycle): " <<
	getAverageThroughput() << endl;
    out << "% Throughput (flits/cycle/IP): " << getThroughput() << endl;
    out << "% Max delay (cycles): " << getMaxDelay() << endl;
    out << "% Total energy (J): " << getTotalPower() << endl;
    out << "% \tDynamic energy (J): " << getDynamicPower() << endl;
    out << "% \tStatic energy (J): " << getStaticPower() << endl;
    out << "% Execution time (s): " << GlobalParams::texec << endl;

    if (GlobalParams::show_buffer_stats)
      showBufferStats(out);

    if (detailed) {
	out << endl << "detailed = [" << endl;
	for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
	    for (int x = 0; x < GlobalParams::mesh_dim_x; x++)
		noc->t[x][y]->r->stats.showStats(y *
						 GlobalParams::
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

	showPowerBreakDown(out);
    }
            
    }else
        
        //In script mode, we need PowerBreakDown's data
        showPowerBreakDown(out);
    
  
}

void GlobalStats::updatePowerBreakDown(map<string,double> &dst,const map<string,double>& src)
{
    for (map<string,double>::const_iterator i=src.begin();i!=src.end();i++)
    {
	dst[i->first]+=i->second;
    }
}


void GlobalStats::showPowerBreakDown(std::ostream & out)
{
    map<string,double> power_breakdown_d;
    map<string,double> power_breakdown_s;

    for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < GlobalParams::mesh_dim_x; x++)
	{
	    updatePowerBreakDown(power_breakdown_d, 
		noc->t[x][y]->r->power.getDynamicPowerBreakDown());

	    updatePowerBreakDown(power_breakdown_s, 
		noc->t[x][y]->r->power.getStaticPowerBreakDown());
	}

    for (map<int, HubConfig>::iterator it = GlobalParams::hub_configuration.begin();
            it != GlobalParams::hub_configuration.end();
            ++it)
    {
	int hub_id = it->first;

	map<int,Hub*>::const_iterator i = noc->hub.find(hub_id);
	Hub * h = i->second;

	    updatePowerBreakDown(power_breakdown_d, 
		h->power.getDynamicPowerBreakDown());

	    updatePowerBreakDown(power_breakdown_s, 
		h->power.getStaticPowerBreakDown());
    }

    if(!GlobalParams::use_script_mode){
    
    printMap("power_breakdown_d",power_breakdown_d,out);
    printMap("power_breakdown_s",power_breakdown_s,out);
    
    cout << endl;
    //Dynamic Energy Mapping of the NOC
    double totalLink = 0.0, oneLinkEnergy = 0.0, routerEnergy = 0.0;
    for (int y = 0; y < GlobalParams::mesh_dim_y; y++){
	    for (int x = 0; x < GlobalParams::mesh_dim_x; x++){
                totalLink = 0.0;
		for(int dir = 0; dir < DIRECTIONS; dir++){
                    oneLinkEnergy = noc->t[x][y]->r->power.getLinksEnergy(dir);
                    totalLink += oneLinkEnergy;
                    cout  << oneLinkEnergy << "\t";
                }
                //Dynamic energy
                routerEnergy = noc->t[x][y]->r->power.get_selection_pwr_d() 
                             + noc->t[x][y]->r->power.get_routing_pwr_d()
                             + noc->t[x][y]->r->power.get_crossbar_pwr_d() 
                             + noc->t[x][y]->r->power.get_buffer_front_pwr_d()
                             + noc->t[x][y]->r->power.get_buffer_pop_pwr_d() 
                             + noc->t[x][y]->r->power.get_buffer_push_pwr_d();
                //routerEnergy = noc->t[x][y]->r->power.getTotalPower() - totalLink; // Total energy consumption
                cout << " Router : " << routerEnergy << endl;
            }
    }                

    } else {
        //Data no 1 to 9
        cout << getReceivedPackets() << " " << getReceivedFlits() << " " << getAverageDelay() << " " << getAverageThroughput() 
        << " " << getThroughput() << " " << getMaxDelay() << " " << getTotalPower() << " " << getDynamicPower() << " " << getStaticPower() << " ";
        //Data no 10 to 17
        for (map<string,double>::const_iterator i = power_breakdown_d.begin();i!=power_breakdown_d.end();i++)
	cout << i->second << " ";
        //Data no 18 to 26
        for (map<string,double>::const_iterator i = power_breakdown_s.begin();i!=power_breakdown_s.end();i++)
	cout << i->second << " ";
        //Data no 27
        cout << GlobalParams::texec << " ";
        //Data no 28 to [28 + (NoC_size * 5)] Mapping dynamic energy consumption so 1 router + 4 directions
        //Example for the order : N E S W Router0 N E S W Router1 etc...
        double oneLinkEnergy = 0.0, routerEnergy = 0.0;
        for (int y = 0; y < GlobalParams::mesh_dim_y; y++){
            for (int x = 0; x < GlobalParams::mesh_dim_x; x++){
                for(int dir = 0; dir < DIRECTIONS; dir++){
                    oneLinkEnergy = noc->t[x][y]->r->power.getLinksEnergy(dir);
                    cout  << oneLinkEnergy << " ";
                }
                routerEnergy = noc->t[x][y]->r->power.get_selection_pwr_d() 
                              + noc->t[x][y]->r->power.get_routing_pwr_d() 
                              + noc->t[x][y]->r->power.get_crossbar_pwr_d()
                              + noc->t[x][y]->r->power.get_buffer_push_pwr_d()
                              + noc->t[x][y]->r->power.get_buffer_pop_pwr_d()
                              + noc->t[x][y]->r->power.get_buffer_front_pwr_d();
                
                cout << routerEnergy << " ";
            }
        }
        cout << endl;
    }
}



void GlobalStats::showBufferStats(std::ostream & out)
{
  out << "Router id\tBuffer N\t\tBuffer E\t\tBuffer S\t\tBuffer W\t\tBuffer L" << endl;
  out << "         \tMean\tMax\tMean\tMax\tMean\tMax\tMean\tMax\tMean\tMax" << endl;
  for (int y = 0; y < GlobalParams::mesh_dim_y; y++)
    for (int x = 0; x < GlobalParams::mesh_dim_x; x++)
      {
	out << noc->t[x][y]->r->local_id;
	noc->t[x][y]->r->ShowBuffersStats(out);
	out << endl;
      }
}
