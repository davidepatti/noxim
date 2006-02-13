/*****************************************************************************

  TProcessingElement.cpp -- Processing Element (PE) implementation

 *****************************************************************************/

#include "TProcessingElement.h"

//---------------------------------------------------------------------------

int TProcessingElement::randInt(int min, int max)
{
  return min + (int)((double)(max-min+1) * rand()/(RAND_MAX+1.0));
}

//---------------------------------------------------------------------------

void TProcessingElement::rxProcess()
{
  if(reset.read())
  {
    ack_rx.write(0);
    current_level_rx = 0;
  }
  else
  {
    if(req_rx.read()==1-current_level_rx)
    {
      TFlit flit_tmp = flit_rx.read();
      if(TGlobalParams::verbose_mode > VERBOSE_OFF)
      {
        cout << sc_simulation_time() << ": ProcessingElement[" << local_id << "] RECEIVING " << flit_tmp << endl;
      }
      current_level_rx = 1-current_level_rx;     // Negate the old value for Alternating Bit Protocol (ABP)
    }
    ack_rx.write(current_level_rx);
  }
}

//---------------------------------------------------------------------------

void TProcessingElement::txProcess()
{
  if(reset.read())
  {
    req_tx.write(0);
    current_level_tx = 0;
    transmittedAtPreviousCycle = false;
  }
  else
  {
    TPacket transmittible_packet = nextPacket();
    if(probabilityShot(transmittible_packet))
    {
      packet_queue.push(transmittible_packet);
      transmittedAtPreviousCycle = true;
    }
    else
      transmittedAtPreviousCycle = false;


    if(ack_tx.read() == current_level_tx)
    {
      if(!packet_queue.empty())
      {
        TFlit flit = nextFlit();                  // Generate a new flit
        if(TGlobalParams::verbose_mode > VERBOSE_OFF)
        {
          cout << sc_simulation_time() << ": ProcessingElement[" << local_id << "] SENDING " << flit << endl;
        }
	flit_tx->write(flit);                     // Send the generated flit
	current_level_tx = 1-current_level_tx;    // Negate the old value for Alternating Bit Protocol (ABP)
	req_tx.write(current_level_tx);
      }
    }
  }
}

//---------------------------------------------------------------------------

TFlit TProcessingElement::nextFlit()
{
  TFlit   flit;
  TPacket packet = packet_queue.front();

  flit.src_id      = packet.src_id;
  flit.dst_id      = packet.dst_id;
  flit.timestamp   = packet.timestamp;
  flit.sequence_no = packet.size - packet.flit_left;
  flit.hop_no      = 0;
  //  flit.payload     = DEFAULT_PAYLOAD;

  if(packet.size == packet.flit_left)
    flit.flit_type = FLIT_TYPE_HEAD;
  else if(packet.flit_left == 1)
    flit.flit_type = FLIT_TYPE_TAIL;
  else
    flit.flit_type = FLIT_TYPE_BODY;
  
  packet_queue.front().flit_left--;
  if(packet_queue.front().flit_left == 0)
    packet_queue.pop();

  return flit;
}

//---------------------------------------------------------------------------

