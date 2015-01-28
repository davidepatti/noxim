#ifndef __BUS_H__
#define __BUS_H__

#include "Utils.h"
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
// Channel model supports multiple initiators and multiple targets
// Supports b_ and nb_ transport interfaces, DMI and debug
// It does no arbitration, but routes all transactions from initiators without blocking
// It uses a simple built-in routing algorithm
// ************************************************************************************

struct Channel: sc_module
{

    vector<int> hubs;

    void addHub(int);

  SC_HAS_PROCESS(Channel);
  // ***********************************************************
  // Each multi-socket can be bound to multiple sockets
  // No need for an array-of-sockets
  // ***********************************************************

  tlm_utils::multi_passthrough_target_socket<Channel>    targ_socket;
  tlm_utils::multi_passthrough_initiator_socket<Channel> init_socket;

  int local_id; // Unique ID

  Channel(sc_module_name nm, int id)
  : sc_module(nm), targ_socket("targ_socket"), init_socket("init_socket")
  {
    local_id = id;
    targ_socket.register_b_transport(       this, &Channel::b_transport);
    targ_socket.register_get_direct_mem_ptr(this, &Channel::get_direct_mem_ptr);
    targ_socket.register_transport_dbg(     this, &Channel::transport_dbg);

    init_socket.register_invalidate_direct_mem_ptr(this, &Channel::invalidate_direct_mem_ptr);
  }


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
      //unsigned int target_nr = static_cast<unsigned int>( address );
      int target_nr = NOT_VALID;

      masked_address = address;

      for (unsigned int i=0;i<hubs.size();i++)
      {
	  if (hubs[i]==static_cast<int>(masked_address))
	  {
	      target_nr = i;
	      break;
	  }	  
      }
      LOG << "Address " << masked_address << "(Hub_"<<masked_address<<") in this channel corresponds to target_nr " << target_nr << endl;
      assert(target_nr!=NOT_VALID);
      return target_nr;
  }

  inline sc_dt::uint64 compose_address( unsigned int target_nr, sc_dt::uint64 address)
  {
    return address;
  }

  std::map <tlm::tlm_generic_payload*, unsigned int> m_id_map;
};

#endif
