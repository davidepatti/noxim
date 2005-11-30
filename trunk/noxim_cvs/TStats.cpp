#include "TStats.h"

// TODO: nan in averageDelay

//---------------------------------------------------------------------------

void TStats::setId(int node_id)
{
  id = node_id;
}

//---------------------------------------------------------------------------

void TStats::receivedFlit(const double arrival_time,
			  const TFlit& flit)
{
  if (flit.flit_type != FLIT_TYPE_HEAD) 
    return;

  int i = searchCommHistory(flit.src_id);
  if (i == -1)
    {
      CommHistory ch;

      ch.src_id = flit.src_id;
      ch.delays.push_back(arrival_time - flit.timestamp);
      chist.push_back(ch);
    }
  else
    chist[i].delays.push_back(arrival_time - flit.timestamp);
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
    avg += (double)chist[k].delays.size() * getAverageDelay(chist[k].src_id);
  
  return avg/(double)getReceivedPackets();
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

int TStats::searchCommHistory(int src_id)
{
  for (unsigned int i=0; i<chist.size(); i++)
    if (chist[i].src_id == src_id)
      return i;

  return -1;
}

//---------------------------------------------------------------------------

void TStats::showStats(std::ostream& out)
{
  out << "% node id: " << id << endl;
  for (unsigned int i=0; i<chist.size(); i++)
    {
      out << "% source id: " << chist[i].src_id 
	  << ", received " << chist[i].delays.size() << " packets"
	  << ", avg delay " << getAverageDelay(chist[i].src_id) 
	  << endl;
    }

  out << "% Agregated average delay " << getAverageDelay() << endl;
}

//---------------------------------------------------------------------------
