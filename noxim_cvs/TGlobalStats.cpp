#include "TGlobalStats.h"
#include "TTile.h"

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

  for (int y=0; y<MESH_DIM_Y; y++)
    for (int x=0; x<MESH_DIM_X; x++)
      {
	unsigned int received_packets = noc->t[x][y]->r->stats.getReceivedPackets(); 

	avg_delay += received_packets * noc->t[x][y]->r->stats.getAverageDelay();
	total_packets += received_packets;	
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

void TGlobalStats::showStats(std::ostream& out)
{
  for (int y=0; y<MESH_DIM_Y; y++)
    for (int x=0; x<MESH_DIM_X; x++)
      noc->t[x][y]->r->stats.showStats(out);

  out << "% Global average delay: " << getAverageDelay() << endl;
}

//---------------------------------------------------------------------------

