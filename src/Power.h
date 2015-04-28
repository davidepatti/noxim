/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the power model
 */

#ifndef __NOXIMPOWER_H__
#define __NOXIMPOWER_H__

#include <cassert>
#include <map>
#include "DataStructs.h"

#include "yaml-cpp/yaml.h"

using namespace std;

/*

The average energy dissipated by a flit for a hop switch was estimated
as being 0.151nJ, 0.178nJ, 0.182nJ and 0.189nJ for XY, Odd-Even, DyAD,
and NoP-OE respectively

We assumed the tile size to be 2mm x 2mm and that the tiles were
arranged in a regular fashion on the floorplan. The load wire
capacitance was set to 0.50fF per micron, so considering an average of
25% switching activity the amount of energy consumed by a flit for a
hop interconnect is 0.384nJ.



#define PWR_ROUTING_XY             0.151e-9
#define PWR_ROUTING_WEST_FIRST     0.155e-9
#define PWR_ROUTING_NORTH_LAST     0.155e-9
#define PWR_ROUTING_NEGATIVE_FIRST 0.155e-9
#define PWR_ROUTING_ODD_EVEN       0.178e-9
#define PWR_ROUTING_DYAD           0.182e-9
#define PWR_ROUTING_FULLY_ADAPTIVE 0.0
#define PWR_ROUTING_TABLE_BASED    0.185e-9

#define PWR_SEL_RANDOM             0.002e-9
#define PWR_SEL_BUFFER_LEVEL       0.006e-9
#define PWR_SEL_NOP                0.012e-9

#define PWR_FORWARD_FLIT           0.384e-9
#define PWR_INCOMING               0.002e-9
#define PWR_STANDBY                0.0001e-9/2.0
*/

class Power {

  public:

    Power();


    void configureRouter(int link_width,
	                 int buffer_depth,
			 int buffer_size,
			 string routing_function,
			 string selection_function);

    void configureHub(int link_width,
	              int txrx_buffer_depth,
	              int txrx_buffer_size,
	              int buffer_depth,
		      int buffer_size);

    void bufferPush(); // x
    void bufferPop(); // x
    void bufferFront(); // x
    void antennaBufferPush();
    void antennaBufferPop();
    void antennaBufferFront();
    void wirelessTx(int src,int dst,int length);
    void wirelessRx();

    void routing(); //x 
    void selection(); // x
    void crossBar(); //x
    void link(); // x 
    void networkInterface(); //x

    void leakage();


    double getDynamicPower() {
	return total_power_d;
    } 
    double getStaticPower() {
	return total_power_s;
    } 
    double getTotalPower() {
	return (total_power_d + total_power_s);
    } 


  private:

    double total_power_d;
    double total_power_s;

    double buffer_push_pwr_d;
    double buffer_pop_pwr_d;
    double buffer_front_pwr_d;
    double buffer_pwr_s;
    
    double antenna_buffer_push_pwr_d;
    double antenna_buffer_pop_pwr_d;
    double antenna_buffer_front_pwr_d;
    double antenna_buffer_pwr_s;

    map< pair<int, int> , double>  bit_wireless_tx_pwr;
    double flit_wireless_rx_pwr;
    double transceiver_pwr_s;

    double routing_pwr_d;
    double routing_pwr_s;

    double selection_pwr_d;
    double selection_pwr_s;

    double crossbar_pwr_d;
    double crossbar_pwr_s;

    double link_pwr_d;
    double link_pwr_s;

    double ni_pwr_d;
    double ni_pwr_s;
   
    
};

#endif
