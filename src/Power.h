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
    void wirelessDynamicRx();
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

    double getTotalNIPower(){  
        return power_breakdown_d["ni_pwr_d"] + power_breakdown_s["ni_pwr_s"] ;
    }
    double getTotalLinkPower(){
        return power_breakdown_d["link_r2r_pwr_d"] + power_breakdown_s["link_r2r_pwr_s"];
    }
    
    /*Load improved link power model, Returns true if ok, false otherwise*/
    bool load_improved_link_model(const char *fname);
    void r2rImproved_link(int transition_array[]);
    
    
    /* Set and Get methods for instantPower 
     * Get : return the pair of the buffer at the index in parameter
     * Set : write in the buffer case the total instaneous current power of the router
     *       with the time in cycles when it's recorded 
     */
    pair<double,double> getInstantPower(const int index);
    void setInstantPower();
    void setTxPower(double val);
    void setRxPower(double val);
    void setLeakagePower(double val);
    void setIndexPower(int val);
    double getTxPower();
    double getRxPower();
    double getLeakagePower();
    int getIndexPower();
    double getbuffer_push_pwr_d(){ return buffer_push_pwr_d;}
    double getni_pwr_d(){ return ni_pwr_d;}
    double getbuffer_front_pwr_d(){ return buffer_front_pwr_d;}
    double getcrossbar_pwr_d(){ return crossbar_pwr_d;}
    double getbuffer_pop_pwr_d(){ return buffer_pop_pwr_d;}
    double getlink_r2r_pwr_d(){ return link_r2r_pwr_d;}
    
    int getFlitDir(){ return flitDir;}
    void setFlitDir(const int direction);
    double getLinksEnergy(const int index){return linksEnergy[index];}
        
    /*Get methods for total dynamic energy mapping*/
    double get_selection_pwr_d(){return  power_breakdown_d["selection_pwr_d"];}
    double get_routing_pwr_d(){return power_breakdown_d["routing_pwr_d"];}
    double get_crossbar_pwr_d(){return power_breakdown_d["crossbar_pwr_d"];}
    double get_buffer_push_pwr_d(){return power_breakdown_d["buffer_push_pwr_d"];}
    double get_buffer_pop_pwr_d(){return power_breakdown_d["buffer_pop_pwr_d"];}
    double get_buffer_front_pwr_d(){return power_breakdown_d["buffer_front_pwr_d"];}

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
    double transceiver_tx_pwr_s;
    double transceiver_rx_pwr_s;
    double transceiver_tx_pwr_biasing;
    double transceiver_rx_pwr_biasing;
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
    
    /*Erwan Moréac 07/07/15, Add of improved link power model
     *There are 22 transitions types */
    double improved_link_model_d[22];
    //Creation of a circular buffer of size 3 in order to record at each cycle the instaneous power
    //First is the time and Second the instaneous power
    pair<double,double>  instantPower[3];
    double TxPower;
    double RxPower;
    double LeakagePower;
    int IndexPower;
    //An array of 4 energy consumption value to know energy consumption on each link
    //[0]=NORTH [1]=EAST [2]=SOUTH [3]=WEST
    double linksEnergy[DIRECTIONS];
    //To know which direction the router is sending the flit
    int flitDir;

    map< pair<int, int> , double>  attenuation_map;
    double attenuation2power(double);


    void printBreakDown(string label, const map<string,double> & m,std::ostream & out) const;

    map<string,double> power_breakdown_d;
    map<string,double> power_breakdown_s;

    int sleep_end_cycle;

    bool isSleeping();

    
};

#endif

