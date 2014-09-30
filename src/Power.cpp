/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the power model
 */

#include <iostream>
#include "Main.h"
#include "Power.h"
using namespace std;

double Power::pwr_buffering   = 0.0;
double Power::pwr_routing     = 0.0;
double Power::pwr_selection   = 0.0;
double Power::pwr_arbitration = 0.0;
double Power::pwr_crossbar    = 0.0;
double Power::pwr_link        = 0.0;
double Power::pwr_link_lv     = 0.0;
double Power::pwr_leakage     = 0.0;
double Power::pwr_end2end     = 0.0;

bool   Power::power_data_loaded = false;

Power::Power()
{
  pwr = 0.0;

  if (!power_data_loaded)
    {
      assert(LoadPowerData(GlobalParams::router_power_filename));
      power_data_loaded = true;
    }
}

void Power::Routing()
{
  pwr += pwr_routing;
}

void Power::Selection()
{
  pwr += pwr_selection;
}

void Power::Buffering()
{
  pwr += pwr_buffering;
}

void Power::Link(bool low_voltage)
{
  pwr += low_voltage ? pwr_link_lv : pwr_link;
}

void Power::Arbitration()
{
  pwr += pwr_arbitration;
}

void Power::Crossbar()
{
  pwr += pwr_crossbar;
}

void Power::Leakage()
{
  pwr += pwr_leakage;
}

void Power::EndToEnd()
{
  pwr += pwr_end2end;
}

bool Power::LoadPowerData(const char *fname)
{
  ifstream fin(fname, ios::in);

  if (!fin)
    return false;
  
  while (!fin.eof())
    {
      char line[1024];
      fin.getline(line, sizeof(line)-1);

      if (line[0] != '\0')
	{
	  if (line[0] != '#')
	    {
	      char   label[1024];
	      double value;
	      int params = sscanf(line, "%s %lf", label, &value);
	      if (params != 2)
		cerr << "WARNING: Invalid power data format: " << line << endl;
	      else
		{
		  if (strcmp(label, "PWR_ROUTING") == 0)
		    pwr_routing = value;
		  else if (strcmp(label, "PWR_BUFFERING") == 0)
		    pwr_buffering = value;
		  else if (strcmp(label, "PWR_SELECTION") == 0)
		    pwr_selection = value;
		  else if (strcmp(label, "PWR_ARBITRATION") == 0)
		    pwr_arbitration = value;
		  else if (strcmp(label, "PWR_CROSSBAR") == 0)
		    pwr_crossbar = value;
		  else if (strcmp(label, "PWR_LINK") == 0)
		    pwr_link = value;
		  else if (strcmp(label, "PWR_LINK_LV") == 0)
		    pwr_link_lv = value;
		  else if (strcmp(label, "PWR_LEAKAGE") == 0)
		    pwr_leakage = value;
		  else if (strcmp(label, "PWR_END2END") == 0)
		    pwr_end2end = value;
		  else
		    cerr << "WARNING: Invalid power label: " << label << endl;
		}
	    }
	}
    }

  fin.close();

  return true;
}
