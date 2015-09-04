/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2015 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the router
 */

#include "Router.h"

void Router::rxProcess()
{
    if (reset.read()) {
	// Clear outputs and indexes of receiving protocol
	for (int i = 0; i < DIRECTIONS + 2; i++) {
	    ack_rx[i].write(0);
	    current_level_rx[i] = 0;
	}
	routed_flits = 0;
	local_drained = 0;
    } else {
	// For each channel decide if a new flit can be accepted
	//
	// This process simply sees a flow of incoming flits. All arbitration
	// and wormhole related issues are addressed in the txProcess()

	for (int i = 0; i < DIRECTIONS + 2; i++) {
	    // To accept a new flit, the following conditions must match:
	    //
	    // 1) there is an incoming request
	    // 2) there is a free slot in the input buffer of direction i

	    if ((req_rx[i].read() == 1 - current_level_rx[i])
		&& !buffer[i].IsFull()) {
		Flit received_flit = flit_rx[i].read();

		// Store the incoming flit in the circular buffer
		buffer[i].Push(received_flit);

		power.bufferRouterPush();

		// Negate the old value for Alternating Bit Protocol (ABP)
		current_level_rx[i] = 1 - current_level_rx[i];


		// if a new flit is injected from local PE
		if (received_flit.src_id == local_id)
		  power.networkInterface();
	    }
	    ack_rx[i].write(current_level_rx[i]);
	}
    }
}



void Router::txProcess()
{
  if (reset.read()) 
    {
      // Clear outputs and indexes of transmitting protocol
      for (int i = 0; i < DIRECTIONS + 2; i++) 
	{
	  req_tx[i].write(0);
	  current_level_tx[i] = 0;
	}
    } 
  else 
    {
      // 1st phase: Reservation
      for (int j = 0; j < DIRECTIONS + 2; j++) 
	{
	  int i = (start_from_port + j) % (DIRECTIONS + 2);
	 
	  /*
	  if (!buffer[i].deadlockFree())
	  {
	      LOG << " deadlock on buffer input " << i << endl;
	      buffer[i].Print(" deadlock ");
	      assert(false);

	  }
	  */

	  if (!buffer[i].IsEmpty()) 
	    {

	      Flit flit = buffer[i].Front();
	      power.bufferRouterFront();

	      if (flit.flit_type == FLIT_TYPE_HEAD) 
		{
		  // prepare data for routing
		  RouteData route_data;
		  route_data.current_id = local_id;
		  route_data.src_id = flit.src_id;
		  route_data.dst_id = flit.dst_id;
		  route_data.dir_in = i;

		  int o = route(route_data);

		  LOG << " checking reservation availability of direction " << o << " for flit " << flit << endl;

		  if (reservation_table.isAvailable(o)) 
		  {
		      LOG << " reserving direction " << o << " for flit " << flit << endl;
		      reservation_table.reserve(i, o);
		  }
		  else
		  {
		      LOG << " cannot reserve direction " << o << " for flit " << flit << endl;
		  }
		}
	    }
	}
      start_from_port++;


      // 2nd phase: Forwarding
      for (int i = 0; i < DIRECTIONS + 2; i++) 
      {
	  if (!buffer[i].IsEmpty()) 
	  {
	      // power contribution already computed in 1st phase
	      Flit flit = buffer[i].Front();

	      int o = reservation_table.getOutputPort(i);
	      if (o != NOT_RESERVED) 
	      {
		  if (current_level_tx[o] == ack_tx[o].read()) 
		  {
		      //if (GlobalParams::verbose_mode > VERBOSE_OFF) 
			  LOG << "Input[" << i << "] forward to Output[" << o << "], flit: " << flit << endl;

		      flit_tx[o].write(flit);
		      if (o == DIRECTION_HUB)
		      {
			  power.r2hLink();
			  LOG << "Forwarding to HUB " << flit << endl;
		      }
		      else
		      {
			  power.r2rLink();
		      }

		      power.crossBar();

		      current_level_tx[o] = 1 - current_level_tx[o];
		      req_tx[o].write(current_level_tx[o]);
		      buffer[i].Pop();

		      power.bufferRouterPop();

		      // if flit has been consumed
		      if (flit.dst_id == local_id)
			  power.networkInterface();

		      if (flit.flit_type == FLIT_TYPE_TAIL)
			  reservation_table.release(o);

		      // Update stats
		      if (o == DIRECTION_LOCAL) 
		      {
			  LOG << "Consumed flit " << flit << endl;
			  stats.receivedFlit(sc_time_stamp().to_double() / GlobalParams::clock_period_ps, flit);
			  if (GlobalParams::
				  max_volume_to_be_drained) 
			  {
			      if (drained_volume >= GlobalParams:: max_volume_to_be_drained)
				  sc_stop();
			      else 
			      {
				  drained_volume++;
				  local_drained++;
			      }
			  }
		      } 
		      else if (i != DIRECTION_LOCAL) 
		      {
			  // Increment routed flits counter
			  routed_flits++;
		      }
		  }
		  else
		  {
		      LOG << " cannot forward Input[" << i << "] forward to Output[" << o << "], flit: " << flit << endl;
		      if (flit.flit_type == FLIT_TYPE_HEAD)
			  reservation_table.release(o);
		  }
	      }
	  }
      }
    }				// else reset read
}

