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
  assert(possible_destinations.size()>0);
  return possible_destinations[rand()%possible_destinations.size()];
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
        int src, dst;
        float pir = TGlobalParams::packet_injection_rate;
        if (sscanf(line, "%d %d %f", &src, &dst, &pir) >= 2)
        {
          numberOfLines++;

          // Create a link from the parameters read on the line
          TLocalTrafficLink link;
          link.src = src;
          link.dst = dst;
          link.pir = pir;

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

