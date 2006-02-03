/*****************************************************************************

  TGlobalTrafficTable.cpp -- Global Traffic Table implementation

*****************************************************************************/

#include "TGlobalTrafficTable.h"

//---------------------------------------------------------------------------

TGlobalTrafficTable::TGlobalTrafficTable()
{
  valid = false;
  numberOfLines = 0;
}

//---------------------------------------------------------------------------

int TGlobalTrafficTable::occurrencesAsSource(const int id)
{
  int count=0;
  for(unsigned int i=0; i<traffic_table.size(); i++)
  {
    if(traffic_table[i].src == id) count++;
  }
  return count;
}

//---------------------------------------------------------------------------

int TGlobalTrafficTable::randomDestinationGivenTheSource(const int src)
{
  assert(isValid());

  vector<int> possible_destinations;
  for(unsigned int i=0; i<traffic_table.size(); i++)
  {
    if(traffic_table[i].src == src)
      possible_destinations.push_back(traffic_table[i].dst);
  }

  //assert(possible_destinations.size()>0);
  int ndests = possible_destinations.size();

  return (ndests) ? possible_destinations[rand() % ndests] : NOT_VALID;
}

//---------------------------------------------------------------------------

float TGlobalTrafficTable::getPirForTheSelectedLink(int src_id, int dst_id)
{
  for(unsigned int i=0; i<traffic_table.size(); i++)
  {
    if(traffic_table[i].src==src_id && traffic_table[i].dst==dst_id)
      return traffic_table[i].pir;
  }

  return 0;
}

//---------------------------------------------------------------------------

float TGlobalTrafficTable::getPorForTheSelectedLink(int src_id, int dst_id)
{
  for(unsigned int i=0; i<traffic_table.size(); i++)
  {
    if(traffic_table[i].src==src_id && traffic_table[i].dst==dst_id)
      return traffic_table[i].por;
  }

  return 0;
}

//---------------------------------------------------------------------------

bool TGlobalTrafficTable::load(const char* fname)
{
  // Open file
  ifstream fin(fname, ios::in);
  if(!fin)
    return false;
  
  // Initialize variables
  traffic_table.clear();
  bool stop = false;

  // Cycle reading file
  while(!fin.eof() && !stop)
  {
    char line[128];
    fin.getline(line, sizeof(line)-1);

    if (line[0] == '\0')
      stop = true;
    else
    {
      if (line[0] != '%')
      {
        int src, dst;  // Mandatory
        float pir = TGlobalParams::packet_injection_rate;
        float por = TGlobalParams::probability_of_retransmission;
        int t_on = DEFAULT_RESET_TIME;
        int t_off = DEFAULT_RESET_TIME+TGlobalParams::simulation_time;
        int t_period = 0;  // Means no periodicity

        if (sscanf(line, "%d %d %f %f %d %d %d", &src, &dst, &pir, &por, &t_on, &t_off, &t_period) >= 2)
        {
          numberOfLines++;

          // Create a link from the parameters read on the line
          TLocalTrafficLink link;
          link.src = src;
          link.dst = dst;
          link.pir = pir;
          link.por = por;
          assert(t_off>t_on);
          link.por = t_on;
          link.por = t_off;
          if(t_period!=0) assert(t_period>t_on+t_off);
          link.por = t_period;

          // Add this link to the vector of links
          traffic_table.push_back(link);
	}
      }
    }
  }

  valid = true;
  return true;
}

//---------------------------------------------------------------------------