NoP_data Router::getCurrentNoPData()
{
    NoP_data NoP_data;

    for (int j = 0; j < DIRECTIONS; j++) {
	try {
		NoP_data.channel_status_neighbor[j].free_slots = free_slots_neighbor[j].read();
		NoP_data.channel_status_neighbor[j].available = (reservation_table.isAvailable(j));
	}
	catch (int e)
	{
	    if (e!=NOT_VALID) assert(false);
	    // Nothing to do if an NOT_VALID direction is caught
	};
    }

    NoP_data.sender_id = local_id;

    return NoP_data;
}

void Router::perCycleUpdate()
{
    if (reset.read()) {
	for (int i = 0; i < DIRECTIONS + 1; i++)
	    free_slots[i].write(buffer[i].GetMaxBufferSize());
    } else {
        selectionStrategy->perCycleUpdate(this);

	power.leakageRouter();
	for (int i = 0; i < DIRECTIONS + 1; i++)
	{
	    power.leakageBufferRouter();
	    power.leakageLinkRouter2Router();
	}

	power.leakageLinkRouter2Hub();
    }
}

vector < int > Router::routingFunction(const RouteData & route_data)
{
    if (GlobalParams::use_winoc)
    {
        if (hasRadioHub(local_id) &&
                hasRadioHub(route_data.dst_id) &&
                !sameRadioHub(local_id,route_data.dst_id)
           )
        {
            LOG << "Setting direction HUB to reach destination node " << route_data.dst_id << endl;

            vector<int> dirv;
            dirv.push_back(DIRECTION_HUB);
            return dirv;
        }
    }
    LOG << "Wired routing for dst = " << route_data.dst_id << endl;

    return routingAlgorithm->route(this, route_data);
}

int Router::route(const RouteData & route_data)
{

    if (route_data.dst_id == local_id)
	return DIRECTION_LOCAL;

    power.routing();
    vector < int >candidate_channels = routingFunction(route_data);

    power.selection();
    return selectionFunction(candidate_channels, route_data);
}

void Router::NoP_report() const
{
    NoP_data NoP_tmp;
	LOG << "NoP report: " << endl;

    for (int i = 0; i < DIRECTIONS; i++) {
	NoP_tmp = NoP_data_in[i].read();
	if (NoP_tmp.sender_id != NOT_VALID)
	    cout << NoP_tmp;
    }
}

//---------------------------------------------------------------------------

