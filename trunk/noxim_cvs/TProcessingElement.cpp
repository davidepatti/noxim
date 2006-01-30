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
        cout << sc_simulation_time() << ": ProcessingElement[" << id << "] RECEIVING " << flit_tmp << endl;
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
          cout << sc_simulation_time() << ": ProcessingElement[" << id << "] SENDING " << flit << endl;
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
  if(!transmittedAtPreviousCycle)
  {
    if(TGlobalParams::traffic_distribution==TRAFFIC_TABLE_BASED)
    {
      threshold = traffic_table->getPirForTheSelectedLink(p.src_id, p.dst_id);
    }
    else
    {
      threshold = TGlobalParams::packet_injection_rate;
    }
  }
  else
  {
    if(TGlobalParams::traffic_distribution==TRAFFIC_TABLE_BASED)
    {
      threshold = traffic_table->getPorForTheSelectedLink(p.src_id, p.dst_id);
    }
    else
    {
      threshold = TGlobalParams::probability_of_retransmission;
    }
  }

  if( ((double)rand())/RAND_MAX < threshold)
    return true;
  else
    return false;
  
/* wormhole test
  static int s1 = 0;
  static int s2 = 0;
  TCoord position = id2Coord(id);
  if(s1 != 0 && s2 != 0) return false;
  if( (position.x == 0 && position.y == 0) && s1 == 0)
    {
      s1++;
      return true;
    }
  if( (position.x == 1 && position.y == 0) && s2 == 0)
    {
      s2++;
      return true;
    }
  return false;
*/

}

//---------------------------------------------------------------------------

TPacket TProcessingElement::nextPacket()
{
  switch(TGlobalParams::traffic_distribution)
  {
    case TRAFFIC_UNIFORM:
      return trafficUniform();
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
      return trafficUniform();
  }
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficUniform()
{
  TPacket p;
  p.src_id = id;

  // Uniform destination distribution
  do
  {
    p.dst_id = randInt(0, (TGlobalParams::mesh_dim_x * TGlobalParams::mesh_dim_y)-1);
  } while(p.dst_id==p.src_id);

  p.timestamp = sc_simulation_time();
  p.size = p.flit_left = randInt(TGlobalParams::min_packet_size, 
				 TGlobalParams::max_packet_size); // 2 + (rand() % TGlobalParams::max_packet_size);

  return p;
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficTranspose1()
{
  TPacket p;
  p.src_id = id;
  TCoord src,dst;

  // Transpose 1 destination distribution
  src.x = id2Coord(p.src_id).x;
  src.y = id2Coord(p.src_id).y;
  dst.x = TGlobalParams::mesh_dim_x-1-src.y;
  dst.y = TGlobalParams::mesh_dim_y-1-src.x;
  fixRanges(src, dst);
  p.dst_id = coord2Id(dst);

  p.timestamp = sc_simulation_time();
  
  p.size = p.flit_left = randInt(TGlobalParams::min_packet_size, 
				 TGlobalParams::max_packet_size); 
  return p;
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficTranspose2()
{
  TPacket p;
  p.src_id = id;
  TCoord src,dst;

  // Transpose 2 destination distribution
  src.x = id2Coord(p.src_id).x;
  src.y = id2Coord(p.src_id).y;
  dst.x = src.y;
  dst.y = src.x;
  fixRanges(src, dst);
  p.dst_id = coord2Id(dst);

  p.timestamp = sc_simulation_time();

  p.size = p.flit_left = randInt(TGlobalParams::min_packet_size, 
				 TGlobalParams::max_packet_size);

  return p;
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficTableBased()
{
  TPacket p;
  p.src_id = id;

  // Traffic Table Based destination distribution
  p.dst_id = traffic_table->randomDestinationGivenTheSource(p.src_id);

  p.timestamp = sc_simulation_time();
  
  p.size = p.flit_left = randInt(TGlobalParams::min_packet_size, 
				 TGlobalParams::max_packet_size);

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
