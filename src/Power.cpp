/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2015 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the power model
 */

#include <iostream>
#include "Power.h"
#include "Utils.h"
#include "systemc.h"

#define W2J(watt) ((watt)*GlobalParams::clock_period_ps*1.0e-12)

using namespace std;


Power::Power()
{
    total_power_s = 0.0;

    buffer_push_pwr_d = 0.0;
    buffer_pop_pwr_d = 0.0;
    buffer_front_pwr_d = 0.0;
    buffer_pwr_s = 0.0;
    
    antenna_buffer_push_pwr_d = 0.0;
    antenna_buffer_pop_pwr_d = 0.0;
    antenna_buffer_front_pwr_d = 0.0;
    antenna_buffer_pwr_s = 0.0;

    wireless_rx_pwr = 0.0;
    transceiver_tx_pwr_s = 0.0;
    transceiver_rx_pwr_s = 0.0;
    transceiver_tx_pwr_biasing = 0.0;
    transceiver_rx_pwr_biasing = 0.0;
    wireless_snooping = 0.0;

    routing_pwr_d = 0.0;
    routing_pwr_s = 0.0;

    selection_pwr_d = 0.0;
    selection_pwr_s = 0.0;

    crossbar_pwr_d = 0.0;
    crossbar_pwr_s = 0.0;

    link_r2r_pwr_d = 0.0;
    link_r2r_pwr_s = 0.0;
    link_r2h_pwr_s = 0.0;
    link_r2h_pwr_d = 0.0;

    default_tx_energy = 0.0;

    ni_pwr_d = 0.0;
    ni_pwr_s = 0.0;

    sleep_end_cycle = NOT_VALID;

    for(int i=0; i<22;i++)
        improved_link_model_d[i] = 0.0;
    
    for(int i=0;i<3;i++)
        instantPower[i] = make_pair(0.0,0.0);
    
    IndexPower = 0;
    TxPower = 0.0;
    RxPower = 0.0;
    LeakagePower = 0.0;
    
    for(int i=0; i<4;i++)
           linksEnergy[i] = 0.0;
    
    flitDir = 0;
}

