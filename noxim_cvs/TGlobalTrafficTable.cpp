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

bool TGlobalTrafficTable::load(const char* fname)
{
  ifstream fin(fname, ios::in);

  if (!fin)
    return false;
  
  traffic_table.clear();

  bool stop = false;
  while (!fin.eof() && !stop)
    {
      char line[128];
      fin.getline(line, sizeof(line)-1);

      if (line[0] == '\0')
	stop = true;
      else
	{
	  if (line[0] != '%')
	    {
	      int node_id, in_src, in_dst, dst_id, out_src, out_dst;
	  
	      if (sscanf(line+1, "%d %d->%d %d", &node_id, &in_src, &in_dst, &dst_id) == 4)
		{
		  TLinkId lin(in_src, in_dst);
		  
		  char *pstr = line+COLUMN_AOC;
		  while (sscanf(pstr, "%d->%d", &out_src, &out_dst) == 2)
		    {
		      TLinkId lout(out_src, out_dst);
		      rt_noc[node_id][lin][dst_id].insert(lout);
		      
		      pstr = strstr(pstr, ",");
		      pstr++;
		    }
		}
	    }
	}
    }

  valid = true;
  
  return true;
}

//---------------------------------------------------------------------------
