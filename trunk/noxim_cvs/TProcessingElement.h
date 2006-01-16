/*****************************************************************************

  TProcessingElement.h -- Processing Element (PE) definition

 *****************************************************************************/

#ifndef __TPROCESSING_ELEMENT_H__
#define __TPROCESSING_ELEMENT_H__

//---------------------------------------------------------------------------

#include <queue>
#include <systemc.h>
#include "NoximDefs.h"
#include "TGlobalTrafficTable.h"
using namespace std;

SC_MODULE(TProcessingElement)
{

  // I/O Ports

  sc_in_clk            clock;        // The input clock for the PE
  sc_in<bool>          reset;        // The reset signal for the PE

  sc_in<TFlit>         flit_rx;      // The input channel
  sc_in<bool>          req_rx;       // The request associated with the input channel
  sc_out<bool>         ack_rx;       // The outgoing ack signal associated with the input channel

  sc_out<TFlit>        flit_tx;      // The output channel
  sc_out<bool>         req_tx;       // The request associated with the output channel
  sc_in<bool>          ack_tx;       // The outgoing ack signal associated with the output channel

  sc_out<int>         buffer_level;
  sc_in<int>          buffer_level_neighbor;

  // Registers

  int                  id;                     // Unique identification number
  bool                 current_level_rx;       // Current level for Alternating Bit Protocol (ABP)
  bool                 current_level_tx;       // Current level for Alternating Bit Protocol (ABP)
  queue<TPacket>       packet_queue;           // Local queue of packets
  bool                 transmittedAtPreviousCycle;  // Used for distributions with memory

  // Functions

  void                 rxProcess();                       // The receiving process
  void                 txProcess();                       // The transmitting process
  bool                 probabilityShot(TPacket p);        // The probability to send a new packet
  TFlit                nextFlit();                        // Take the next flit of the current packet
  TPacket              nextPacket();                      // Create a new packet
  TPacket              trafficUniform();                  // Uniform destination distribution
  TPacket              trafficTranspose1();               // Transpose 1 destination distribution
  TPacket              trafficTranspose2();               // Transpose 2 destination distribution
  TPacket              trafficTableBased();               // Traffic Table Based destination distribution
  TGlobalTrafficTable* traffic_table;                     // Reference to the Global traffic Table
  int                  occurrencesInTrafficTableAsSource; // Number of occurrences in Traffic Table
  void                 fixRanges(const TCoord, TCoord&);  // Fix the ranges of the destination

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
