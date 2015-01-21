#ifndef __NOXIMTLMTARGET_H__
#define __NOXIMTLMTARGET_H__

#include "tlm_utils/simple_target_socket.h"
#include "Utils.h"
#include "DataStructs.h"
#include "Buffer.h"

#include <queue>

using namespace sc_core;
using namespace std;

#define MEM_SIZE 256

struct Hub;

// **************************************************************************************
// Target module able to handle two pipelined transactions
// **************************************************************************************


struct Target: sc_module
{
  // TLM-2 socket, defaults to 32-bits wide, base protocol
  Hub* hub;
  tlm_utils::simple_target_socket<Target> socket;

  SC_HAS_PROCESS(Target);
  int local_id;

  //SC_CTOR(Target)
  //: socket("socket")
  Target(sc_module_name nm, int id, Hub* h): sc_module(nm), hub(h), socket("socket")
  {
      local_id = id;

    // Register callback for incoming b_transport interface method call
      socket.register_b_transport(this, &Target::b_transport);
  }

  virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay );

  int mem[MEM_SIZE];

  int   n_trans;
  bool  response_in_progress;

  Buffer buffer_rx;

  Flit get_payload();


};



#endif
