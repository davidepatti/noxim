/*****************************************************************************

  TGlobalStats.cpp -- Global Statistics implementation

 *****************************************************************************/

#include "TGlobalStats.h"

//---------------------------------------------------------------------------

TGlobalStats::TGlobalStats(const TNoC* _noc)
{
  noc = _noc;
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
      n += noc->t[x][y]->r->stats.getReceivedFlits();

  return n;
}

//---------------------------------------------------------------------------

double TGlobalStats::getThroughput()
{
  int total_cycles = TGlobalParams::simulation_time - 
    TGlobalParams::stats_warm_up_time;

  int number_of_ip = TGlobalParams::mesh_dim_x * TGlobalParams::mesh_dim_y;

  return (double)getReceivedFlits()/(double)(total_cycles * number_of_ip);
}

//---------------------------------------------------------------------------

void TGlobalStats::showStats(std::ostream& out)
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
}

//---------------------------------------------------------------------------

