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
    power.setRxPower(0.0);
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

		power.bufferPush();
        power.setRxPower(power.getRxPower() + power.getbuffer_push_pwr_d());

		// Negate the old value for Alternating Bit Protocol (ABP)
		current_level_rx[i] = 1 - current_level_rx[i];


		// if a new flit is injected from local PE
		if (received_flit.src_id == local_id)
		  power.networkInterface();
                  power.setRxPower(power.getRxPower() + power.getni_pwr_d());
	    }
	    ack_rx[i].write(current_level_rx[i]);
	}
    }
    if (GlobalParams::verbose_mode > VERBOSE_HIGH)
    cout << "fin update process RX du routeur " << local_id << " " << power.getRxPower() << endl;
    
    // On 3 processes Tx, Rx and perCycleUpdate, Rx is the last executed according to the scheduler
    // So we sum the power here
    power.setInstantPower();
}



void Router::txProcess()
{
  power.setTxPower(0.0);
  if (reset.read()) 
    {
      // Clear outputs and indexes of transmitting protocol
      for (int i = 0; i < DIRECTIONS + 2; i++) 
	{
	  req_tx[i].write(0);
	  current_level_tx[i] = 0;
          previous_payload[i] = 0;
	}
      // Clear transition array
      for(int i = 0; i < 22;i++)
          transition_types[i] = 0;
      
      //Name of file to store all alpha values
      alphaFileName = "Alpha_file";
      
      
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
	      LOG << " deadlock on buffer " << i << endl;
	      buffer[i].Print(" deadlock ");
	  }
	  */

	  if (!buffer[i].IsEmpty()) 
	    {

	      Flit flit = buffer[i].Front();
	      power.bufferFront();
              power.setTxPower(power.getTxPower() + power.getbuffer_front_pwr_d());

	      if (flit.flit_type == FLIT_TYPE_HEAD) 
		{
		  // prepare data for routing
		  RouteData route_data;
		  route_data.current_id = local_id;
		  route_data.src_id = flit.src_id;
		  route_data.dst_id = flit.dst_id;
		  route_data.dir_in = i;

		  int o = route(route_data);


		  if (reservation_table.isAvailable(o)) 
		  {
		      reservation_table.reserve(i, o);
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
		      if (GlobalParams::verbose_mode > VERBOSE_OFF) 
			  LOG << "Input[" << i << "] forward to Output[" << o << "], flit: " << flit << endl << "Flit's Payload   : " 
                              << hex << flit.payload.data << endl << "Previous payload : " << previous_payload[o] << dec << endl;
                          //Erwan 22/06/15 check flit's payload and previous payload at this output
                      
		      flit_tx[o].write(flit);
                      
                      //Erwan,  define transitions between 2 following flits
                      if(GlobalParams::link_pwr_model == CROSSTALK_MODEL)
                         compute_nb_transitions(flit.payload.data,previous_payload[o],transition_types);
                      
                      if (GlobalParams::verbose_mode > VERBOSE_HIGH){
                        for(int i =0; i<22; i++)
                           cout << endl << "Transitions type " << i << " = " << transition_types[i];
                      
                      cout << endl;
                      }
                      
		      if (o == DIRECTION_HUB)
		      {
			  power.r2hLink();
			  LOG << "Forwarding to HUB " << endl;
                          //We are computing instant power for wired NoC only
                          //TODO make available for winoc
		      }
		      else
		      {
                          	 
                        //Erwan, set the direction for the map of energy consumption
                        power.setFlitDir(o);
                          
                        if(GlobalParams::link_pwr_model == CROSSTALK_MODEL){
                            //Instant power upated in r2rImproved_link
                            //if (o != DIRECTION_LOCAL)
                            power.r2rImproved_link(transition_types);        
                            
                            if(GlobalParams::alpha_trace){
                                //Write a new alpha value if direction is not local
                                if (o != DIRECTION_LOCAL){
                                    
                                      if(streamAlphaFile.is_open()){
                                            if (GlobalParams::verbose_mode > VERBOSE_MEDIUM)
                                                cout << "Instant Alpha file already opened" << endl;          
                                        }else{
                                            streamAlphaFile.open(alphaFileName.c_str(),ios::app);
                                            if (GlobalParams::verbose_mode > VERBOSE_MEDIUM)
                                                cout << "Opening Instant Alpha file" << endl;

                                            streamAlphaFile << setprecision (2) << fixed << current_alpha << "\n"; 
                                            streamAlphaFile.close();
                                        }
                                }
                            }
                            
                        }
                        if(GlobalParams::link_pwr_model == STATIC_MODEL){
                        power.r2rLink();
                        power.setTxPower(power.getTxPower() + power.getlink_r2r_pwr_d());
                        }
                                             
		      }

                      //Current flit is now saved as previous payload at this output
                      previous_payload[o] = flit.payload.data;
                      
		      power.crossBar();

		      current_level_tx[o] = 1 - current_level_tx[o];
		      req_tx[o].write(current_level_tx[o]);
		      buffer[i].Pop();

		      power.bufferPop();
                      
                      power.setTxPower(power.getTxPower() + power.getbuffer_pop_pwr_d() + power.getcrossbar_pwr_d());

		      // if flit has been consumed
		      if (flit.dst_id == local_id)
			  power.networkInterface();
                          power.setTxPower(power.getTxPower() + power.getni_pwr_d());

		      if (flit.flit_type == FLIT_TYPE_TAIL)
			  reservation_table.release(o);

		      // Update stats
		      if (o == DIRECTION_LOCAL) 
		      {
			  //LOG << "Consumed flit src " << flit.src_id << " dst = " << flit.dst_id << endl;
			  stats.receivedFlit(sc_time_stamp().to_double() / GlobalParams::clock_period_ps, flit);
			  if (GlobalParams::
				  max_volume_to_be_drained) 
			  {
			      if (drained_volume >=
				      GlobalParams::
				      max_volume_to_be_drained)
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
	      }
	  }
      }
    }				// else reset read
  if (GlobalParams::verbose_mode > VERBOSE_HIGH)
  cout << "fin update process TX du routeur " << local_id << " " << power.getTxPower() << endl;
}

NoP_data Router::getCurrentNoPData()
{
    NoP_data NoP_data;

    for (int j = 0; j < DIRECTIONS; j++) {
	NoP_data.channel_status_neighbor[j].free_slots =
	    free_slots_neighbor[j].read();
	NoP_data.channel_status_neighbor[j].available =
	    (reservation_table.isAvailable(j));
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
            //Instant power updated in leakage
	    power.leakage();
    }
    if (GlobalParams::verbose_mode > VERBOSE_HIGH)
    cout << "fin update leakage du routeur " << local_id << " " << power.getLeakagePower()<< endl;
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
            LOG << "Setting direction hub to reach destination node " << route_data.dst_id << endl;

            vector<int> dirv;
            dirv.push_back(DIRECTION_HUB);
            return dirv;
        }
    }
    //LOG << "Wired routing for dst = " << route_data.dst_id << endl;

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

    selectionStrategy->apply(this, directions, route_data);

    return 0;
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
	int flits =
	    GlobalParams::buffer_depth - free_slots_neighbor[i];
	if (flits >
	    (int) (GlobalParams::buffer_depth *
		   GlobalParams::dyad_threshold))
	    return true;
    }

    return false;
}

