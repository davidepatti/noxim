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


class Power {

  public:

    Power();


    void configureRouter(int link_width,
	                 int buffer_depth,
			 int buffer_item_size,
			 string routing_function,
			 string selection_function);

    void configureHub(int link_width, 
	              int buffer_depth, 
		      int buffer_item_size, 
		      int antenna_buffer_depth, 
		      int antenna_buffer_item_size, 
		      int data_rate_gbs);

    void bufferPush(); 
    void bufferPop(); 
    void bufferFront(); 
    void antennaBufferPush();
    void antennaBufferPop();
    void antennaBufferFront(); 
    void wirelessTx(int src,int dst,int length);
    void wirelessDynamicRx(int no_receivers);
    void wirelessSnooping();

    void routing();
    void selection(); 
    void crossBar(); 
    void r2hLink(); 
    void r2rLink(); 
    void networkInterface();

    void leakage();
    void biasing();

    double getDynamicPower();
    double getStaticPower();

    double getTotalPower() {
	return (getDynamicPower() + getStaticPower());
    } 


    void printBreakDown(std::ostream & out);
    map<string,double> getDynamicPowerBreakDown(){ return power_breakdown_d;}
    map<string,double> getStaticPowerBreakDown(){ return power_breakdown_s;}

    void rxSleep(int cycles);

  private:

    double total_power_s;

    double buffer_push_pwr_d;
    double buffer_pop_pwr_d;
    double buffer_front_pwr_d;
    double buffer_pwr_s;
    
    double antenna_buffer_push_pwr_d;
    double antenna_buffer_pop_pwr_d;
    double antenna_buffer_front_pwr_d;
    double antenna_buffer_pwr_s;

    double wireless_rx_pwr;
    double transceiver_pwr_s;
    double transceiver_pwr_biasing;
    double wireless_snooping;

    double default_tx_energy;

    double routing_pwr_d;
    double routing_pwr_s;

    double selection_pwr_d;
    double selection_pwr_s;

    double crossbar_pwr_d;
    double crossbar_pwr_s;

    double link_r2r_pwr_d;
    double link_r2r_pwr_s;
    double link_r2h_pwr_s;
    double link_r2h_pwr_d;

    double ni_pwr_d;
    double ni_pwr_s;

    map< pair<int, int> , double>  attenuation_map;
    double attenuation2power(double);


    void printBreakDown(string label, const map<string,double> & m,std::ostream & out) const;

    map<string,double> power_breakdown_d;
    map<string,double> power_breakdown_s;

    int sleep_end_cycle;

    bool isSleeping();

    
};

#endif
