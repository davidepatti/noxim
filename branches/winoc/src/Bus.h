#ifndef __BUS_H__
#define __BUS_H__

#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/multi_passthrough_initiator_socket.h"
#include "tlm_utils/multi_passthrough_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"
#include <queue>

using namespace sc_core;
using namespace std;

#define DEBUG

// ************************************************************************************
// Bus model supports multiple initiators and multiple targets
// Supports b_ and nb_ transport interfaces, DMI and debug
// It does no arbitration, but routes all transactions from initiators without blocking
// It uses a simple built-in routing algorithm
// ************************************************************************************

struct Bus: sc_module
{
  // ***********************************************************
  // Each multi-socket can be bound to multiple sockets
  // No need for an array-of-sockets
  // ***********************************************************

  tlm_utils::multi_passthrough_target_socket<Bus>    targ_socket;
  tlm_utils::multi_passthrough_initiator_socket<Bus> init_socket;

  SC_CTOR(Bus)
  : targ_socket("targ_socket"), init_socket("init_socket")
  {
    targ_socket.register_nb_transport_fw(   this, &Bus::nb_transport_fw);
    targ_socket.register_b_transport(       this, &Bus::b_transport);
    targ_socket.register_get_direct_mem_ptr(this, &Bus::get_direct_mem_ptr);
    targ_socket.register_transport_dbg(     this, &Bus::transport_dbg);

    init_socket.register_nb_transport_bw(          this, &Bus::nb_transport_bw);
    init_socket.register_invalidate_direct_mem_ptr(this, &Bus::invalidate_direct_mem_ptr);
  }


  // Tagged non-blocking transport forward method
  virtual tlm::tlm_sync_enum nb_transport_fw(int id,
      tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay);

  // Tagged non-blocking transport backward method
  virtual tlm::tlm_sync_enum nb_transport_bw(int id,
      tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay);

  // Tagged TLM-2 blocking transport method
  virtual void b_transport( int id, tlm::tlm_generic_payload& trans, sc_time& delay );

  // Tagged TLM-2 forward DMI method
  virtual bool get_direct_mem_ptr(int id,
                                  tlm::tlm_generic_payload& trans,
                                  tlm::tlm_dmi&  dmi_data);

  // Tagged debug transaction method
  virtual unsigned int transport_dbg(int id, tlm::tlm_generic_payload& trans);

  // Tagged backward DMI method
  virtual void invalidate_direct_mem_ptr(int id,
                                         sc_dt::uint64 start_range,
                                         sc_dt::uint64 end_range);

  // Simple fixed address decoding
  // In this example, for clarity, the address is passed through unmodified to the target
  inline unsigned int decode_address( sc_dt::uint64 address, sc_dt::uint64& masked_address )
  {
    unsigned int target_nr = static_cast<unsigned int>( address & 0x1 );
    masked_address = address;
    return target_nr;
  }

  inline sc_dt::uint64 compose_address( unsigned int target_nr, sc_dt::uint64 address)
  {
    return address;
  }

  std::map <tlm::tlm_generic_payload*, unsigned int> m_id_map;
};

#endif
