#include "Channel.h"

  tlm::tlm_sync_enum Channel::nb_transport_fw(int id,
      tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay)
  {
    assert (id < (int)targ_socket.size());

    // Forward path
    m_id_map[ &trans ] = id;

    sc_dt::uint64 address = trans.get_address();
    sc_dt::uint64 masked_address;
    unsigned int target_nr = decode_address( address, masked_address);

    if (target_nr < init_socket.size())
    {
      // Modify address within transaction
      trans.set_address( masked_address );

      // Forward transaction to appropriate target
      tlm::tlm_sync_enum status = init_socket[target_nr]->nb_transport_fw(trans, phase, delay);

      if (status == tlm::TLM_COMPLETED)
        // Put back original address
        trans.set_address( address );
      return status;
    }
    else
      return tlm::TLM_COMPLETED;
  }

  tlm::tlm_sync_enum Channel::nb_transport_bw(int id,
      tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay)
  {
    assert (id < (int)init_socket.size());

    // Backward path

    // Replace original address
    sc_dt::uint64 address = trans.get_address();
    trans.set_address( compose_address( id, address ) );

    return targ_socket[ m_id_map[ &trans ] ]->nb_transport_bw(trans, phase, delay);
  }

  void Channel::b_transport( int id, tlm::tlm_generic_payload& trans, sc_time& delay )
  {
    assert (id < (int)targ_socket.size());

    // Forward path
    sc_dt::uint64 address = trans.get_address();
    sc_dt::uint64 masked_address;
    unsigned int target_nr = decode_address( address, masked_address);

    if (target_nr < init_socket.size())
    {
      // Modify address within transaction
      trans.set_address( masked_address );

      // Forward transaction to appropriate target
      init_socket[target_nr]->b_transport(trans, delay);

      // Replace original address
      trans.set_address( address );
    }
  }

  bool Channel::get_direct_mem_ptr(int id,
                                  tlm::tlm_generic_payload& trans,
                                  tlm::tlm_dmi&  dmi_data)
  {
    sc_dt::uint64 masked_address;
    unsigned int target_nr = decode_address( trans.get_address(), masked_address );
    if (target_nr >= init_socket.size())
      return false;

    trans.set_address( masked_address );

    bool status = init_socket[target_nr]->get_direct_mem_ptr( trans, dmi_data );

    // Calculate DMI address of target in system address space
    dmi_data.set_start_address( compose_address( target_nr, dmi_data.get_start_address() ));
    dmi_data.set_end_address  ( compose_address( target_nr, dmi_data.get_end_address() ));

    return status;
  }

  unsigned int Channel::transport_dbg(int id, tlm::tlm_generic_payload& trans)
  {
    sc_dt::uint64 masked_address;
    unsigned int target_nr = decode_address( trans.get_address(), masked_address );
    if (target_nr >= init_socket.size())
      return 0;
    trans.set_address( masked_address );

    // Forward debug transaction to appropriate target
    return init_socket[target_nr]->transport_dbg( trans );
  }

  void Channel::invalidate_direct_mem_ptr(int id,
                                         sc_dt::uint64 start_range,
                                         sc_dt::uint64 end_range)
  {
    // Reconstruct address range in system memory map
    sc_dt::uint64 bw_start_range = compose_address( id, start_range );
    sc_dt::uint64 bw_end_range   = compose_address( id, end_range );

    // Propagate call backward to all initiators
    for (unsigned int i = 0; i < targ_socket.size(); i++)
      targ_socket[i]->invalidate_direct_mem_ptr(bw_start_range, bw_end_range);
  }