void Router::ShowBuffersStats(std::ostream & out)
{
  for (int i=0; i<DIRECTIONS+2; i++)
    buffer[i].ShowStats(out);
}

    
void Router::compute_nb_transitions(sc_uint<MAX_FLIT_PAYLOAD> cur_word,sc_uint<MAX_FLIT_PAYLOAD> previous_word, int array_transition[])
{
    int i,cpt;
    char all_transitions[GlobalParams::flit_size];
    
    
    // Clear transition arrays
    for(i=0;i<GlobalParams::flit_size;i++)
    {
        all_transitions[i] = 'L';
        
        if(i<22)
        transition_types[i] = 0;   
    }
    
    cpt = 0; 
    double cpt_alpha = 0;
    //Compute all transitions on each bitline
    for(i = 0; i < GlobalParams::flit_size; i++)
    {
        if((previous_word[i]==0)&&(cur_word[i]==0)){all_transitions[cpt]='L'; } // stay at Low
	if((previous_word[i]==0)&&(cur_word[i]==1)){
            all_transitions[cpt]='1'; // high level transition
            //Current alpha computing
            if(GlobalParams::alpha_trace)
                cpt_alpha++;
        } 
	if((previous_word[i]==1)&&(cur_word[i]==0)){
            all_transitions[cpt]='0'; // low level transition
            //Current alpha computing
            if(GlobalParams::alpha_trace)
                cpt_alpha++;
        } 
	if((previous_word[i]==1)&&(cur_word[i]==1)){all_transitions[cpt]='H'; } // stay at High  
        cpt++;

    }
    
    //Current alpha computing
    if(GlobalParams::alpha_trace)
        current_alpha = cpt_alpha / GlobalParams::flit_size;
    
    
    //Compute all transitions types that occurs with this 2 words between 3 wires by 3 wires
    for(i = 0;i < GlobalParams::flit_size;i++)
    {
        //If edges bitlines, we consider 1 wire that stays at low (L))
        if(i==0)
        {
            // (L)00    
	    if(((all_transitions[i]=='0')&&(all_transitions[i+1]=='0'))){transition_types[12]++;}   
            // (L)0L
	    if((all_transitions[i]=='0')&&(all_transitions[i+1]=='L')){transition_types[13]++;}
            // (L)01 
	    if(((all_transitions[i]=='0')&&(all_transitions[i+1]=='1'))){transition_types[14]++;}   
            // (L)1L   
	    if((all_transitions[i]=='1')&&(all_transitions[i+1]=='L')){transition_types[15]++;}
            // (L)10
	    if(((all_transitions[i]=='1')&&(all_transitions[i+1]=='0'))){transition_types[16]++;}   
            // (L)11
	    if(((all_transitions[i]=='1')&&(all_transitions[i+1]=='1'))){transition_types[17]++;}   
            // (L)0H 
	    if(((all_transitions[i]=='0')&&(all_transitions[i+1]=='H'))){transition_types[18]++;}   
            // (L)1H
	    if(((all_transitions[i]=='1')&&(all_transitions[i+1]=='H'))){transition_types[19]++;}   
            // xLx  
	    if(all_transitions[i]=='L'){transition_types[20]++;}
            // xHx  
	    if(all_transitions[i]=='H'){transition_types[21]++;}
            
        }else if(i==(GlobalParams::flit_size - 1))
            {
                // 00(L)   
                if(((all_transitions[i]=='0')&&(all_transitions[i-1]=='0'))){transition_types[12]++;}   
                // L0(L)
                if((all_transitions[i]=='0')&&(all_transitions[i-1]=='L')){transition_types[13]++;}
                // 10(L)
                if(((all_transitions[i]=='0')&&(all_transitions[i-1]=='1'))){transition_types[14]++;}   
                // L1(L)   
                if((all_transitions[i]=='1')&&(all_transitions[i-1]=='L')){transition_types[15]++;}
                // 01(L)
                if(((all_transitions[i]=='1')&&(all_transitions[i-1]=='0'))){transition_types[16]++;}   
                // 11(L)
                if(((all_transitions[i]=='1')&&(all_transitions[i-1]=='1'))){transition_types[17]++;}   
                // H0(L)
                if(((all_transitions[i]=='0')&&(all_transitions[i-1]=='H'))){transition_types[18]++;}   
                // H1(L)   
                if(((all_transitions[i]=='1')&&(all_transitions[i-1]=='H'))){transition_types[19]++;}   
                // xLx  
                if(all_transitions[i]=='L'){transition_types[20]++;}
                // xHx  
                if(all_transitions[i]=='H'){transition_types[21]++;}
               
            
            }else{
                // 000
                if((all_transitions[i-1]=='0')&&(all_transitions[i]=='0')&&(all_transitions[i+1]=='0')){transition_types[0]++;}
                // 010   
		if((all_transitions[i-1]=='0')&&(all_transitions[i]=='1')&&(all_transitions[i+1]=='0')){transition_types[1]++;}
                // 100 001
		if(((all_transitions[i-1]=='1')&&(all_transitions[i]=='0')&&(all_transitions[i+1]=='0'))||
                  ((all_transitions[i-1]=='0')&&(all_transitions[i]=='0')&&(all_transitions[i+1]=='1'))){transition_types[2]++;}
                // 101   
		if((all_transitions[i-1]=='1')&&(all_transitions[i]=='0')&&(all_transitions[i+1]=='1')){transition_types[3]++;}
                // 110 011
		if(((all_transitions[i-1]=='1')&&(all_transitions[i]=='1')&&(all_transitions[i+1]=='0'))||
		  ((all_transitions[i-1]=='0')&&(all_transitions[i]=='1')&&(all_transitions[i+1]=='1'))){transition_types[4]++;}   
                // 111   
		if((all_transitions[i-1]=='1')&&(all_transitions[i]=='1')&&(all_transitions[i+1]=='1')){transition_types[5]++;}
                // H00 00H
		if(((all_transitions[i-1]=='H')&&(all_transitions[i]=='0')&&(all_transitions[i+1]=='0'))||
		  ((all_transitions[i-1]=='0')&&(all_transitions[i]=='0')&&(all_transitions[i+1]=='H'))){transition_types[6]++;}   
                // H0H   
		if((all_transitions[i-1]=='H')&&(all_transitions[i]=='0')&&(all_transitions[i+1]=='H')){transition_types[7]++;}
                // H01 10H
		if(((all_transitions[i-1]=='H')&&(all_transitions[i]=='0')&&(all_transitions[i+1]=='1'))||
		  ((all_transitions[i-1]=='1')&&(all_transitions[i]=='0')&&(all_transitions[i+1]=='H'))){transition_types[8]++;}   
                //H1H   
		if((all_transitions[i-1]=='H')&&(all_transitions[i]=='1')&&(all_transitions[i+1]=='H')){transition_types[9]++;}
                //H10 01H
		if(((all_transitions[i-1]=='H')&&(all_transitions[i]=='1')&&(all_transitions[i+1]=='0'))||
		  ((all_transitions[i-1]=='0')&&(all_transitions[i]=='1')&&(all_transitions[i+1]=='H'))){transition_types[10]++;}   
                // H11 11H   
		if(((all_transitions[i-1]=='H')&&(all_transitions[i]=='1')&&(all_transitions[i+1]=='1'))||
		  ((all_transitions[i-1]=='1')&&(all_transitions[i]=='1')&&(all_transitions[i+1]=='H'))){transition_types[11]++;}   
                // L00 00L   
		if(((all_transitions[i-1]=='L')&&(all_transitions[i]=='0')&&(all_transitions[i+1]=='0'))||
		  ((all_transitions[i-1]=='0')&&(all_transitions[i]=='0')&&(all_transitions[i+1]=='L'))){transition_types[12]++;}   
                // L0L
		if((all_transitions[i-1]=='L')&&(all_transitions[i]=='0')&&(all_transitions[i+1]=='L')){transition_types[13]++;}
                // L01 10L
		if(((all_transitions[i-1]=='L')&&(all_transitions[i]=='0')&&(all_transitions[i+1]=='1'))||
		  ((all_transitions[i-1]=='1')&&(all_transitions[i]=='0')&&(all_transitions[i+1]=='L'))){transition_types[14]++;}   
                // L1L   
		if((all_transitions[i-1]=='L')&&(all_transitions[i]=='1')&&(all_transitions[i+1]=='L')){transition_types[15]++;}
                // L10 01L
		if(((all_transitions[i-1]=='L')&&(all_transitions[i]=='1')&&(all_transitions[i+1]=='0'))||
		  ((all_transitions[i-1]=='0')&&(all_transitions[i]=='1')&&(all_transitions[i+1]=='L'))){transition_types[16]++;}   
                // 11L L11
		if(((all_transitions[i-1]=='1')&&(all_transitions[i]=='1')&&(all_transitions[i+1]=='L'))||
		  ((all_transitions[i-1]=='L')&&(all_transitions[i]=='1')&&(all_transitions[i+1]=='1'))){transition_types[17]++;}   
                // L0H H0L
		if(((all_transitions[i-1]=='L')&&(all_transitions[i]=='0')&&(all_transitions[i+1]=='H'))||
		  ((all_transitions[i-1]=='H')&&(all_transitions[i]=='0')&&(all_transitions[i+1]=='L'))){transition_types[18]++;}   
                // L1H H1L   
		if(((all_transitions[i-1]=='L')&&(all_transitions[i]=='1')&&(all_transitions[i+1]=='H'))||
		  ((all_transitions[i-1]=='H')&&(all_transitions[i]=='1')&&(all_transitions[i+1]=='L'))){transition_types[19]++;}   
                // xLx  
		if(all_transitions[i]=='L'){transition_types[20]++;}
                // xHx  
		if(all_transitions[i]=='H'){transition_types[21]++;}
               
        }   
    }
}



