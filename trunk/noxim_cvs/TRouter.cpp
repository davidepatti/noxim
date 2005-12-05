/*****************************************************************************

  TRouter.cpp -- Router implementation

*****************************************************************************/

#include "TRouter.h"

//---------------------------------------------------------------------------

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
	  if ( (req_rx[i].read()==1-current_level_rx[i]) && !buffer[i].IsFull() )
	    {
#ifdef DEBUG
	      TFlit f = flit_rx[i].read();
	      cout << sc_simulation_time() << ": Router[" << id <<"], Buffer["<< i << "], RECEIVED " << f << endl;
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

//---------------------------------------------------------------------------

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
		cout << sc_simulation_time() << ": Router[" << id << "], Buffer[" << i << "](" << buffer[i].Size() << " flits)" << endl;
#endif

		TFlit flit = buffer[i].Front();

		if (flit.flit_type==FLIT_TYPE_HEAD) 
		{
		    dest = routing(flit.src_id, flit.dst_id);
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
			cout << sc_simulation_time() << ": Router[" << id << "] SENDING " << flit << " towards port " << dest << endl;
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

//---------------------------------------------------------------------------

void TRouter::bufferMonitor()
{
  if (reset.read())
    {
      for (int i=0; i<DIRECTIONS+1; i++) buffer_level[i] = 0;
    }
  else
  {
      for (int i=0; i<DIRECTIONS+1; i++) buffer_level[i].write(buffer[i].Size());
  }
}

//---------------------------------------------------------------------------

int TRouter::routing(int src_id, int dst_id)
{
  if (dst_id == id)
    return DIRECTION_LOCAL;

  TCoord position  = id2Coord(id);
  TCoord src_coord = id2Coord(src_id);
  TCoord dst_coord = id2Coord(dst_id);

  switch (routing_type)
    {
    case XY:
      return selectionFunction(routingXY(position, dst_coord));

    case WEST_FIRST:
      return selectionFunction(routingWestFirst(position, dst_coord));

    case NORTH_LAST:
      return selectionFunction(routingNorthLast(position, dst_coord));

    case NEGATIVE_FIRST:
      return selectionFunction(routingNegativeFirst(position, dst_coord));

    case ODD_EVEN:
      return selectionFunction(routingOddEven(position, src_coord, dst_coord));

    case DYAD:
      return selectionFunction(routingDyAD(position, dst_coord));

    case LOOK_AHEAD:
      return selectionFunction(routingLookAhead(position, dst_coord));

    case NOPCAR:
      return selectionFunction(routingNoPCAR(position, dst_coord));
      
    case FULLY_ADAPTIVE:
      return selectionFunction(routingFullyAdaptive(position, dst_coord));

    default:
      assert(false);
    }
}

//---------------------------------------------------------------------------

int TRouter::selectionNoPCAR(const vector<int>& directions)
{
    assert(false);
}
//---------------------------------------------------------------------------

int TRouter::selectionBufferLevel(const vector<int>& directions)
{
    // TODO: currently unfair if multiple directions have same buffer level
    unsigned int max_free_positions = 0;
    int direction_choosen = -1;

    for (unsigned int i=0;i<directions.size();i++)
    {
	unsigned int free_positions = buffer_depth - buffer_level_neighbor[directions[i]];
	if (free_positions >= max_free_positions)
	{
	    direction_choosen = directions[i];
	    max_free_positions = free_positions;
	}
    }

#ifdef DEBUG
    cout << sc_simulation_time() << ": Router[" << id << "], SELECTION between: ";
    for (unsigned int i=0;i<directions.size();i++)
	cout << directions[i] << "(" << buffer_level_neighbor[directions[i]] << " flits),";
    cout << " direction choosen: " << direction_choosen << endl;
#endif
    
    assert(direction_choosen>=0);
    return direction_choosen;
}
//---------------------------------------------------------------------------

int TRouter::selectionRandom(const vector<int>& directions)
{
  return directions[rand() % directions.size()]; 
}

//---------------------------------------------------------------------------

int TRouter::selectionFunction(const vector<int>& directions)
{
    // TODO: vedere che dice mau
    if (directions.size()==1) return directions[0];

    switch (selection_type)
    {
	case SELECTION_RANDOM:
	    return selectionRandom(directions);
	case SELECTION_BUFFER_LEVEL:
	    return selectionBufferLevel(directions);
	case SELECTION_NOPCAR:
	    return selectionNoPCAR(directions);
	default:
	    assert(false);
    }
}

//---------------------------------------------------------------------------

vector<int> TRouter::routingXY(const TCoord& current, const TCoord& destination)
{
  vector<int> directions;
  
  if (destination.x > current.x)
    directions.push_back(DIRECTION_EAST);
  else if (destination.x < current.x)
    directions.push_back(DIRECTION_WEST);
  else if (destination.y > current.y)
    directions.push_back(DIRECTION_SOUTH);
  else
    directions.push_back(DIRECTION_NORTH);

  return directions;
}

//---------------------------------------------------------------------------

vector<int> TRouter::routingWestFirst(const TCoord& current, const TCoord& destination)
{
  vector<int> directions;

  if (destination.x <= current.x ||
      destination.y == current.y)
    return routingXY(current, destination);

  if (destination.y < current.y)
    {
      directions.push_back(DIRECTION_NORTH);
      directions.push_back(DIRECTION_EAST);
    }
  else
    {
      directions.push_back(DIRECTION_SOUTH);
      directions.push_back(DIRECTION_EAST);
    }

  return directions;
}

//---------------------------------------------------------------------------

vector<int> TRouter::routingNorthLast(const TCoord& current, const TCoord& destination)
{
  vector<int> directions;

  if (destination.x == current.x ||
      destination.y <= current.y)
    return routingXY(current, destination);

  if (destination.x < current.x)
    {
      directions.push_back(DIRECTION_SOUTH);
      directions.push_back(DIRECTION_WEST);
    }
  else
    {
      directions.push_back(DIRECTION_SOUTH);
      directions.push_back(DIRECTION_EAST);
    }

  return directions;
}

//---------------------------------------------------------------------------

vector<int> TRouter::routingNegativeFirst(const TCoord& current, const TCoord& destination)
{
  vector<int> directions;

  if ( (destination.x <= current.x && destination.y <= current.y) ||
       (destination.x >= current.x && destination.y >= current.y) )
    return routingXY(current, destination);

  if (destination.x > current.x && 
      destination.y < current.y)
    {
      directions.push_back(DIRECTION_NORTH);
      directions.push_back(DIRECTION_EAST);
    }
  else
    {
      directions.push_back(DIRECTION_SOUTH);
      directions.push_back(DIRECTION_WEST);
    }

  return directions;
}

//---------------------------------------------------------------------------

vector<int> TRouter::routingOddEven(const TCoord& current, 
				    const TCoord& source, const TCoord& destination)
{
  vector<int> directions;

  int c0 = current.x;
  int c1 = current.y;
  int s0 = source.x;
  int s1 = source.y;
  int d0 = destination.x;
  int d1 = destination.y;
  int e0, e1;

  e0 = d0 - c0;
  e1 = -(d1 - c1);

  if (e0 == 0)
    {
      if (e1 > 0)
	directions.push_back(DIRECTION_NORTH);
      else
	directions.push_back(DIRECTION_SOUTH);
    }
  else
    {
      if (e0 > 0)
	{
	  if (e1 == 0)
	    directions.push_back(DIRECTION_EAST);
	  else
	    {
	      if ( (c0 % 2 == 1) || (c0 == s0) )
		{
		  if (e1 > 0)
		    directions.push_back(DIRECTION_NORTH);
		  else
		    directions.push_back(DIRECTION_SOUTH);
		}
	      if ( (d0 % 2 == 1) || (e0 != 1) )
		directions.push_back(DIRECTION_EAST);
	    }
	}
      else
	{
	  directions.push_back(DIRECTION_WEST);
	  if (c0 % 2 == 0)
	    {
	      if (e1 > 0)
		directions.push_back(DIRECTION_NORTH);
	      else
		directions.push_back(DIRECTION_SOUTH);
	    }
	}
    }
  
  assert(directions.size() > 0 && directions.size() <= 2);
  
  return directions;
}

//---------------------------------------------------------------------------

vector<int> TRouter::routingDyAD(const TCoord& current, const TCoord& destination)
{
  vector<int> directions;

  assert(false);
  return directions;
}

//---------------------------------------------------------------------------

vector<int> TRouter::routingLookAhead(const TCoord& current, const TCoord& destination)
{
  vector<int> directions;

  assert(false);
  return directions;
}

//---------------------------------------------------------------------------

vector<int> TRouter::routingNoPCAR(const TCoord& current, const TCoord& destination)
{
  vector<int> directions;

  assert(false);
  return directions;
}

//---------------------------------------------------------------------------

vector<int> TRouter::routingFullyAdaptive(const TCoord& current, const TCoord& destination)
{
  vector<int> directions;

  if (destination.x == current.x ||
      destination.y == current.y)
    return routingXY(current, destination);

  if (destination.x > current.x &&
      destination.y < current.y)
    {
      directions.push_back(DIRECTION_NORTH);
      directions.push_back(DIRECTION_EAST);
    }
  else if (destination.x > current.x &&
	   destination.y > current.y)
    {
      directions.push_back(DIRECTION_SOUTH);
      directions.push_back(DIRECTION_EAST);
    }
  else if (destination.x < current.x &&
	   destination.y > current.y)
    {
      directions.push_back(DIRECTION_SOUTH);
      directions.push_back(DIRECTION_WEST);
    }
  else
    {
      directions.push_back(DIRECTION_NORTH);
      directions.push_back(DIRECTION_WEST);
    }
  
  return directions;
}

//---------------------------------------------------------------------------

void TRouter::configure(int _id, int _routing_type, int _selection_type,int _buffer_depth)
{
  setId(_id);
  routing_type = _routing_type;
  selection_type = _selection_type;
  buffer_depth = _buffer_depth;

}

//---------------------------------------------------------------------------

void TRouter::setId(int _id)
{
  id = _id;
  stats.setId(_id);
}

//---------------------------------------------------------------------------
