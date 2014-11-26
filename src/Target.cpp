#include "Target.h"

void Target::b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
{
    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64    adr = trans.get_address() / 4;
    unsigned char*   ptr = trans.get_data_ptr();
    unsigned int     len = trans.get_data_length();
    unsigned char*   byt = trans.get_byte_enable_ptr();
    //unsigned int     wid = trans.get_streaming_width();

    // Obliged to check address range and check for unsupported features,
    //   i.e. byte enables, streaming, and bursts
    // Can ignore DMI hint and extensions
    // Using the SystemC report handler is an acceptable way of signalling an error

    //if (adr >= sc_dt::uint64(MEM_SIZE) || byt != 0 || len > 4 || wid < len)
    if (adr >= sc_dt::uint64(MEM_SIZE) || byt != 0 )
	SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");

    // Obliged to implement read and write commands
    if ( cmd == tlm::TLM_READ_COMMAND )
	memcpy(ptr, &mem[adr], len);
    else if ( cmd == tlm::TLM_WRITE_COMMAND )
	memcpy(&mem[adr], ptr, len);

    cout << name() << " Time: " << sc_time_stamp()  << " >>>> Target received " <<  mem[0] << endl;

    Flit f = get_payload();

    buffer_rx.Push(f);

    // Obliged to set response status to indicate successful completion
    trans.set_response_status( tlm::TLM_OK_RESPONSE );
}

Flit Target::get_payload()
{
    static int seq = 0;


    Flit f;
    cout << name() << "::get_payload() returning fake flit with src_id = 0, seq " << seq << endl;
    f.src_id = 0;
    f.sequence_no = seq;

    // seed 0, flit HBBT
    if (seq==0)
	f.flit_type = FLIT_TYPE_HEAD;
    else if (seq==3)
	f.flit_type = FLIT_TYPE_TAIL;
    else
	f.flit_type = FLIT_TYPE_BODY;

    seq++;

    if (seq==4) seq = 0;

    return f;
}

