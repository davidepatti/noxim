/*****************************************************************************

  TTile.h -- Tile definition

 *****************************************************************************/

#ifndef __TTILE_H__
#define __TTILE_H__

#include <systemc.h>
#include "TRouter.h"
#include "TProcessingElement.h"

SC_MODULE(TTile)
{

  // I/O Ports

  sc_in_clk           clock;        // The input clock for the tile
  sc_in<bool>         reset;        // The reset signal for the tile

  sc_in<TFlit>        flit_rx[DIRECTIONS];   // The input channels
  sc_in<bool>         req_rx[DIRECTIONS];    // The requests associated with the input channels
  sc_out<bool>        ack_rx[DIRECTIONS];    // The outgoing ack signals associated with the input channels

  sc_out<TFlit>       flit_tx[DIRECTIONS];   // The output channels
  sc_out<bool>        req_tx[DIRECTIONS];    // The requests associated with the output channels
  sc_in<bool>         ack_tx[DIRECTIONS];    // The outgoing ack signals associated with the output channels

  sc_out<uint>        buffer_level[DIRECTIONS];
  sc_in<uint>         buffer_level_neighbor[DIRECTIONS];


  // Signals

  sc_signal<TFlit>    flit_rx_local;   // The input channels
  sc_signal<bool>     req_rx_local;    // The requests associated with the input channels
  sc_signal<bool>     ack_rx_local;    // The outgoing ack signals associated with the input channels

  sc_signal<TFlit>    flit_tx_local;   // The output channels
  sc_signal<bool>     req_tx_local;    // The requests associated with the output channels
  sc_signal<bool>     ack_tx_local;    // The outgoing ack signals associated with the output channels
  
  sc_signal<uint>     buffer_level_local;
  sc_signal<uint>     buffer_level_neighbor_local;



  // Instances
  TRouter*            r;               // Router instance
  TProcessingElement* pe;              // Processing Element instance

  // Constructor

  SC_CTOR(TTile)
  {

    // Router pin assignments
    r = new TRouter("Router");
    r->clock(clock);
    r->reset(reset);
    for(int i=0; i<DIRECTIONS; i++)
    {
      r->flit_rx[i](flit_rx[i]);
      r->req_rx[i](req_rx[i]);
      r->ack_rx[i](ack_rx[i]);

      r->flit_tx[i](flit_tx[i]);
      r->req_tx[i](req_tx[i]);
      r->ack_tx[i](ack_tx[i]);

      r->buffer_level[i](buffer_level[i]);
      r->buffer_level_neighbor[i](buffer_level_neighbor[i]);
    }

    r->flit_rx[DIRECTION_LOCAL](flit_tx_local);
    r->req_rx[DIRECTION_LOCAL](req_tx_local);
    r->ack_rx[DIRECTION_LOCAL](ack_tx_local);

    r->flit_tx[DIRECTION_LOCAL](flit_rx_local);
    r->req_tx[DIRECTION_LOCAL](req_rx_local);
    r->ack_tx[DIRECTION_LOCAL](ack_rx_local);

    r->buffer_level[DIRECTION_LOCAL](buffer_level_local);
    r->buffer_level_neighbor[DIRECTION_LOCAL](buffer_level_neighbor_local);

    // Processing Element pin assignments
    pe = new TProcessingElement("ProcessingElement");
    pe->clock(clock);
    pe->reset(reset);

    pe->flit_rx(flit_rx_local);
    pe->req_rx(req_rx_local);
    pe->ack_rx(ack_rx_local);

    pe->flit_tx(flit_tx_local);
    pe->req_tx(req_tx_local);
    pe->ack_tx(ack_tx_local);

    pe->buffer_level(buffer_level_local);
    pe->buffer_level_neighbor(buffer_level_neighbor_local);

  }

};

#endif
