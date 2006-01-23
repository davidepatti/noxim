/*****************************************************************************

  TRouter.h -- Router definition

 *****************************************************************************/

#ifndef __TROUTER_H__
#define __TROUTER_H__

//---------------------------------------------------------------------------

#include <systemc.h>
#include "NoximDefs.h"
#include "TBuffer.h"
#include "TStats.h"
#include "TGlobalRoutingTable.h"
#include "TLocalRoutingTable.h"
#include "TReservationTable.h"

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

  sc_out<int>       buffer_level[DIRECTIONS+1];
  sc_in<int>        buffer_level_neighbor[DIRECTIONS+1];

  // Neighbor-on-Path related I/O

  sc_out<TNoP_data>       NoP_data_out[DIRECTIONS];
  sc_in<TNoP_data>        NoP_data_in[DIRECTIONS];

  // Registers

  /*
  TCoord             position;                        // Router position inside the mesh
  */
  int                local_id;                              // Unique ID
  int                routing_type;                    // Type of routing algorithm
  int                selection_type;
  TBuffer            buffer[DIRECTIONS+1];            // Buffer for each input channel 
  bool               current_level_rx[DIRECTIONS+1];  // Current level for Alternating Bit Protocol (ABP)
  bool               current_level_tx[DIRECTIONS+1];  // Current level for Alternating Bit Protocol (ABP)
  TStats             stats;                           // Statistics
  TLocalRoutingTable routing_table;                          // Routing table
  TReservationTable  reservation_table;                       // Switch reservation table
  int                start_from_port;                 // Port from which to start the reservation cycle

  // Functions

  void               rxProcess();        // The receiving process
  void               txProcess();        // The transmitting process
  void               bufferMonitor();
  void               configure(const int _id, const double _warm_up_time,
			       const unsigned int _max_buffer_size,
			       TGlobalRoutingTable& grt);


  // Constructor

  SC_CTOR(TRouter)
  {
    SC_METHOD(rxProcess);
    sensitive(reset);
    sensitive_pos(clock);

    SC_METHOD(txProcess);
    sensitive(reset);
    sensitive_pos(clock);

    SC_METHOD(bufferMonitor);
    sensitive(reset);
    sensitive_pos(clock);
  }

 private:

  // performs actual routing + selection
  int route(const TRouteData& route_data);

  // wrappers
  int selectionFunction(const vector<int>& directions,const TRouteData& route_data);
  vector<int> routingFunction(const TRouteData& route_data);


  // selection strategies
  int selectionRandom(const vector<int>& directions);
  int selectionBufferLevel(const vector<int>& directions);
  int selectionNoP(const vector<int>& directions,const TRouteData& route_data);


  // routing functions
  vector<int> routingXY(const TCoord& current, const TCoord& destination);
  vector<int> routingWestFirst(const TCoord& current, const TCoord& destination);
  vector<int> routingNorthLast(const TCoord& current, const TCoord& destination);
  vector<int> routingNegativeFirst(const TCoord& current, const TCoord& destination);
  vector<int> routingOddEven(const TCoord& current, const TCoord& source, const TCoord& destination);
  vector<int> routingDyAD(const TCoord& current, const TCoord& destination);
  vector<int> routingLookAhead(const TCoord& current, const TCoord& destination);
  vector<int> routingFullyAdaptive(const TCoord& current, const TCoord& destination);
  vector<int> routingTableBased(const int dir_in, const TCoord& current, const TCoord& destination);
  TNoP_data getCurrentNoPData() const;
  void NoP_report() const;
  int NoPScore(const TNoP_data& nop_data, const vector<int>& nop_channels) const;
  int reflexDirection(int direction) const;
  int getNeighborId(int _id,int direction) const;


};

#endif
