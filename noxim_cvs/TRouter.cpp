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
	  channel_state[i] = CHANNEL_EMPTY;
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
              if(TGlobalParams::verbose_mode)
              {
                TFlit f = flit_rx[i].read();
	        cout << sc_simulation_time() << ": Router[" << id <<"], Buffer["<< i << "], RECEIVED " << f << endl;
              }

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

                if(TGlobalParams::verbose_mode)
                {
                  cout << sc_simulation_time() << ": Router[" << id << "], Buffer[" << i << "](" << buffer[i].Size() << " flits)" << endl;
                }

		TFlit flit = buffer[i].Front();

		if (flit.flit_type==FLIT_TYPE_HEAD) 
		{
		    dest = routing(i, flit.src_id, flit.dst_id);
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
                      if(TGlobalParams::verbose_mode)
                      {
			cout << sc_simulation_time() << ": Router[" << id << "] SENDING " << flit << " towards port " << dest << endl;
                      }

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

TNoP_data TRouter::getCurrentNoPData() const 
{
    TNoP_data NoP_data;

    for (int j=0; j<DIRECTIONS; j++)
	NoP_data.buffer_status_neighbor[j] = buffer_status_neighbor[j].read();

    NoP_data.sender_id = id;

    return NoP_data;
}

//---------------------------------------------------------------------------

void TRouter::bufferMonitor()
{
  if (reset.read())
    {
	// upon reset, buffer status is put to 0
	TBufferStatus bs_empty;
	bs_empty.level = 0;
	bs_empty.state = CHANNEL_EMPTY;

      for (int i=0; i<DIRECTIONS+1; i++) buffer_status[i].write(bs_empty);
    }
  else
  {
      TBufferStatus tmp_bs;

      // update current buffers status to neighbors

      for (int i=0; i<DIRECTIONS+1; i++) 
      {
	  tmp_bs.level = buffer[i].Size();
	  tmp_bs.state = channel_state[i];
	  buffer_status[i].write(tmp_bs);
      }

      // NoP selection: send neighbor info to each direction 'i'
      TNoP_data current_NoP_data = getCurrentNoPData();

      for (int i=0; i<DIRECTIONS; i++)
	  NoP_data_out[i].write(current_NoP_data);

#if 0
      NoP_report();
#endif
  }
}

//---------------------------------------------------------------------------

int TRouter::routing(int dir_in, int src_id, int dst_id)
{
  if (dst_id == id)
    return DIRECTION_LOCAL;

  TCoord position  = id2Coord(id);
  TCoord src_coord = id2Coord(src_id);
  TCoord dst_coord = id2Coord(dst_id);

  switch (TGlobalParams::routing_algorithm)
    {
    case ROUTING_XY:
      return selectionFunction(routingXY(position, dst_coord));

    case ROUTING_WEST_FIRST:
      return selectionFunction(routingWestFirst(position, dst_coord));

    case ROUTING_NORTH_LAST:
      return selectionFunction(routingNorthLast(position, dst_coord));

    case ROUTING_NEGATIVE_FIRST:
      return selectionFunction(routingNegativeFirst(position, dst_coord));

    case ROUTING_ODD_EVEN:
      return selectionFunction(routingOddEven(position, src_coord, dst_coord));

    case ROUTING_DYAD:
      return selectionFunction(routingDyAD(position, dst_coord));

    case ROUTING_LOOK_AHEAD:
      return selectionFunction(routingLookAhead(position, dst_coord));

    case ROUTING_FULLY_ADAPTIVE:
      return selectionFunction(routingFullyAdaptive(position, dst_coord));

    case ROUTING_TABLE_BASED:
      return selectionFunction(routingTableBased(dir_in, position, dst_coord));

    default:
      assert(false);
      return 0;
    }
}

//---------------------------------------------------------------------------

void TRouter::NoP_report() const
{
    TNoP_data NoP_tmp;
      cout << sc_simulation_time() << ": Router[" << id << "], NoP report: " << endl;

      for (int i=0;i<DIRECTIONS; i++) 
      {
	  NoP_tmp = NoP_data_in[i].read();
	  cout << NoP_tmp;
      }
}
//---------------------------------------------------------------------------

int TRouter::selectionNoP(const vector<int>& directions)
{
    assert(false);

    // TODO: selection not implemented 
    return directions[rand() % directions.size()]; 
}

//---------------------------------------------------------------------------

int TRouter::selectionBufferLevel(const vector<int>& directions)
{
    // TODO: currently unfair if multiple directions have same buffer level
    unsigned int max_free_positions = 0;
    int direction_choosen = -1;

    for (unsigned int i=0;i<directions.size();i++)
    {
	unsigned int free_positions = buffer_depth - buffer_status_neighbor[directions[i]].read().level;
	if (free_positions >= max_free_positions)
	{
	    direction_choosen = directions[i];
	    max_free_positions = free_positions;
	}
    }

    if(TGlobalParams::verbose_mode)
    {
      cout << sc_simulation_time() << ": Router[" << id << "], SELECTION between: ";
      for (unsigned int i=0;i<directions.size();i++)
	cout << directions[i] << "(" << buffer_status_neighbor[directions[i]] << " flits),";
      cout << " direction choosen: " << direction_choosen << endl;
    }
    
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

    switch (TGlobalParams::selection_strategy)
    {
	case SEL_RANDOM:
	    return selectionRandom(directions);
	case SEL_BUFFER_LEVEL:
	    return selectionBufferLevel(directions);
	case SEL_NOP:
	    return selectionNoP(directions);
	default:
	    assert(false);
            return 0;
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
  //  int s1 = source.y;
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

vector<int> TRouter::routingTableBased(const int dir_in, const TCoord& current, const TCoord& destination)
{
  TAdmissibleOutputs ao = rtable.getAdmissibleOutputs(dir_in, coord2Id(destination));

  assert(ao.size() > 0);
  
  return admissibleOutputsSet2Vector(ao);
}

//---------------------------------------------------------------------------

void TRouter::configure(const int _id, 
			const double _warm_up_time,
			TGlobalRoutingTable& grt)
{
  id = _id;
  stats.configure(_id, _warm_up_time);
  
  if (grt.isValid())
    grt.getNodeRoutingTable(_id);
}

//---------------------------------------------------------------------------
