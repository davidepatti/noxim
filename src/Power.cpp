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
#include "Power.h"
using namespace std;


Power::Power()
{
   total_power_d = 0.0;
   total_power_s = 0.0;

   buffer_pwr_s = 0.0;
   antenna_buffer_pwr_s = 0.0;
   routing_pwr_s = 0.0;
   selection_pwr_s = 0.0;
   crossbar_pwr_s = 0.0;
   link_pwr_s = 0.0;
   transceiver_pwr_s = 0.0;
   ni_pwr_s = 0.0;

}

void Power::configureRouter(int link_width,
	int buffer_depth,
	int buffer_size,
	string routing_function,
	string selection_function)
{

// (s)tatic, (d)ynamic power

    // Buffer 
    pair<int,int> key = pair<int,int>(buffer_depth,buffer_size);

    /* TODO TURI: decommentare blocco assert
    assert(buffer_push_pm.find(key)!=buffer_push_pm.end());
    assert(buffer_pop_pm.find(key)!=buffer_pop_pm.end());
    assert(buffer_front_pm.find(key)!=buffer_front_pm.end());
    assert(buffer_leakage_pm.find(key)!=buffer_leakage_pm.end());
    */

    
    buffer_push_pwr_d = 0.0; //buffer_push_pm[key];
    buffer_pop_pwr_d = 0.0; //buffer_pop_pm[key];
    buffer_front_pwr_d = 0.0; //buffer_front_pm[key];
    buffer_pwr_s = 0.0; //buffer_leakage_pm[key];


    // Routing 
    
    /* TODO TURI: decommentare blocco assert
    assert(routing_pm_s.find(routing_function)!=routing_pm_s.end());
    assert(routing_pm_d.find(routing_function)!=routing_pm_d.end());
    */

    routing_pwr_d = 0.0; // routing_pm_d[routing_function];
    routing_pwr_s = 0.0; // routing_pm_s[routing_function];

    // Selection 
    /* TODO TURI: decommentare blocco assert
    assert(selection_pm_s.find(selection_function)!=selection_pm_s.end());
    assert(selection_pm_d.find(selection_function)!=selection_pm_d.end());
    */

    selection_pwr_d = 0.0; // selection_pm_d[selection_function];
    selection_pwr_s = 0.0; // selection_pm_s[selection_function];

    // CrossBar
    crossbar_pwr_s = 0.0; // crossbar_pwr_s;
    crossbar_pwr_d = 0.0; // crossbar_pwr_d;




// Link 
    link_pwr_s = 0.0; // link_width * bit_line_pwr_s;
    link_pwr_d = 0.0; // link_width * bit_line_pwr_d;

// NetworkInterface
    ni_pwr_s = 0.0; // ni_pwr_s_TURI_SCEGLI_NOME;
    ni_pwr_d = 0.0; // ni_pwr_d_TURI_SCEGLI_NOME;




}

void Power::configureHub(int link_width,
	int buffer_depth,
	int buffer_size,
	int antenna_buffer_depth,
	int antenna_buffer_size)
{
// (s)tatic, (d)ynamic power

// Buffer //////////////////////////////////
    pair<int,int> key = pair<int,int>(buffer_depth,buffer_size);

    /* TODO TURI: decommentare blocco assert
    assert(buffer_push_pm.find(key)!=buffer_push_pm.end());
    assert(buffer_pop_pm.find(key)!=buffer_pop_pm.end());
    assert(buffer_front_pm.find(key)!=buffer_front_pm.end());
    assert(buffer_leakage_pm.find(key)!=buffer_leakage_pm.end());
    */

    buffer_push_pwr_d = 0.0; //buffer_push_pm[key];
    buffer_pop_pwr_d = 0.0; //buffer_pop_pm[key];
    buffer_front_pwr_d = 0.0; //buffer_front_pm[key];
    buffer_pwr_s = 0.0; //buffer_leakage_pm[key];
// Buffer Antenna//////////////////////////////////
    pair<int,int> akey = pair<int,int>(antenna_buffer_depth,antenna_buffer_size);

    /* TODO TURI: decommentare blocco assert
    assert(antenna_buffer_push_pm.find(akey)!=buffer_push_pm.end());
    assert(antenna_buffer_pop_pm.find(akey)!=buffer_pop_pm.end());
    assert(antenna_buffer_front_pm.find(akey)!=buffer_front_pm.end());
    assert(antenna_buffer_leakage_pm.find(akey)!=buffer_leakage_pm.end());
    */

    antenna_buffer_push_pwr_d = 0.0; //buffer_push_pm[akey];
    antenna_buffer_pop_pwr_d = 0.0; //buffer_pop_pm[akey];
    antenna_buffer_front_pwr_d = 0.0; //buffer_front_pm[akey];
    antenna_buffer_pwr_s = 0.0; //buffer_leakage_pm[akey];

    // mappa (id_src,id_dst) -> bit pwr tx
    // TODO turi: facci ca**so ! occhio vivo
    //bit_wireless_tx_pwr = bit_wireless_tx_pm;

    // non e' una mappa, e' indipendente da sorgente e destinazione
    flit_wireless_rx_pwr = 0.0; //antenna_buffer_size * bit_wireless_rx;

    transceiver_pwr_s = 0.0; //transceiver_pwr_s_TURI_SCEGLI_NOME;

}


void Power::bufferPush()
{
    total_power_d+= buffer_push_pwr_d;
}

void Power::bufferPop()
{
    total_power_d+= buffer_pop_pwr_d;
}

void Power::bufferFront()
{
    total_power_d+= buffer_front_pwr_d;
}

void Power::antennaBufferPush()
{
    total_power_d+= antenna_buffer_push_pwr_d;
}

void Power::antennaBufferPop()
{
    total_power_d+= antenna_buffer_pop_pwr_d;
}

void Power::antennaBufferFront()
{
    total_power_d+= antenna_buffer_front_pwr_d;
}

void Power::routing()
{
    total_power_d+= routing_pwr_d;
}

void Power::selection()
{
    total_power_d+= selection_pwr_d;
}

void Power::crossBar()
{
    total_power_d+=crossbar_pwr_d;
}

void Power::link()
{
    total_power_d+=link_pwr_d;
}

void Power::networkInterface()
{
    total_power_d+=ni_pwr_d;
}


void Power::wirelessTx(int src,int dst,int length)
{
    pair<int,int> key = pair<int,int>(src,dst);

    assert(bit_wireless_tx_pwr.find(key)!=bit_wireless_tx_pwr.end());

    total_power_d += bit_wireless_tx_pwr[key] * length;

}

void Power::wirelessRx()
{
    total_power_d+= flit_wireless_rx_pwr;
}


void Power::leakage()
{
    total_power_s+= buffer_pwr_s+
	            antenna_buffer_pwr_s+
		    routing_pwr_s+
		    selection_pwr_s+
		    crossbar_pwr_s+
		    link_pwr_s+
		    transceiver_pwr_s+
		    ni_pwr_s;
}