void Power::configureRouter(int link_width,
	int buffer_depth,
	int buffer_item_size,
	string routing_function,
	string selection_function)
{
// (s)tatic, (d)ynamic power

    // Buffer 
    pair<int,int> key = pair<int,int>(buffer_depth, buffer_item_size);
    
    assert(GlobalParams::power_configuration.bufferPowerConfig.leakage.find(key) != GlobalParams::power_configuration.bufferPowerConfig.leakage.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.push.find(key) != GlobalParams::power_configuration.bufferPowerConfig.push.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.front.find(key) != GlobalParams::power_configuration.bufferPowerConfig.front.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.pop.find(key) != GlobalParams::power_configuration.bufferPowerConfig.pop.end());

    // Dynamic values are expressed in Joule
    // Static/Leakage values must be converted from Watt to Joule

    buffer_pwr_s = W2J(GlobalParams::power_configuration.bufferPowerConfig.leakage[key]);
    buffer_push_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.push[key];
    buffer_front_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.front[key];
    buffer_pop_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.pop[key];

    // Routing 
    assert(GlobalParams::power_configuration.routerPowerConfig.routing_algorithm_pm.find(routing_function) != GlobalParams::power_configuration.routerPowerConfig.routing_algorithm_pm.end());

    routing_pwr_s = W2J(GlobalParams::power_configuration.routerPowerConfig.routing_algorithm_pm[routing_function].first);
    routing_pwr_d = GlobalParams::power_configuration.routerPowerConfig.routing_algorithm_pm[routing_function].second;

    // Selection 
    assert(GlobalParams::power_configuration.routerPowerConfig.selection_strategy_pm.find(selection_function) != GlobalParams::power_configuration.routerPowerConfig.selection_strategy_pm.end());

    selection_pwr_s = W2J(GlobalParams::power_configuration.routerPowerConfig.selection_strategy_pm[selection_function].first);
    selection_pwr_d = GlobalParams::power_configuration.routerPowerConfig.selection_strategy_pm[selection_function].second;

    // CrossBar
    // TODO future work: tuning of crossbar radix
    pair<int,int> xbar_k = pair<int,int>(5,GlobalParams::flit_size);
    crossbar_pwr_s = W2J(GlobalParams::power_configuration.routerPowerConfig.crossbar_pm[xbar_k].first);
    crossbar_pwr_d = GlobalParams::power_configuration.routerPowerConfig.crossbar_pm[xbar_k].second;
    
    // NetworkInterface
    ni_pwr_s = W2J(GlobalParams::power_configuration.routerPowerConfig.network_interface[GlobalParams::flit_size].first);
    ni_pwr_d = GlobalParams::power_configuration.routerPowerConfig.network_interface[GlobalParams::flit_size].second;

    // Link 
    // Router has both type of links
    double length_r2h = GlobalParams::r2h_link_length;
    double length_r2r = GlobalParams::r2r_link_length;
    
    assert(GlobalParams::power_configuration.linkBitLinePowerConfig.find(length_r2r)!=GlobalParams::power_configuration.linkBitLinePowerConfig.end());
    assert(GlobalParams::power_configuration.linkBitLinePowerConfig.find(length_r2h)!=GlobalParams::power_configuration.linkBitLinePowerConfig.end());

    
    link_r2r_pwr_s= W2J(link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2r].first);
    link_r2r_pwr_d= link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2r].second;
    
    //Erwan 18/06/15 Control if we are using wireless NoC, if it's not the case there is no r2h link
    if (GlobalParams::use_winoc)
    {
    link_r2h_pwr_s= W2J(link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2h].first);
    link_r2h_pwr_d= link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2h].second;
    }else{
        link_r2h_pwr_s= 0.0;
        link_r2h_pwr_d= 0.0;    
    }
    
    //07/07/15 Improved Link power model
    load_improved_link_model(GlobalParams::interconnect_model_filename);
    
}

