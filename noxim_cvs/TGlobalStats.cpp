/*****************************************************************************

  TGlobalStats.cpp -- Global Statistics implementation

 *****************************************************************************/

#include <iomanip>
#include "TGlobalStats.h"

//---------------------------------------------------------------------------

TGlobalStats::TGlobalStats(const TNoC* _noc)
{
  noc = _noc;

#ifdef TESTING
  drained_total = 0;
#endif
}

//---------------------------------------------------------------------------

double TGlobalStats::getAverageDelay()
{
  unsigned int total_packets = 0;
  double       avg_delay     = 0.0;

  for (int y=0; y<TGlobalParams::mesh_dim_y; y++)
    for (int x=0; x<TGlobalParams::mesh_dim_x; x++)
      {
	unsigned int received_packets = noc->t[x][y]->r->stats.getReceivedPackets(); 

	if (received_packets)
	  {
	    avg_delay += received_packets * noc->t[x][y]->r->stats.getAverageDelay();
	    total_packets += received_packets;
	  }
      }

  avg_delay /= (double)total_packets;

  return avg_delay;  
}

//---------------------------------------------------------------------------

double TGlobalStats::getAverageDelay(const int src_id, const int dst_id)
{
  TTile* tile = noc->searchNode(dst_id);
  
  assert(tile != NULL);

  return tile->r->stats.getAverageDelay(src_id);
}

//---------------------------------------------------------------------------

double TGlobalStats::getMaxDelay()
{
  double maxd = -1.0;

  for (int y=0; y<TGlobalParams::mesh_dim_y; y++)
    for (int x=0; x<TGlobalParams::mesh_dim_x; x++)
      {
	TCoord coord;
	coord.x = x;
	coord.y = y;
	int node_id = coord2Id(coord);
	double d = getMaxDelay(node_id);
	if (d > maxd)
	  maxd = d;
      }

  return maxd;
}

//---------------------------------------------------------------------------

double TGlobalStats::getMaxDelay(const int node_id)
{
  TCoord coord = id2Coord(node_id);

  unsigned int received_packets = noc->t[coord.x][coord.y]->r->stats.getReceivedPackets(); 

  if (received_packets)
    return noc->t[coord.x][coord.y]->r->stats.getMaxDelay();
  else
    return -1.0;
}

//---------------------------------------------------------------------------

double TGlobalStats::getMaxDelay(const int src_id, const int dst_id)
{
  TTile* tile = noc->searchNode(dst_id);
  
  assert(tile != NULL);

  return tile->r->stats.getMaxDelay(src_id);
}

//---------------------------------------------------------------------------

vector<vector<double> > TGlobalStats::getMaxDelayMtx()
{
  vector<vector<double> > mtx;

  mtx.resize(TGlobalParams::mesh_dim_y);
  for (int y=0; y<TGlobalParams::mesh_dim_y; y++)
    mtx[y].resize(TGlobalParams::mesh_dim_x);

  for (int y=0; y<TGlobalParams::mesh_dim_y; y++)
    for (int x=0; x<TGlobalParams::mesh_dim_x; x++)
      {
	TCoord coord;
	coord.x = x;
	coord.y = y;
	int id = coord2Id(coord);
	mtx[y][x] = getMaxDelay(id);
      }

  return mtx;
}

//---------------------------------------------------------------------------

double TGlobalStats::getAverageThroughput(const int src_id, const int dst_id)
{
  TTile* tile = noc->searchNode(dst_id);
  
  assert(tile != NULL);

  return tile->r->stats.getAverageThroughput(src_id);
}

//---------------------------------------------------------------------------

double TGlobalStats::getAverageThroughput()
{
  unsigned int total_comms    = 0;
  double       avg_throughput = 0.0;

  for (int y=0; y<TGlobalParams::mesh_dim_y; y++)
    for (int x=0; x<TGlobalParams::mesh_dim_x; x++)
      {
	unsigned int ncomms = noc->t[x][y]->r->stats.getTotalCommunications(); 

	if (ncomms)
	  {
	    avg_throughput += ncomms * noc->t[x][y]->r->stats.getAverageThroughput();
	    total_comms += ncomms;
	  }
      }

  avg_throughput /= (double)total_comms;

  return avg_throughput;
}

//---------------------------------------------------------------------------

