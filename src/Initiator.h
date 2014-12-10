#ifndef __NOXIMTLMINITIATOR_H__
#define __NOXIMTLMINITIATOR_H__

#include "tlm_utils/simple_initiator_socket.h"

#include "Utils.h"
#include "DataStructs.h"
#include "Buffer.h"

using namespace sc_core;
using namespace std;

// **************************************************************************************
// Initiator module generating multiple pipelined generic payload transactions
// **************************************************************************************

struct Initiator: sc_module
{
  // TLM-2 socket, defaults to 32-bits wide, base protocol
  tlm_utils::simple_initiator_socket<Initiator> socket;

  SC_CTOR(Initiator)
  : socket("socket")  // Construct and name socket
  {

    SC_THREAD(thread_process);
  }

  void thread_process();
  void check_transaction(tlm::tlm_generic_payload& trans);

  sc_event end_request_event;

  // custom
  sc_event start_request_event;

  Buffer buffer_tx;
  Flit flit_payload; 
};

#endif
