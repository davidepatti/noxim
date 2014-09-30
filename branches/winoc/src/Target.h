#ifndef __NOXIMTLMTARGET_H__
#define __NOXIMTLMTARGET_H__

#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/multi_passthrough_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"

#include "Utils.h"
#include <queue>

using namespace sc_core;
using namespace std;

// **************************************************************************************
// Target module able to handle two pipelined transactions
// **************************************************************************************

DECLARE_EXTENDED_PHASE(internal_ph);

struct Target: sc_module
{
  // TLM-2 socket, defaults to 32-bits wide, base protocol
  tlm_utils::simple_target_socket<Target> socket;

  SC_CTOR(Target)
  : socket("socket")
  , n_trans(0)
  , response_in_progress(false)
  , next_response_pending(0)
  , end_req_pending()
  , m_peq(this, &Target::peq_cb)
  {
    // Register callbacks for incoming interface method calls
    socket.register_nb_transport_fw(this, &Target::nb_transport_fw);
  }

  // TLM-2 non-blocking transport method

  virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay );
  void peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase);
  tlm::tlm_sync_enum send_end_req(tlm::tlm_generic_payload& trans);
  void send_response(tlm::tlm_generic_payload& trans);

  int   n_trans;
  bool  response_in_progress;
  tlm::tlm_generic_payload*  next_response_pending;
  std::queue<tlm::tlm_generic_payload*>  end_req_pending;
  tlm_utils::peq_with_cb_and_phase<Target> m_peq;
};



#endif