void Power::configureHub(int link_width,
	int buffer_depth,
	int buffer_item_size,
	int antenna_buffer_depth,
	int antenna_buffer_item_size,
	int data_rate_gbs)
{
// (s)tatic, (d)ynamic power

    // Buffer 
    pair<int,int> key = pair<int,int>(buffer_depth, buffer_item_size);
    
    assert(GlobalParams::power_configuration.bufferPowerConfig.leakage.find(key) != GlobalParams::power_configuration.bufferPowerConfig.leakage.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.push.find(key) != GlobalParams::power_configuration.bufferPowerConfig.push.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.front.find(key) != GlobalParams::power_configuration.bufferPowerConfig.front.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.pop.find(key) != GlobalParams::power_configuration.bufferPowerConfig.pop.end());

     //Erwan 18/06/15 Control if we are using wireless NoC, this is to avoid undesired leakage power
    if (GlobalParams::use_winoc)
    {
    buffer_pwr_s = W2J(GlobalParams::power_configuration.bufferPowerConfig.leakage[key]);
    buffer_push_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.push[key];
    buffer_front_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.front[key];
    buffer_pop_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.pop[key];
    }else{
        buffer_pwr_s = 0.0;
        buffer_push_pwr_d = 0.0;
        buffer_front_pwr_d = 0.0;
        buffer_pop_pwr_d = 0.0;    
    }
    
    
    // Buffer Antenna
    pair<int,int> akey = pair<int,int>(antenna_buffer_depth,antenna_buffer_item_size);
    
    assert(GlobalParams::power_configuration.bufferPowerConfig.leakage.find(akey) != GlobalParams::power_configuration.bufferPowerConfig.leakage.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.push.find(akey) != GlobalParams::power_configuration.bufferPowerConfig.push.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.front.find(akey) != GlobalParams::power_configuration.bufferPowerConfig.front.end());
    assert(GlobalParams::power_configuration.bufferPowerConfig.pop.find(akey) != GlobalParams::power_configuration.bufferPowerConfig.pop.end());

    antenna_buffer_pwr_s = W2J(GlobalParams::power_configuration.bufferPowerConfig.leakage[akey]);
    antenna_buffer_push_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.push[akey];
    antenna_buffer_front_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.front[akey];
    antenna_buffer_pop_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.pop[akey];

    attenuation_map = GlobalParams::power_configuration.hubPowerConfig.transmitter_attenuation_map;


    // TX
    // Joule
    default_tx_energy = (GlobalParams::power_configuration.hubPowerConfig.default_tx_energy / (10e9*data_rate_gbs) )* antenna_buffer_item_size;

    // RX Dynamic
    wireless_rx_pwr = antenna_buffer_item_size * GlobalParams::power_configuration.hubPowerConfig.rx_dynamic;
    
    // RX snooping
    wireless_snooping = GlobalParams::power_configuration.hubPowerConfig.rx_snooping;

    // TX leakage
    transceiver_tx_pwr_s = W2J(GlobalParams::power_configuration.hubPowerConfig.transceiver_leakage.first);
    // RX TX leakage
    transceiver_rx_pwr_s = W2J(GlobalParams::power_configuration.hubPowerConfig.transceiver_leakage.second);
   
    // TX biasing
    transceiver_tx_pwr_biasing = W2J(GlobalParams::power_configuration.hubPowerConfig.transceiver_biasing.first);
    // RX biasing
    transceiver_rx_pwr_biasing = W2J(GlobalParams::power_configuration.hubPowerConfig.transceiver_biasing.second);
    // Link 
    // Hub has only Router/Hub link connections
    double length_r2h = GlobalParams::r2h_link_length;
    assert(GlobalParams::power_configuration.linkBitLinePowerConfig.find(length_r2h)!=GlobalParams::power_configuration.linkBitLinePowerConfig.end());

    //Erwan 18/06/15 Control if we are using wireless NoC, this is to avoid undesired leakage power
    if (GlobalParams::use_winoc)
    {
    link_r2h_pwr_s= W2J(link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2h].first);
    link_r2h_pwr_d= link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2h].second;
    }else{
        link_r2h_pwr_s= 0.0;
        link_r2h_pwr_d= 0.0;   
    }
}


void Power::bufferPush()
{
    power_breakdown_d["buffer_push_pwr_d"] += buffer_push_pwr_d;
}

void Power::bufferPop()
{
    power_breakdown_d["buffer_pop_pwr_d"]+= buffer_pop_pwr_d;
}

void Power::bufferFront()
{
    power_breakdown_d["buffer_front_pwr_d"]+= buffer_front_pwr_d;
}

void Power::antennaBufferPush()
{
    power_breakdown_d["antenna_buffer_push_pwr_d"]+= antenna_buffer_push_pwr_d;
}

void Power::antennaBufferPop()
{
    power_breakdown_d["antenna_buffer_pop_pwr_d"]+= antenna_buffer_pop_pwr_d;
}

void Power::antennaBufferFront()
{
    power_breakdown_d["antenna_buffer_front_pwr_d"]+= antenna_buffer_front_pwr_d;
}

void Power::routing()
{
    power_breakdown_d["routing_pwr_d"]+= routing_pwr_d;
}

void Power::selection()
{
    power_breakdown_d["selection_pwr_d"]+=selection_pwr_d ;
}

void Power::crossBar()
{
    power_breakdown_d["crossbar_pwr_d"]+=crossbar_pwr_d;
}

void Power::r2rLink()
{
    power_breakdown_d["link_r2r_pwr_d"]+=link_r2r_pwr_d;
    //Update the link energy depending to the direction
    linksEnergy[flitDir] += link_r2r_pwr_d;
    
    if(GlobalParams::verbose_mode > VERBOSE_LOW)
    cout << "test" << linksEnergy[flitDir] << " dir is :"<< flitDir << endl;
}

