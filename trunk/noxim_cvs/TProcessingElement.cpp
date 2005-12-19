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
      packet_queue.push(nextPacket());
    
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

  switch(TGlobalParams::traffic_distribution)
  {
    case TRAFFIC_UNIFORM:
      do {
        p.dst_id = rand() % (TGlobalParams::mesh_dim_x * TGlobalParams::mesh_dim_y);
      } while(p.dst_id==p.src_id);
      break;
  }

  p.timestamp = sc_simulation_time();
  p.size = p.flit_left = 2 + (rand() % TGlobalParams::max_packet_size);
  return p;
}


