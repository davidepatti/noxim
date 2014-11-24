#include "Initiator.h"

  void Initiator::thread_process()
  {

      tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
      tlm::tlm_phase phase;
      sc_time delay;

      while (1)
      {

	  cout << "\n" << name() << " ****** Initiator - waiting for transmissions" << endl;

	  // TODO: check
	  wait(start_request_event);

	  cout << "\n" << name() << " ****** Initiator - starting blocking transmissions" << endl;

	  // TODO: fixed address!
	  int i = 0;

	  tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;

	  data = 42;

	  trans->set_command(cmd);
	  trans->set_address( i );
	  //trans->set_data_ptr( reinterpret_cast<unsigned char*>(&data) );
	  //trans->set_data_length( 4 );
	  trans->set_data_ptr( reinterpret_cast<unsigned char*>(&flit_payload) );
	  trans->set_data_length( sizeof(Flit) );
	  trans->set_streaming_width( 4 ); // = data_length to indicate no streaming
	  trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
	  trans->set_dmi_allowed( false ); // Mandatory initial value
	  trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

	  delay = sc_time(rand_ps(), SC_PS);

	  cout << "Calling b_transport at " << sc_time_stamp() << " with delay = " << delay << endl;

	  // Call b_transport to demonstrate the b/nb conversion by the simple_target_socket
	  socket->b_transport( *trans, delay );

	  // Initiator obliged to check response status and delay
	  if ( trans->is_response_error() )
	      SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

	  cout << "trans = { " << (cmd ? 'W' : 'R') << ", " << hex << i
	      << " } , data = " << hex << data << " at time " << sc_time_stamp()
	      << " delay = " << delay << endl;

	  // Realize the delay annotated onto the transport call
	  wait(delay);
	  //check_transaction( *trans );

      }

  }



void Initiator::set_payload(Flit& payload)
{
    this->flit_payload = payload;
}

