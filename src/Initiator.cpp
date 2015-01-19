#include "Initiator.h"

  void Initiator::thread_process()
  {

      tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
      tlm::tlm_phase phase;
      sc_time delay;

      while (1)
      {

	  LOG << " ****** Initiator - waiting for transmissions" << endl;

	  // TODO: check
	  wait(start_request_event);

	  LOG << " ****** Initiator - starting blocking transmissions" << endl;

	  tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;
	  flit_payload = buffer_tx.Front();

	  int destHub = tile2Hub(flit_payload.dst_id);
	  LOG << "Forwarding to wireless to reach Hub_" << destHub <<  endl;

	  trans->set_command(cmd);
	  trans->set_address(destHub);

	  trans->set_data_ptr( reinterpret_cast<unsigned char*>(&flit_payload) );
	  trans->set_data_length( sizeof(Flit) );
	  trans->set_streaming_width( sizeof(Flit) ); // = data_length to indicate no streaming
	  trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
	  trans->set_dmi_allowed( false ); // Mandatory initial value
	  trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

	  delay = sc_time(rand_ps(), SC_PS);

	  LOG << "Calling blocking transport with delay = " << delay << " and target address = " << destHub << endl;

	  // Call b_transport to demonstrate the b/nb conversion by the simple_target_socket
	  socket->b_transport( *trans, delay );


	  // Initiator obliged to check response status and delay
	  if ( !trans->is_response_error() )
	  {
	      buffer_tx.Pop();
	  }
	  else
	  {
		LOG << " WARNING: incomplete transaction " << endl;
	  }


	  LOG << "trans = { " << (cmd ? 'W' : 'R') << ", " << destHub
	      << " } , " <<  " at time " << sc_time_stamp()
	      << " delay = " << delay << endl;

	  // Realize the delay annotated onto the transport call
	  wait(delay);
	  //check_transaction( *trans );

      }

  }



