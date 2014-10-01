#ifndef __NOXIMTLMINITIATOR_H__
#define __NOXIMTLMINITIATOR_H__

#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/multi_passthrough_initiator_socket.h"
//#include "tlm_utils/peq_with_cb_and_phase.h"

//#include "MM.h"
#include "Utils.h"
#include "Main.h"

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
    /* TODO: remove nb
  , request_in_progress(0)
  , m_peq(this, &Initiator::peq_cb)
  */
  {
    // Register callbacks for incoming interface method calls
    //socket.register_nb_transport_bw(this, &Initiator::nb_transport_bw);

    SC_THREAD(thread_process);
  }

  void thread_process();
  /* TODO: remove nb
  //tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay );
  //void peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase);
  */
  void check_transaction(tlm::tlm_generic_payload& trans);

  //mm   m_mm;
  int  data;
  //tlm::tlm_generic_payload* request_in_progress;
  sc_event end_request_event;
  //tlm_utils::peq_with_cb_and_phase<Initiator> m_peq;

  // custom
  sc_event start_request_event;

  void set_payload(Flit&);
  Flit flit_payload;
};

#endif
