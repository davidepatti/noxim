/*****************************************************************************

  TRouter.cpp -- Router implementation

*****************************************************************************/

#include "TRouter.h"

void TRouter::rxProcess()
{
  if(reset.read())
    {
      // Clear outputs and indexes of receiving protocol
      for(int i=0; i<DIRECTIONS+1; i++)
	{
	  ack_rx[i].write(0);
	  channel_state[i] = STATE_CHANNEL_EMPTY;
	  current_level_rx[i] = 0;
	  reservation_table[i] = CHANNEL_NOT_RESERVED;
	}
    }
  else
    {
      // For each channel decide to accept or deny a new packet
      for(int i=0; i<DIRECTIONS+1; i++)
	{
	  // To accept a new flit, the following conditions must match:
	  //
	  // 1) there is an incoming request
	  // 2) there is a free location in the buffer
	  // 3) if the buffer contains an initiated packet, only can continue with it, or
	  // 4) if the buffer is empty or the last packet is completed, can accept a new one
	  //
	  // Actually since the Processing Element runs a Tx process a time (no multi-threading)
	  // there's no need to check for conditions 3 and 4.
	  if ( (req_rx[i].read()==1-current_level_rx[i]) &&
	       !buffer[i].IsFull() )
	    {
#ifdef DEBUG
	      TFlit f = flit_rx[i].read();
	      cout << sc_simulation_time() << ": Router " << id << ", Buffer " << i << ", " << f << endl;
#endif

	      // Store the incoming flit in the circular buffer
	      buffer[i].Push(flit_rx[i].read());            

	      // Negate the old value for Alternating Bit Protocol (ABP)
	      current_level_rx[i] = 1-current_level_rx[i];
	    }
	  ack_rx[i].write(current_level_rx[i]);
	}
    }
}

void TRouter::txProcess()
{
    if(reset.read())
    {
	// Clear outputs and indexes of transmitting protocol
	for(int i=0; i<DIRECTIONS+1; i++)
	{
	    req_tx[i].write(0);
	    current_level_tx[i] = 0;
	}
    }
    else
    {
	// For each channel see if it is possible to send a flit to its destination
	for(int i=0; i<DIRECTIONS+1; i++)
	{
	    //TODO: non e' fair! 
	    //Il canale i ha maggiore priorita' rispetto ai canali >i

	    // To send a flit the following conditions must match:
	    //
	    // 1) there is a new flit in the buffer that needs to be sent (look at the indexes)
	    // 2) if the destination got an initiated packet, only can continue with it (TO BE ADDED)
	    // 3) if the destination completed the last packet, then can accept a new one (TO BE ADDED)
	    if ( !buffer[i].IsEmpty() )
	    {
		int dest; // temporary to store current 

#ifdef DEBUG
		cout << sc_simulation_time() << ": Router " << id << ", Buffer " << i << " (" << buffer[i].Size() << ")" << endl;
#endif

		TFlit flit = buffer[i].Front();

		if (flit.flit_type==FLIT_TYPE_HEAD) 
		{
		    dest = routing(flit.dst_id);
		    if (reservation_table[dest] == CHANNEL_NOT_RESERVED) 
		    {
			short_circuit[i] = dest;     // crossbar: link input i to output dest 
			reservation_table[dest] = i; // crossbar: reserve the output channel
		    }
		}
		else dest = short_circuit[i];  // previously set by header flit

		if (reservation_table[dest] == i)  // current flit belong to the worm that reserved the output
		{
		    if ( current_level_tx[dest] == ack_tx[dest].read() )
		    {
#ifdef DEBUG
			cout << sc_simulation_time() << ": Router " << id << " SENDING " << flit << " towards port " << dest << endl;
#endif

			flit_tx[dest].write(flit);
			current_level_tx[dest] = 1 - current_level_tx[dest];
			req_tx[dest].write(current_level_tx[dest]);
			buffer[i].Pop();

			if (flit.flit_type==FLIT_TYPE_TAIL) reservation_table[short_circuit[i]] = CHANNEL_NOT_RESERVED;
			
			// Update stats
			if (dest == DIRECTION_LOCAL)
			  stats.receivedFlit(sc_simulation_time(), flit);
		    }
		}

	    } // if buffer
	} // for
    } // else
}

int TRouter::routing(int dst_id)
{
  TCoord position  = id2Coord(id);
  TCoord dst_coord = id2Coord(dst_id);

  // Compare destination coordinates with the current ones
  if(dst_coord==position)
    {
      return DIRECTION_LOCAL;
    }
  else if(routing_type==ROUTING_TYPE_XY)
    {
      if(dst_coord.x>position.x)
	{
	  return DIRECTION_EAST;
	}
      else if(dst_coord.x<position.x)
	{
	  return DIRECTION_WEST;
	}
      else if(dst_coord.y>position.y)                            // (dst_coord.x==position.x)
	{
	  return DIRECTION_SOUTH;
	}
      else                            // (dst_coord.x==position.x) && (dst_coord.y<position.y)
	{
	  return DIRECTION_NORTH;
	}
    }
  else
    {
      return 0;
    }
}


void TRouter::setId(int _id)
{
  id = _id;
  stats.setId(_id);
}
