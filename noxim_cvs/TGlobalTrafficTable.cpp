/*****************************************************************************

  TGlobalTrafficTable.cpp -- Global Traffic Table implementation

*****************************************************************************/
/* Copyright 2005-2007  
    Maurizio Palesi <mpalesi@diit.unict.it>
    Fabrizio Fazzino <fabrizio.fazzino@diit.unict.it>
    Davide Patti <dpatti@diit.unict.it>

 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "TGlobalTrafficTable.h"

//---------------------------------------------------------------------------

TGlobalTrafficTable::TGlobalTrafficTable()
{
}

//---------------------------------------------------------------------------

bool TGlobalTrafficTable::load(const char* fname)
{
  // Open file
  ifstream fin(fname, ios::in);
  if (!fin)
    return false;
  
  // Initialize variables
  traffic_table.clear();

  // Cycle reading file
  while (!fin.eof())
    {
      char line[512];
      fin.getline(line, sizeof(line)-1);

      if (line[0] != '\0')
	{
	  if (line[0] != '%')
	    {
	      int src, dst;  // Mandatory
	      float pir, por;
	      int t_on, t_off, t_period;

	      int params = sscanf(line, "%d %d %f %f %d %d %d", &src, &dst, &pir, &por, &t_on, &t_off, &t_period);
	      if (params >= 2)
		{
		  // Create a communication from the parameters read on the line
		  TCommunication communication;
	    
		  // Mandatory fields
		  communication.src = src;
		  communication.dst = dst;
	    
		  // Custom PIR
		  if (params>=3 && pir>=0 && pir<=1) communication.pir = pir;
		  else communication.pir = TGlobalParams::packet_injection_rate; 
	  
		  // Custom POR
		  if(params>=4 && por>=0 && por<=1) communication.por = por;
		  else communication.por = communication.pir; // TGlobalParams::probability_of_retransmission;
	  
		  // Custom Ton
		  if(params>=5 && t_on>=0) communication.t_on = t_on;
		  else communication.t_on = 0;
	  
		  // Custom Toff
		  if(params>=6 && t_off>=0) { assert(t_off>t_on); communication.t_off = t_off; }
		  else communication.t_off = DEFAULT_RESET_TIME + TGlobalParams::simulation_time;

		  // Custom Tperiod
		  if(params>=7 && t_period>0) { assert(t_period>t_off); communication.t_period = t_period; }
		  else communication.t_period = DEFAULT_RESET_TIME + TGlobalParams::simulation_time;

		  // Add this communication to the vector of communications
		  traffic_table.push_back(communication);
		}
	    }
	}
    }

  return true;
}

//---------------------------------------------------------------------------

double TGlobalTrafficTable::getCumulativePirPor(const int src_id, 
						const int ccycle,
						const bool pir_not_por,
						vector<pair<int,double> >& dst_prob)
{
  double cpirnpor = 0.0;

  dst_prob.clear();

  for (unsigned int i=0; i<traffic_table.size(); i++)
    {
      TCommunication comm = traffic_table[i];
      if (comm.src == src_id)
	{
	  int r_ccycle = ccycle % comm.t_period;
	  if (r_ccycle > comm.t_on && r_ccycle < comm.t_off)
	    {
	      cpirnpor += pir_not_por ? comm.pir : comm.por;
	      pair<int,double> dp(comm.dst, cpirnpor);
	      dst_prob.push_back(dp);
	    }
	}
    }

  return cpirnpor;
}

//---------------------------------------------------------------------------

int TGlobalTrafficTable::occurrencesAsSource(const int src_id)
{
  int count = 0;

  for (unsigned int i=0; i<traffic_table.size(); i++)
    if (traffic_table[i].src == src_id)
      count++;

  return count;
}

//---------------------------------------------------------------------------
