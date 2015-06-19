#include "Channel.h"
void Channel::b_transport( int id, tlm::tlm_generic_payload& trans, sc_time& delay )
{

    // the total transmission delay is due to TLM Initiator delay +
    // channel delay
    delay += sc_time(this->flit_transmission_delay_ps, SC_PS);

    assert (id < (int)targ_socket.size());

    // Forward path
    sc_dt::uint64 address = trans.get_address();
    sc_dt::uint64 masked_address;
    unsigned int target_nr = decode_address( address, masked_address);

    if (target_nr < init_socket.size())
    {
	// Modify address within transaction
	trans.set_address( masked_address );


	accountWirelessRxPower();

	powerManager(target_nr,trans);

	// Realize the delay annotated onto the transport call
	wait(delay);

	// Forward transaction to appropriate target
	init_socket[target_nr]->b_transport(trans, delay);


	// Replace original address
	trans.set_address( address );
    }
}


void Channel::accountWirelessRxPower()
{
    for (unsigned int i = 0; i<hubs.size();i++)
    {
	if (!GlobalParams::use_wirxsleep) 
	    hubs[i]->power.wirelessDynamicRx();
	else
	if (!(hubs[i]->power.isSleeping()))
	    hubs[i]->power.wirelessDynamicRx();
    }
}


void Channel::powerManager(int hub_dst_index, tlm::tlm_generic_payload& trans)
{
    if (!GlobalParams::use_wirxsleep) return;

    struct Flit* f = (struct Flit*)trans.get_data_ptr();

    if (f->flit_type==FLIT_TYPE_HEAD)
    {
	int sleep_cycles = flit_transmission_cycles * f->sequence_length;

	for (unsigned int i = 0; i<hubs.size();i++)
	{

	    if (i!=hub_dst_index) 
	    {
		hubs[i]->power.rxSleep(sleep_cycles);
		LOG << " HUB_"<<hubs_id[i]<<" rxSleep() invoked with " << sleep_cycles << " cycles " << endl;
	    }
	}
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


void Channel::addHub(Hub* h)
{

    hubs.push_back(h);
    hubs_id.push_back(h->getID());

}