void Power::r2hLink()
{
    power_breakdown_d["link_r2h_pwr_d"]+=link_r2h_pwr_d;
}

void Power::networkInterface()
{
    power_breakdown_d["ni_pwr_d"]+=ni_pwr_d;
}


double Power::getDynamicPower()
{
    double power = 0.0;
    for (map<string,double>::iterator i = power_breakdown_d.begin(); i!=power_breakdown_d.end(); i++)
    {
	//cout << " ABBIANDO " << i->first << " = " << i->second << endl;
	power+= i->second;
    }

    return power;
}

double Power::getStaticPower()
{
    double power = 0.0;
    for (map<string,double>::iterator i = power_breakdown_s.begin(); i!=power_breakdown_s.end(); i++)
	power+= i->second;

    return power;
}


pair<double,double> Power::getInstantPower(const int index)
{
    return instantPower[index];
}


void Power::setInstantPower()
{   
    double now = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
    //There is a language abuse, in fact it should be getTotalEnergy
    double power = (getTxPower() + getRxPower() + getLeakagePower())/(GlobalParams::clock_period_ps*1.0e-12);
    
    if (GlobalParams::verbose_mode > VERBOSE_HIGH)
    cout << "Total instant Power = " << power << endl;
    
    if(IndexPower > 2)
        IndexPower = 0;
        
    instantPower[IndexPower] = make_pair(now,power);
    IndexPower++;
    
}

void Power::setTxPower(double val)
{
        TxPower = val;
}

void Power::setRxPower(double val)
{   
        RxPower = val;
}

void Power::setLeakagePower(double val)
{
       LeakagePower = val;
}

void Power::setIndexPower(int val)
{    
        IndexPower = val;
}

void Power::setFlitDir(const int direction)
{
        flitDir = direction;   
}

double Power::getTxPower()
{
    return TxPower;
}

double Power::getRxPower()
{
    return RxPower;
}

double Power::getLeakagePower()
{
    return LeakagePower;
}

int Power::getIndexPower()
{
    return IndexPower;
}






double Power::attenuation2power(double attenuation)
{
    // TODO
    return attenuation;
}



void Power::wirelessTx(int src,int dst,int length)
{
    power_breakdown_d["wireless_tx"] += default_tx_energy;
    return;

    // TODO enable attenuation_map

    pair<int,int> key = pair<int,int>(src,dst);
    assert(attenuation_map.find(key)!=attenuation_map.end());

    power_breakdown_d["wireless_tx"] += attenuation2power(attenuation_map[key]) * length;
}

void Power::wirelessDynamicRx()
{
    if (!isSleeping())
	power_breakdown_d["wireless_dynamic_rx_pwr"]+= wireless_rx_pwr;
}

void Power::wirelessSnooping()
{
    if (!isSleeping())
	power_breakdown_d["wireless_snooping"] += wireless_snooping;
}

void Power::biasing()
{
    power_breakdown_s["transceiver_pwr_biasing"] += transceiver_tx_pwr_biasing;

    if (!isSleeping())
	power_breakdown_s["transceiver_pwr_biasing"] += transceiver_rx_pwr_biasing;
}

