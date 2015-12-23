#include "Hub.h"
#include "Target.h"



void Target::b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
{
    /*
    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64    adr = trans.get_address() / 4;
    unsigned char*   ptr = trans.get_data_ptr();
    unsigned int     len = trans.get_data_length();
    */
    //unsigned char*   byt = trans.get_byte_enable_ptr();
    //unsigned int     wid = trans.get_streaming_width();

    // Obliged to check address range and check for unsupported features,
    //   i.e. byte enables, streaming, and bursts
    // Can ignore DMI hint and extensions
    // Using the SystemC report handler is an acceptable way of signalling an error

    //if (adr >= sc_dt::uint64(MEM_SIZE) || byt != 0 || len > 4 || wid < len)
    /*
    if (adr >= sc_dt::uint64(MEM_SIZE) || byt != 0 )
	SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");
	*/

    // Obliged to implement read and write commands
    struct Flit* my_flit = (struct Flit*)trans.get_data_ptr();


    LOG << "*** Received: " << *my_flit << endl;

    if (!buffer_rx.IsFull())
    {
	
	int dst_port = hub->tile2Port(my_flit->dst_id);

	if (my_flit->flit_type==FLIT_TYPE_HEAD)
	{
	    if (hub->antenna2tile_reservation_table.isAvailable(dst_port))
	    {
		//LOG << "Reserving output port " << dst_port << " for channel " << local_id << endl;
		hub->antenna2tile_reservation_table.reserve(local_id, dst_port);
	    }
	    else
	    {
		LOG << "WARNING: cannot reserve output port " << dst_port << " for channel " << local_id << endl;
		return;
	    }

	}

	if (my_flit->flit_type == FLIT_TYPE_TAIL) 
	{
	    //LOG << "Releasing reservation for output port " << dst_port << endl;
	    hub->antenna2tile_reservation_table.release(dst_port);
	}
	

	LOG << "Flit " << *my_flit << " moved to rx_buffer " << endl;
	buffer_rx.Push(*my_flit);
	hub->power.antennaBufferPush();
	// Obliged to set response status to indicate successful completion
	trans.set_response_status( tlm::TLM_OK_RESPONSE );
    }
    else
    {
	LOG << "WARNING: buffer_rx is full " << endl;
    }


    // FIXME: controlla commento in RadioProcess 

}

