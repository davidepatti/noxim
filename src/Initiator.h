/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the global params needed by Noxim
 * to forward configuration to every sub-block
 */
#ifndef __NOXIMTLMINITIATOR_H__
#define __NOXIMTLMINITIATOR_H__

#include "tlm_utils/simple_initiator_socket.h"

#include "Utils.h"
#include "DataStructs.h"
#include "Buffer.h"



using namespace sc_core;
using namespace std;

struct Hub;

// **************************************************************************************
// Initiator module generating multiple pipelined generic payload transactions
// **************************************************************************************

struct Initiator: sc_module
{
  Hub * hub;
  // TLM-2 socket, defaults to 32-bits wide, base protocol
  tlm_utils::simple_initiator_socket<Initiator> socket;

  SC_HAS_PROCESS(Initiator);

  //SC_CTOR(Initiator)
  //: socket("socket")  // Construct and name socket
  Initiator(sc_module_name nm,Hub* h): sc_module(nm),hub(h), socket("socket")
  {

      int c;
      if (GlobalParams::use_winoc) SC_THREAD(thread_process);
      sscanf(nm,"init_%d",&c);
      _channel_id = c;
      current_hub_relay = NOT_VALID;
  }

  void thread_process();
  void check_transaction(tlm::tlm_generic_payload& trans);

  sc_event end_request_event;

  // custom
  sc_event start_request_event;

  Buffer buffer_tx;
  Flit flit_payload; 

    private: 
  int _channel_id;
  int current_hub_relay;
};

#endif
