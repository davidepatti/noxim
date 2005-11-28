/*****************************************************************************

  TRouter.h -- Router definition

 *****************************************************************************/

#include <systemc.h>
#include "NoximDefs.h"
#include "TBuffer.h"
#include "TStats.h"

SC_MODULE(TRouter)
{

  // I/O Ports

  sc_in_clk          clock;        // The input clock for the router
  sc_in<bool>        reset;        // The reset signal for the router

  sc_in<TFlit>       flit_rx[DIRECTIONS+1];   // The input channels (including local one)
  sc_in<bool>        req_rx[DIRECTIONS+1];    // The requests associated with the input channels
  sc_out<bool>       ack_rx[DIRECTIONS+1];    // The outgoing ack signals associated with the input channels

  sc_out<TFlit>      flit_tx[DIRECTIONS+1];   // The output channels (including local one)
  sc_out<bool>       req_tx[DIRECTIONS+1];    // The requests associated with the output channels
  sc_in<bool>        ack_tx[DIRECTIONS+1];    // The outgoing ack signals associated with the output channels

  // Registers

  /*
  TCoord             position;                        // Router position inside the mesh
  */
  int                id;
  int                routing_type;                    // Type of routing algorithm
  TBuffer            buffer[DIRECTIONS+1];            // Buffer for each input channel
  int                channel_state[DIRECTIONS+1];     // Current state for each channel (is empty, has head, has tail)
  bool               current_level_rx[DIRECTIONS+1];  // Current level for Alternating Bit Protocol (ABP)
  bool               current_level_tx[DIRECTIONS+1];  // Current level for Alternating Bit Protocol (ABP)
  int                reservation_table[DIRECTIONS+1]; // Output channels reservations
  int                short_circuit[DIRECTIONS+1];     // Crossbar I/O connections

  TStats             stats;

  // Functions

  void               rxProcess();        // The receiving process
  void               txProcess();        // The transmitting process
  int                routing(int);       // The routing algorithm
  void               setId(int);

  // Constructor

  SC_CTOR(TRouter)
  {
    SC_METHOD(rxProcess);
    sensitive(reset);
    sensitive_pos(clock);

    SC_METHOD(txProcess);
    sensitive(reset);
    sensitive_pos(clock);
  }    

};