int Router::NoPScore(const NoP_data & nop_data,
			  const vector < int >&nop_channels) const
{
    int score = 0;

    for (unsigned int i = 0; i < nop_channels.size(); i++) {
	int available;

	if (nop_data.channel_status_neighbor[nop_channels[i]].available)
	    available = 1;
	else
	    available = 0;

	int free_slots =
	    nop_data.channel_status_neighbor[nop_channels[i]].free_slots;

	score += available * free_slots;
    }

    return score;
}

int Router::selectionFunction(const vector < int >&directions,
				   const RouteData & route_data)
{
    // not so elegant but fast escape ;)
    if (directions.size() == 1)
	return directions[0];

    return selectionStrategy->apply(this, directions, route_data);
}

void Router::configure(const int _id,
			    const double _warm_up_time,
			    const unsigned int _max_buffer_size,
			    GlobalRoutingTable & grt)
{
    local_id = _id;
    stats.configure(_id, _warm_up_time);

    start_from_port = DIRECTION_LOCAL;

    if (grt.isValid())
	routing_table.configure(grt, _id);

    for (int i = 0; i < DIRECTIONS + 2; i++)
	buffer[i].SetMaxBufferSize(_max_buffer_size);

    int row = _id / GlobalParams::mesh_dim_x;
    int col = _id % GlobalParams::mesh_dim_x;
    if (row == 0)
      buffer[DIRECTION_NORTH].Disable();
    if (row == GlobalParams::mesh_dim_y-1)
      buffer[DIRECTION_SOUTH].Disable();
    if (col == 0)
      buffer[DIRECTION_WEST].Disable();
    if (col == GlobalParams::mesh_dim_x-1)
      buffer[DIRECTION_EAST].Disable();

}

unsigned long Router::getRoutedFlits()
{
    return routed_flits;
}

unsigned int Router::getFlitsCount()
{
    unsigned count = 0;

    for (int i = 0; i < DIRECTIONS + 2; i++)
	count += buffer[i].Size();

    return count;
}


int Router::reflexDirection(int direction) const
{
    if (direction == DIRECTION_NORTH)
	return DIRECTION_SOUTH;
    if (direction == DIRECTION_EAST)
	return DIRECTION_WEST;
    if (direction == DIRECTION_WEST)
	return DIRECTION_EAST;
    if (direction == DIRECTION_SOUTH)
	return DIRECTION_NORTH;

    // you shouldn't be here
    assert(false);
    return NOT_VALID;
}

int Router::getNeighborId(int _id, int direction) const
{
    Coord my_coord = id2Coord(_id);

    switch (direction) {
    case DIRECTION_NORTH:
	if (my_coord.y == 0)
	    return NOT_VALID;
	my_coord.y--;
	break;
    case DIRECTION_SOUTH:
	if (my_coord.y == GlobalParams::mesh_dim_y - 1)
	    return NOT_VALID;
	my_coord.y++;
	break;
    case DIRECTION_EAST:
	if (my_coord.x == GlobalParams::mesh_dim_x - 1)
	    return NOT_VALID;
	my_coord.x++;
	break;
    case DIRECTION_WEST:
	if (my_coord.x == 0)
	    return NOT_VALID;
	my_coord.x--;
	break;
    default:
	LOG << "Direction not valid : " << direction;
	assert(false);
    }

    int neighbor_id = coord2Id(my_coord);

    return neighbor_id;
}

bool Router::inCongestion()
{
    for (int i = 0; i < DIRECTIONS; i++) {

	if (free_slots_neighbor[i]==NOT_VALID) continue;

	int flits = GlobalParams::buffer_depth - free_slots_neighbor[i];
	if (flits > (int) (GlobalParams::buffer_depth * GlobalParams::dyad_threshold))
	    return true;
    }

    return false;
}

void Router::ShowBuffersStats(std::ostream & out)
{
  for (int i=0; i<DIRECTIONS+2; i++)
    buffer[i].ShowStats(out);
}
