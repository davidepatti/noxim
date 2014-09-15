/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the global traffic table
 */

#include "NoximGlobalTrafficTable.h"

NoximGlobalTrafficTable::NoximGlobalTrafficTable()
{
}

bool NoximGlobalTrafficTable::load(const char *fname)
{
  // Open file
  ifstream fin(fname, ios::in);
  if (!fin)
    return false;

  // Initialize variables
  traffic_table.clear();

  // Cycle reading file
  while (!fin.eof()) {
    char line[512];
    fin.getline(line, sizeof(line) - 1);

    if (line[0] != '\0') {
      if (line[0] != '%') {
	int src, dst;	// Mandatory
	float pir, por;
	int t_on, t_off, t_period;
	int t_use_lvp;

	int params =
	  sscanf(line, "%d %d %d %f %f %d %d %d", &src, &dst, &t_use_lvp, &pir,
		 &por, &t_on, &t_off, &t_period);
	if (params >= 2) {
	  // Create a communication from the parameters read on the line
	  NoximCommunication communication;

	  // Mandatory fields
	  communication.src = src;
	  communication.dst = dst;

	  // Use low voltage path
	  if (params >= 3 && t_use_lvp == 1)
	    communication.use_low_voltage_path = true;
	  else
	    communication.use_low_voltage_path = false;

	  // Custom PIR
	  if (params >= 4 && pir >= 0 && pir <= 1)
	    communication.pir = pir;
	  else
	    communication.pir =
	      NoximGlobalParams::packet_injection_rate;

	  // Custom POR
	  if (params >= 5 && por >= 0 && por <= 1)
	    communication.por = por;
	  else
	    communication.por = communication.pir;	// NoximGlobalParams::probability_of_retransmission;

	  // Custom Ton
	  if (params >= 6 && t_on >= 0)
	    communication.t_on = t_on;
	  else
	    communication.t_on = 0;

	  // Custom Toff
	  if (params >= 7 && t_off >= 0) {
	    assert(t_off > t_on);
	    communication.t_off = t_off;
	  } else
	    communication.t_off =
	      DEFAULT_RESET_TIME +
	      NoximGlobalParams::simulation_time;

	  // Custom Tperiod
	  if (params >= 8 && t_period > 0) {
	    assert(t_period > t_off);
	    communication.t_period = t_period;
	  } else
	    communication.t_period =
	      DEFAULT_RESET_TIME +
	      NoximGlobalParams::simulation_time;

	  // Add this communication to the vector of communications
	  traffic_table.push_back(communication);
	}
      }
    }
  }

  return true;
}

double NoximGlobalTrafficTable::getCumulativePirPor(const int src_id,
						    const int ccycle,
						    const bool pir_not_por,
						    vector < pair < int, double > > &dst_prob,
						    vector <bool> &use_low_voltage_path)
{
  double cpirnpor = 0.0;

  dst_prob.clear();
  use_low_voltage_path.clear();

  for (unsigned int i = 0; i < traffic_table.size(); i++) {
    NoximCommunication comm = traffic_table[i];
    if (comm.src == src_id) {
      int r_ccycle = ccycle % comm.t_period;
      if (r_ccycle > comm.t_on && r_ccycle < comm.t_off) {
	cpirnpor += pir_not_por ? comm.pir : comm.por;
	pair < int, double >dp(comm.dst, cpirnpor);
	dst_prob.push_back(dp);
	use_low_voltage_path.push_back(comm.use_low_voltage_path);
      }
    }
  }

  return cpirnpor;
}

int NoximGlobalTrafficTable::occurrencesAsSource(const int src_id)
{
  int count = 0;

  for (unsigned int i = 0; i < traffic_table.size(); i++)
    if (traffic_table[i].src == src_id)
      count++;

  return count;
}