void Power::leakage()
{

    power_breakdown_s["buffer_pwr_s"]+=buffer_pwr_s;
    power_breakdown_s["antenna_buffer_pwr_s"]+=antenna_buffer_pwr_s;
    power_breakdown_s["routing_pwr_s"]+=routing_pwr_s;
    power_breakdown_s["selection_pwr_s"]+=selection_pwr_s;
    power_breakdown_s["crossbar_pwr_s"]+=crossbar_pwr_s;
    power_breakdown_s["link_r2r_pwr_s"]+=link_r2r_pwr_s;
    power_breakdown_s["link_r2h_pwr_s"]+=link_r2h_pwr_s;
    power_breakdown_s["transceiver_pwr_s"]+=transceiver_tx_pwr_s;
    
    double tmp = 0.0;
    tmp += buffer_pwr_s;
    tmp += antenna_buffer_pwr_s;
    tmp += routing_pwr_s;
    tmp += selection_pwr_s;
    tmp += crossbar_pwr_s;
    tmp += link_r2r_pwr_s;
    tmp += link_r2h_pwr_s;
    tmp += transceiver_tx_pwr_s;
        
    if (!isSleeping()){
	power_breakdown_s["transceiver_pwr_s"]+=transceiver_rx_pwr_s;
        tmp += transceiver_rx_pwr_s;
    }
    power_breakdown_s["ni_pwr_s"]+=ni_pwr_s;
    tmp += ni_pwr_s;
    
    LeakagePower = tmp;
    
}

void Power::printBreakDown(std::ostream & out)
{
    printMap("power_breakdown_d",power_breakdown_d,cout);
    printMap("power_breakdown_s",power_breakdown_s,cout);
}


void Power::rxSleep(int cycles)
{

    int sleep_start_cycle = (int)(sc_time_stamp().to_double()/GlobalParams::clock_period_ps);
    sleep_end_cycle = sleep_start_cycle + cycles;
}


bool Power::isSleeping()
{
    int now = (int)(sc_time_stamp().to_double()/GlobalParams::clock_period_ps);

    return (now<sleep_end_cycle);

}


bool Power::load_improved_link_model(const char *fname)
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
              double value2;
              double value3;
              double tmp = 0;
              double tmp2 = 0;
	      int params = sscanf(line, "%s %lf %lf %lf", label, &value, &value2, &value3);
	      if (params != 4)
		cerr << "WARNING: Invalid link power data format: " << line << endl;
	      else
		{ /* this function is called after yaml configuration so we
                   * know r2r_link_length*/
		  if (strcmp(label, "000") == 0){
                    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[0] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "010") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[1] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "100_001") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[2] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "101") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[3] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "110_011") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[4] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "111") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[5] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "H00_00H") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[6] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "H0H") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[7] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "H01_10H") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[8] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "H1H") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[9] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "H10_01H") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[10] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "H11_11H") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[11] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "L00_00L") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[12] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "L0L") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[13] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "L01_10L") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[14] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "L1L") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[15] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "L10_01L") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[16] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "11L_L11") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[17] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "L0H_H0L") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[18] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "L1H_H1L") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[19] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "xLx") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[20] = tmp + tmp2 + value3;
		  }else if (strcmp(label, "xHx") == 0){
		    tmp = value * GlobalParams::r2r_link_length * GlobalParams::r2r_link_length;
                    tmp2 = value2 * GlobalParams::r2r_link_length;
		    improved_link_model_d[21] = tmp + tmp2 + value3;
		  }else
		    cerr << "WARNING: Invalid link power label: " << label << endl;
		}
	    }
	}
    }

    //Display to check if the function is correct
  if(GlobalParams::verbose_mode > VERBOSE_LOW){
    for(int i=0;i<22;i++)
        cout << endl << "improved_link_model_d["<<i<<"] = "<< improved_link_model_d[i];
  
    cout << endl;
  }
  
  fin.close();

  return true;
}

 void Power::r2rImproved_link(int transition_array[])
 {
     double somme = 0.0;
     
     for(int i = 0;i < 22;i++)
         if(transition_array[i]!=0)
         somme += improved_link_model_d[i] * transition_array[i];
        
     power_breakdown_d["link_r2r_pwr_d"]+= somme;  
     
     //We must update here the instant power
     TxPower += somme;
     
     if(GlobalParams::verbose_mode > VERBOSE_LOW){
         cout<< "link power consumption is : " << somme << endl;
         cout<< "total link power consumption is : " << power_breakdown_d["link_r2r_pwr_d"] << endl;
     }
     
     //Update the link energy depending to the direction
     linksEnergy[flitDir] += somme;
 }
    


 
 

