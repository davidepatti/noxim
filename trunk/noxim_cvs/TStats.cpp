/*****************************************************************************

  TStats.cpp -- Statistics implementation

 *****************************************************************************/

#include <iostream>
#include <iomanip>
#include "TStats.h"
// TODO: nan in averageDelay


//---------------------------------------------------------------------------

void TStats::configure(const int node_id, const double _warm_up_time)
{
  id = node_id;
  warm_up_time = _warm_up_time;
}

//---------------------------------------------------------------------------

void TStats::receivedFlit(const double arrival_time,
			  const TFlit& flit)
{
  if (arrival_time - DEFAULT_RESET_TIME < warm_up_time)
    return;

  int i = searchCommHistory(flit.src_id);
  
  if (i == -1)
    {
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

//---------------------------------------------------------------------------

double TStats::getAverageDelay(const int src_id)
{
  double sum = 0.0;
  
  int i = searchCommHistory(src_id);

  assert(i >= 0);

  for (unsigned int j=0; j<chist[i].delays.size(); j++)
    sum += chist[i].delays[j];

  return sum/(double)chist[i].delays.size();
}

//---------------------------------------------------------------------------

double TStats::getAverageDelay()
{
  double avg = 0.0;

  for (unsigned int k=0; k<chist.size(); k++)
    {
      unsigned int samples = chist[k].delays.size();
      if (samples)
	avg += (double)samples * getAverageDelay(chist[k].src_id);
    }

  return avg/(double)getReceivedPackets();
}

//---------------------------------------------------------------------------

double TStats::getMaxDelay(const int src_id)
{
  double maxd = -1.0;
  
  int i = searchCommHistory(src_id);

  assert(i >= 0);

  for (unsigned int j=0; j<chist[i].delays.size(); j++)
    if (chist[i].delays[j] > maxd)
      {
	//	cout << src_id << " -> " << id << ": " << chist[i].delays[j] << endl;
	maxd = chist[i].delays[j];
      }
  return maxd;
}

//---------------------------------------------------------------------------

double TStats::getMaxDelay()
{
  double maxd = -1.0;

  for (unsigned int k=0; k<chist.size(); k++)
    {
      unsigned int samples = chist[k].delays.size();
      if (samples)
	{
	  double m = getMaxDelay(chist[k].src_id);
	  if (m > maxd)
	    maxd = m;
	}
    }

  return maxd;
}

//---------------------------------------------------------------------------

double TStats::getAverageThroughput(const int src_id)
{
  int i = searchCommHistory(src_id);

  assert(i >= 0);

  if (chist[i].total_received_flits == 0)
    return -1.0;
  else
    return (double)chist[i].total_received_flits/(double)chist[i].last_received_flit_time;
}

//---------------------------------------------------------------------------

double TStats::getAverageThroughput()
{
  double sum = 0.0;

  for (unsigned int k=0; k<chist.size(); k++)
    {
      double avg = getAverageThroughput(chist[k].src_id);
      if (avg > 0.0)
	sum += avg;
    }

  return sum;
}

//---------------------------------------------------------------------------

unsigned int TStats::getReceivedPackets()
{
  int n = 0;

  for (unsigned int i=0; i<chist.size(); i++)
    n += chist[i].delays.size();

  return n;
}

//---------------------------------------------------------------------------

unsigned int TStats::getReceivedFlits()
{
  int n = 0;

  for (unsigned int i=0; i<chist.size(); i++)
    n += chist[i].total_received_flits;

  return n;
}

//---------------------------------------------------------------------------

unsigned int TStats::getTotalCommunications()
{
  return chist.size();
}

//---------------------------------------------------------------------------

int TStats::searchCommHistory(int src_id)
{
  for (unsigned int i=0; i<chist.size(); i++)
    if (chist[i].src_id == src_id)
      return i;

  return -1;
}

//---------------------------------------------------------------------------

void TStats::showStats(int curr_node,
		       std::ostream& out,
		       bool header)
{
  if (header)
    {
      out << "%" 
	  << setw(5)  << "src"
	  << setw(5)  << "dst"
	  << setw(10) << "delay avg"
	  << setw(10) << "delay max"
	  << setw(15) << "throughput"
	  << setw(12) << "received"
	  << setw(12) << "received" 
	  << endl;
      out << "%" 
	  << setw(5)  << ""
	  << setw(5)  << ""
	  << setw(10) << "cycles"
	  << setw(10) << "cycles"
	  << setw(15) << "flits/cycle"
	  << setw(12) << "packets"
	  << setw(12) << "flits" 
	  << endl;
    }
  for (unsigned int i=0; i<chist.size(); i++)
    {
      out << " "
	  << setw(5)  << chist[i].src_id
	  << setw(5)  << curr_node
	  << setw(10) << getAverageDelay(chist[i].src_id)
	  << setw(10) << getMaxDelay(chist[i].src_id)
	  << setw(15) << getAverageThroughput(chist[i].src_id)
	  << setw(12) << chist[i].delays.size()
	  << setw(12) << chist[i].total_received_flits
	  << endl;
    }
  
  out << "% Aggregated average delay (cycles): " << getAverageDelay() << endl;
  out << "% Aggregated average throughput (flits/cycle): " << getAverageThroughput() << endl;
}

//---------------------------------------------------------------------------
