/*****************************************************************************

  TGlobalTrafficTable.cpp -- Global Traffic Table implementation

*****************************************************************************/

#include "TGlobalTrafficTable.h"

//---------------------------------------------------------------------------

TGlobalTrafficTable::TGlobalTrafficTable()
{
  valid = false;
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
  return possible_destinations[rand()%possible_destinations.size()];
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
        int src, dst, traffic=TRAFFIC_UNIFORM;
        if (sscanf(line, "%d %d %d", &src, &dst, &traffic) >= 2)
        {
          // Create a link from the parameters read on the line
          TLocalTrafficLink link;
          link.src = src;
          link.dst = dst;
          link.traffic = traffic;

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
