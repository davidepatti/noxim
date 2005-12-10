/*****************************************************************************

  TProcessingElement.h -- Processing Element (PE) definition

 *****************************************************************************/

#ifndef __TPROCESSING_ELEMENT_H__
#define __TPROCESSING_ELEMENT_H__

//---------------------------------------------------------------------------

#include <queue>
#include <systemc.h>
#include "NoximDefs.h"

SC_MODULE(TProcessingElement)
{

  // I/O Ports

  sc_in_clk           clock;        // The input clock for the PE
  sc_in<bool>         reset;        // The reset signal for the PE

  sc_in<TFlit>        flit_rx;      // The input channel
  sc_in<bool>         req_rx;       // The request associated with the input channel
  sc_out<bool>        ack_rx;       // The outgoing ack signal associated with the input channel

  sc_out<TFlit>       flit_tx;      // The output channel
  sc_out<bool>        req_tx;       // The request associated with the output channel
  sc_in<bool>         ack_tx;       // The outgoing ack signal associated with the output channel

  sc_out<uint>        buffer_level;
  sc_in<uint>         buffer_level_neighbor;

  // Registers
  int                 id;
  bool                current_level_rx;       // Current level for Alternating Bit Protocol (ABP)
  bool                current_level_tx;       // Current level for Alternating Bit Protocol (ABP)
  std::queue<TPacket> packet_queue;

  // Functions

  void                rxProcess();          // The receiving process
  void                txProcess();          // The transmitting process
  int                 probabilityShot();    // The probability to send a new flit
  TFlit               nextFlit();           // Take next flit
  TPacket             randomPacket();       // Create a new random flit


  // Constructor

  SC_CTOR(TProcessingElement)
  {
    SC_METHOD(rxProcess);
    sensitive(reset);
    sensitive_pos(clock);

    SC_METHOD(txProcess);
    sensitive(reset);
    sensitive_pos(clock);
  }    

};

#endif
