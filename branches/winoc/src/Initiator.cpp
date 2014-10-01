#include "Initiator.h"
#include "Main.h"

  void Initiator::thread_process()
  {

      tlm::tlm_generic_payload* trans;
      tlm::tlm_phase phase;
      sc_time delay;


      if (!GlobalParams::use_winoc) goto skip;


      // TODO: check
      wait(start_request_event);


      cout << "\n ****** WIRELESS TEST - starting 2 non-blocking transmissions" << endl;

    // Generate a sequence of random transactions
    for (int i = 0; i < 2; i++)
    {
      int adr = rand();
      tlm::tlm_command cmd = static_cast<tlm::tlm_command>(rand() % 2);
      if (cmd == tlm::TLM_WRITE_COMMAND) data[i % 16] = rand();

      // Grab a new transaction from the memory manager
      trans = m_mm.allocate();
      trans->acquire();

      // Set all attributes except byte_enable_length and extensions (unused)
      trans->set_command( cmd );
      trans->set_address( adr );
      trans->set_data_ptr( reinterpret_cast<unsigned char*>(&data[i % 16]) );
      trans->set_data_length( 4 );
      trans->set_streaming_width( 4 ); // = data_length to indicate no streaming
      trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
      trans->set_dmi_allowed( false ); // Mandatory initial value
      trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

      // Initiator must honor BEGIN_REQ/END_REQ exclusion rule
      if (request_in_progress)
        wait(end_request_event);
      request_in_progress = trans;
      phase = tlm::BEGIN_REQ;

      // Timing annotation models processing time of initiator prior to call
      delay = sc_time(rand_ps(), SC_PS);

      cout << hex << adr << " " << name() << " new, cmd=" << (cmd ? 'W' : 'R')
           << ", data=" << hex << data[i % 16] << " at time " << sc_time_stamp() << endl;

      // Non-blocking transport call on the forward path
      tlm::tlm_sync_enum status;
      status = socket->nb_transport_fw( *trans, phase, delay );

      // Check value returned from nb_transport_fw
      if (status == tlm::TLM_UPDATED)
      {
        // The timing annotation must be honored
        m_peq.notify( *trans, phase, delay );
      }
      else if (status == tlm::TLM_COMPLETED)
      {
        // The completion of the transaction necessarily ends the BEGIN_REQ phase
        request_in_progress = 0;

        // The target has terminated the transaction
        check_transaction( *trans );
      }
      wait( sc_time(rand_ps(), SC_PS) );
    }

    wait(100, SC_NS);

    // Allocate a transaction for one final, nominal call to b_transport
    trans = m_mm.allocate();
    trans->acquire();
    trans->set_command( tlm::TLM_WRITE_COMMAND );
    trans->set_address( 0 );
    trans->set_data_ptr( reinterpret_cast<unsigned char*>(&data[0]) );
    trans->set_data_length( 4 );
    trans->set_streaming_width( 4 ); // = data_length to indicate no streaming
    trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
    trans->set_dmi_allowed( false ); // Mandatory initial value
    trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

    delay = sc_time(rand_ps(), SC_PS);

    cout << "Calling b_transport at " << sc_time_stamp() << " with delay = " << delay << endl;

    // Call b_transport to demonstrate the b/nb conversion by the simple_target_socket
    socket->b_transport( *trans, delay );
    check_transaction( *trans );

skip:
    ;
    // ciao

  }

  // TLM-2 backward non-blocking transport method

  tlm::tlm_sync_enum Initiator::nb_transport_bw( tlm::tlm_generic_payload& trans,
                                              tlm::tlm_phase& phase, sc_time& delay )
  {
    // The timing annotation must be honored
    m_peq.notify( trans, phase, delay );
    return tlm::TLM_ACCEPTED;
  }

  // Payload event queue callback to handle transactions from target
  // Transaction could have arrived through return path or backward path

  void Initiator::peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase)
  {
    #ifdef DEBUG
      if (phase == tlm::END_REQ)
        cout << hex << trans.get_address() << " " << name() << " END_REQ at " << sc_time_stamp() << endl;
      else if (phase == tlm::BEGIN_RESP)
        cout << hex << trans.get_address() << " " << name() << " BEGIN_RESP at " << sc_time_stamp() << endl;
    #endif

    if (phase == tlm::END_REQ || (&trans == request_in_progress && phase == tlm::BEGIN_RESP))
    {
      // The end of the BEGIN_REQ phase
      request_in_progress = 0;
      end_request_event.notify();
    }
    else if (phase == tlm::BEGIN_REQ || phase == tlm::END_RESP)
      SC_REPORT_FATAL("TLM-2", "Illegal transaction phase received by initiator");

    if (phase == tlm::BEGIN_RESP)
    {
      check_transaction( trans );

      // Send final phase transition to target
      tlm::tlm_phase fw_phase = tlm::END_RESP;
      sc_time delay = sc_time(rand_ps(), SC_PS);
      socket->nb_transport_fw( trans, fw_phase, delay );
      // Ignore return value
    }
  }

  // Called on receiving BEGIN_RESP or TLM_COMPLETED
  void Initiator::check_transaction(tlm::tlm_generic_payload& trans)
  {
    if ( trans.is_response_error() )
    {
      char txt[100];
      sprintf(txt, "Transaction returned with error, response status = %s",
                   trans.get_response_string().c_str());
      SC_REPORT_ERROR("TLM-2", txt);
    }

    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64    adr = trans.get_address();
    int*             ptr = reinterpret_cast<int*>( trans.get_data_ptr() );

    cout << hex << adr << " " << name() << " check, cmd=" << (cmd ? 'W' : 'R')
         << ", data=" << hex << *ptr << " at time " << sc_time_stamp() << endl;

    // Allow the memory manager to free the transaction object
    trans.release();
  }


void Initiator::set_payload(Flit& payload)
{
    this->flit_payload = payload;
}