unsigned int TGlobalStats::getReceivedPackets()
{
  unsigned int n = 0;

  for (int y=0; y<TGlobalParams::mesh_dim_y; y++)
    for (int x=0; x<TGlobalParams::mesh_dim_x; x++)
      n += noc->t[x][y]->r->stats.getReceivedPackets();

  return n;
}

//---------------------------------------------------------------------------

unsigned int TGlobalStats::getReceivedFlits()
{
  unsigned int n = 0;

  for (int y=0; y<TGlobalParams::mesh_dim_y; y++)
    for (int x=0; x<TGlobalParams::mesh_dim_x; x++)
    {
       n += noc->t[x][y]->r->stats.getReceivedFlits();
#ifdef TESTING
       drained_total+= noc->t[x][y]->r->local_drained;
#endif
    }

  return n;
}

//---------------------------------------------------------------------------

double TGlobalStats::getThroughput()
{
  int total_cycles = TGlobalParams::simulation_time - 
    TGlobalParams::stats_warm_up_time;


  //  int number_of_ip = TGlobalParams::mesh_dim_x * TGlobalParams::mesh_dim_y;
  //  return (double)getReceivedFlits()/(double)(total_cycles * number_of_ip);

  unsigned int n   = 0;
  unsigned int trf = 0;
  for (int y=0; y<TGlobalParams::mesh_dim_y; y++)
    for (int x=0; x<TGlobalParams::mesh_dim_x; x++)
      {
	unsigned int rf = noc->t[x][y]->r->stats.getReceivedFlits();

	if (rf != 0)
	  n++;

	trf += rf;
      }
  return (double)trf/(double)(total_cycles * n);

}

//---------------------------------------------------------------------------

vector<vector<unsigned long> > TGlobalStats::getRoutedFlitsMtx()
{

  vector<vector<unsigned long> > mtx;

  mtx.resize(TGlobalParams::mesh_dim_y);
  for (int y=0; y<TGlobalParams::mesh_dim_y; y++)
    mtx[y].resize(TGlobalParams::mesh_dim_x);

  for(int y=0; y<TGlobalParams::mesh_dim_y; y++)
    for(int x=0; x<TGlobalParams::mesh_dim_x; x++)
      mtx[y][x] = noc->t[x][y]->r->getRoutedFlits();

  return mtx;
}

//---------------------------------------------------------------------------

double TGlobalStats::getPower()
{
  double power = 0.0;

  for(int y=0; y<TGlobalParams::mesh_dim_y; y++)
    for(int x=0; x<TGlobalParams::mesh_dim_x; x++)
      power += noc->t[x][y]->r->getPower();

  return power;
}

//---------------------------------------------------------------------------

void TGlobalStats::showStats(std::ostream& out, bool detailed)
{
  if(TGlobalParams::verbose_mode > VERBOSE_OFF)
    for (int y=0; y<TGlobalParams::mesh_dim_y; y++)
      for (int x=0; x<TGlobalParams::mesh_dim_x; x++)
        noc->t[x][y]->r->stats.showStats(out);

  out << "% Total received packets: " << getReceivedPackets() << endl;
  out << "% Total received flits: " << getReceivedFlits() << endl;
  out << "% Global average delay (cycles): " << getAverageDelay() << endl;
  out << "% Global average throughput (flits/cycle): " << getAverageThroughput() << endl;
  out << "% Throughput (flits/cycle/IP): " << getThroughput() << endl;
  out << "% Max delay (cycles): " << getMaxDelay() << endl;
  out << "% Total energy (J): " << getPower() << endl;

  if (detailed)
    {
      // show MaxDelay matrix
      vector<vector<double> > md_mtx = getMaxDelayMtx();

      out << endl << "max_delay = [" << endl;
      for (unsigned int y=0; y<md_mtx.size(); y++)
	{
	  out << "   ";
	  for (unsigned int x=0; x<md_mtx[y].size(); x++)
	    out << setw(6) << md_mtx[y][x];
	  out << endl;
	}
      out << "];" << endl;

      // show RoutedFlits matrix
      vector<vector<unsigned long> > rf_mtx = getRoutedFlitsMtx();

      out << endl << "routed_flits = [" << endl;
      for (unsigned int y=0; y<rf_mtx.size(); y++)
	{
	  out << "   ";
	  for (unsigned int x=0; x<rf_mtx[y].size(); x++)
	    out << setw(10) << rf_mtx[y][x];
	  out << endl;
	}
      out << "];" << endl;
    }
}

//---------------------------------------------------------------------------