bool TProcessingElement::probabilityShot(TPacket p)
{
  float threshold;

  // Normal case (using global parameters)
  if(TGlobalParams::traffic_distribution!=TRAFFIC_TABLE_BASED)
  {
    if(!transmittedAtPreviousCycle)
      threshold = TGlobalParams::packet_injection_rate;
    else
      threshold = TGlobalParams::probability_of_retransmission;
  }
  else
  // Traffic Table Based (using the proper parameter for each link)
  {
    if(occurrencesInTrafficTableAsSource==0) return false;

    int now = (int)sc_simulation_time();
    int t_on = traffic_table->getTonForTheSelectedLink(p.src_id, p.dst_id);
    int t_off = traffic_table->getToffForTheSelectedLink(p.src_id, p.dst_id);
    int t_period = traffic_table->getTperiodForTheSelectedLink(p.src_id, p.dst_id);
    if(t_period<=0) t_period = DEFAULT_RESET_TIME + TGlobalParams::simulation_time;

    if((now%t_period)>t_on && (now%t_period)<t_off)
    {
      if(!transmittedAtPreviousCycle)
        threshold = traffic_table->getPirForTheSelectedLink(p.src_id, p.dst_id);
      else
        threshold = traffic_table->getPorForTheSelectedLink(p.src_id, p.dst_id);
    }
    else return false;
  }


  if( ((double)rand())/RAND_MAX < threshold)
    return true;
  else
    return false;
  
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::nextPacket()
{
  switch(TGlobalParams::traffic_distribution)
  {
    case TRAFFIC_RANDOM:
      return trafficRandom();
      break;

    case TRAFFIC_TRANSPOSE1:
      return trafficTranspose1();
      break;

    case TRAFFIC_TRANSPOSE2:
      return trafficTranspose2();
      break;

    case TRAFFIC_TABLE_BASED:
      return trafficTableBased();
      break;

    default:
      assert(false);
      return trafficRandom();
  }
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficRandom()
{
  TPacket p;
  p.src_id = local_id;
  double rnd = rand()/(double)RAND_MAX;
  double range_start = 0.0;

  //cout << "\n " << sc_simulation_time() << " PE " << local_id << " rnd = " << rnd << endl;

  int max_id = (TGlobalParams::mesh_dim_x * TGlobalParams::mesh_dim_y)-1;

  // Random destination distribution
  do
  {
    p.dst_id = randInt(0, max_id);

    // check for hotspot destination
    for (uint i = 0; i<TGlobalParams::hotspots.size(); i++)
    {
	//cout << sc_simulation_time() << " PE " << local_id << " Checking node " << TGlobalParams::hotspots[i].first << " with P = " << TGlobalParams::hotspots[i].second << endl;

	if (rnd>=range_start && rnd < range_start + TGlobalParams::hotspots[i].second)
	{
	    if (local_id != TGlobalParams::hotspots[i].first)
	    {
		//cout << sc_simulation_time() << " PE " << local_id <<" That is ! " << endl;
		p.dst_id = TGlobalParams::hotspots[i].first;
	    }
	    break;
	}
	else 
	    range_start+=TGlobalParams::hotspots[i].second; // try next
    }
  } while(p.dst_id==p.src_id);

  p.timestamp = sc_simulation_time();
  p.size = p.flit_left = getRandomSize();

  return p;
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficTranspose1()
{
  TPacket p;
  p.src_id = local_id;
  TCoord src,dst;

  // Transpose 1 destination distribution
  src.x = id2Coord(p.src_id).x;
  src.y = id2Coord(p.src_id).y;
  dst.x = TGlobalParams::mesh_dim_x-1-src.y;
  dst.y = TGlobalParams::mesh_dim_y-1-src.x;
  fixRanges(src, dst);
  p.dst_id = coord2Id(dst);

  p.timestamp = sc_simulation_time();
  p.size = p.flit_left = getRandomSize();

  return p;
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficTranspose2()
{
  TPacket p;
  p.src_id = local_id;
  TCoord src,dst;

  // Transpose 2 destination distribution
  src.x = id2Coord(p.src_id).x;
  src.y = id2Coord(p.src_id).y;
  dst.x = src.y;
  dst.y = src.x;
  fixRanges(src, dst);
  p.dst_id = coord2Id(dst);

  p.timestamp = sc_simulation_time();
  p.size = p.flit_left = getRandomSize();

  return p;
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficTableBased()
{
  TPacket p;
  p.src_id = local_id;

  // Traffic Table Based destination distribution
  p.dst_id = traffic_table->randomDestinationGivenTheSource(p.src_id);

  p.timestamp = sc_simulation_time();
  p.size = p.flit_left = getRandomSize();

  return p;
}

//---------------------------------------------------------------------------

void TProcessingElement::fixRanges(const TCoord src, TCoord& dst)
{
  // Fix ranges
  if(dst.x<0) dst.x=0;
  if(dst.y<0) dst.y=0;
  if(dst.x>=TGlobalParams::mesh_dim_x) dst.x=TGlobalParams::mesh_dim_x-1;
  if(dst.y>=TGlobalParams::mesh_dim_y) dst.y=TGlobalParams::mesh_dim_y-1;
}

//---------------------------------------------------------------------------

int TProcessingElement::getRandomSize()
{
  return randInt(TGlobalParams::min_packet_size, 
                 TGlobalParams::max_packet_size);
}

//---------------------------------------------------------------------------
