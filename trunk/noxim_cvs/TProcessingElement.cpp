/*****************************************************************************

  TProcessingElement.cpp -- Processing Element (PE) implementation

 *****************************************************************************/

#include "TProcessingElement.h"

//---------------------------------------------------------------------------

void TProcessingElement::rxProcess()
{
  // This function is a simplified version of the corresponding method of the TRouter class
  if(reset.read())
  {
    ack_rx.write(0);
    current_level_rx = 0;
  }
  else
  {
    if(req_rx.read()==1-current_level_rx)
    {
      // Check and print received data (TO BE ADDED)
      TFlit flit_tmp = flit_rx.read();
      current_level_rx = 1-current_level_rx;     // Negate the old value for Alternating Bit Protocol (ABP)
    }
    ack_rx.write(current_level_rx);
  }
}

void TProcessingElement::txProcess()
{
  // This function is a simplified version of the corresponding method of the TRouter class
  if(reset.read())
  {
    req_tx.write(0);
    current_level_tx = 0;
  }
  else
  {
    if (probabilityShot())
    {
      TPacket p = nextPacket();
      // by Fafa      if(p!=NULL) packet_queue.push(p);           // In some cases (e.g. Traffic Table Based) packet generation is disabled
      packet_queue.push(p);           // In some cases (e.g. Traffic Table Based) packet generation is disabled
    }
    if (ack_tx.read() == current_level_tx)
    {
      if (!packet_queue.empty())
      {
        TFlit flit = nextFlit();                  // Generate a new flit
	flit_tx->write(flit);                     // Send the generated flit
	current_level_tx = 1-current_level_tx;    // Negate the old value for Alternating Bit Protocol (ABP)
	req_tx.write(current_level_tx);
      }
    }
  }
}

TFlit TProcessingElement::nextFlit()
{
  TFlit   flit;
  TPacket packet = packet_queue.front();

  flit.src_id      = packet.src_id;
  flit.dst_id      = packet.dst_id;
  flit.timestamp   = packet.timestamp;
  flit.sequence_no = packet.size - packet.flit_left;
  flit.hop_no      = 0;
  //  flit.payload     = DEF_PAYLOAD;

  if (packet.size == packet.flit_left)
    flit.flit_type = FLIT_TYPE_HEAD;
  else if (packet.flit_left == 1)
    flit.flit_type = FLIT_TYPE_TAIL;
  else
    flit.flit_type = FLIT_TYPE_BODY;
  
  packet_queue.front().flit_left--;
  if (packet_queue.front().flit_left == 0)
    packet_queue.pop();

  return flit;
}

int TProcessingElement::probabilityShot()
{
  if ((double)rand()/RAND_MAX <= TGlobalParams::packet_injection_rate)
    return true;

  return false;
  
  /*---------------- wormhole test ---------------*/
//   static int s1 = 0;
//   static int s2 = 0;

//   TCoord position = id2Coord(id);

//   if (s1 != 0 && s2 != 0) return false;

//   if ( (position.x == 0 && position.y == 0) && s1 == 0)
//     {
//       s1++;
//       return true;
//     }

//   if ( (position.x == 1 && position.y == 0) && s2 == 0)
//     {
//       s2++;
//       return true;
//     }
  
//   return false;
  /*----------------------------------------*/
}


TPacket TProcessingElement::nextPacket()
{
  TPacket p;
  p.src_id = id;
  TCoord src,dst;

  switch(TGlobalParams::traffic_distribution)
  {
    case TRAFFIC_UNIFORM:
      do {
        p.dst_id = rand() % (TGlobalParams::mesh_dim_x * TGlobalParams::mesh_dim_y);
      } while(p.dst_id==p.src_id);
      break;

    case TRAFFIC_TRANSPOSE1:
      src.x = id2Coord(p.src_id).x;
      src.y = id2Coord(p.src_id).y;
      dst.x = TGlobalParams::mesh_dim_x-1-src.y;
      dst.y = TGlobalParams::mesh_dim_y-1-src.x;
      fixRanges(src, dst);
      p.dst_id = coord2Id(dst);
      break;

    case TRAFFIC_TRANSPOSE2:
      src.x = id2Coord(p.src_id).x;
      src.y = id2Coord(p.src_id).y;
      dst.x = src.y;
      dst.y = src.x;
      fixRanges(src, dst);
      p.dst_id = coord2Id(dst);
      break;
      /* by Fafa
    case TRAFFIC_TTABLE_BASED:
      if(true) return NULL; // if there is no occurrence in the table
      else do {
        p.dst_id = rand() % (TGlobalParams::mesh_dim_x * TGlobalParams::mesh_dim_y);
      } while(p.dst_id==p.src_id);
      break;
      */
    default:
      assert(false);
  }

  p.timestamp = sc_simulation_time();
  p.size = p.flit_left = 2 + (rand() % TGlobalParams::max_packet_size);
  return p;
}

void TProcessingElement::fixRanges(const TCoord src, TCoord& dst)
{
  // Fix ranges
  if(dst.x<0) dst.x=0;
  if(dst.y<0) dst.y=0;
  if(dst.x>=TGlobalParams::mesh_dim_x) dst.x=TGlobalParams::mesh_dim_x-1;
  if(dst.y>=TGlobalParams::mesh_dim_y) dst.y=TGlobalParams::mesh_dim_y-1;
}
