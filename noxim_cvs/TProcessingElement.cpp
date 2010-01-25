/*****************************************************************************

  TProcessingElement.cpp -- Processing Element (PE) implementation

 *****************************************************************************/
/* Copyright 2005-2007  
    Maurizio Palesi <mpalesi@diit.unict.it>
    Fabrizio Fazzino <fabrizio.fazzino@diit.unict.it>
    Davide Patti <dpatti@diit.unict.it>

 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
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
      TFlit flit = flit_rx.read();
      
      // checks if the incoming packets claims for an ack
      if (flit.flit_type == FLIT_TYPE_TAIL && flit.claims_ack)
      {
	TAck ack(flit.src_id, flit.dst_id, 2); // ack is 2 flits long (head+tail)
	acks_to_send.push(ack);
	/*
	if (local_id == 30 && flit.src_id==12 && flit.dst_id==30)
	  cout << local_id << ": acks_to_send.push @ " << sc_time_stamp().to_double()/1000 << endl;
	*/
      }

      // checks if the incoming packet is an ack
      if (flit.flit_type == FLIT_TYPE_TAIL && flit.ack)
      {
	// removes the ack from the set of the acks it is waiting for
	set<int>::iterator i = acks_to_receive.find(flit.src_id);
	assert(i != acks_to_receive.end());
	acks_to_receive.erase(i);
	/*
	if (flit.src_id == 30)
	  cout << local_id << ": received ack from 30, ts=" << flit.timestamp << " @ " << sc_time_stamp().to_double()/1000 << endl;
	*/
      }

      if(TGlobalParams::verbose_mode > VERBOSE_OFF)
      {
        cout << sc_simulation_time() << ": ProcessingElement[" << local_id << "] RECEIVING " << flit << endl;
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
    // acks have priority over packets to be sent
    while (!acks_to_send.empty())
    {
      TAck ack = acks_to_send.front(); 
      acks_to_send.pop();
      TPacket packet = ack.makePacket(sc_time_stamp().to_double()/1000);
      packet_queue.push(packet);
      /*
      if (packet.src_id == 30 && packet.dst_id == 12)
	cout << local_id << ": packet_queue.push ack(30->12) " << " @ " << sc_time_stamp().to_double()/1000 << endl;
      */
    }

    // now data packets can be sent
    vector<TPacket> communication;
    if (canShot(communication))
    {
      for (vector<TPacket>::iterator i=communication.begin(); i!=communication.end(); i++)
      {
	/*
	if (i->src_id == 12 && i->dst_id == 30) 
	  cout << local_id << ": Injected 12->30, ts=" << i->timestamp << " @ " << sc_time_stamp().to_double()/1000 << endl;
	*/
	packet_queue.push(*i);
	if (i->claims_ack)
	  acks_to_receive.insert(i->dst_id);
      }
      transmittedAtPreviousCycle = true;
    }
    else
      transmittedAtPreviousCycle = false;


    if (ack_tx.read() == current_level_tx)
    {
      if (!packet_queue.empty())
      {
        TFlit flit = nextFlit();                  // Generate a new flit
        if (TGlobalParams::verbose_mode > VERBOSE_OFF)
        {
          cout << sc_time_stamp().to_double()/1000 << ": ProcessingElement[" << local_id << "] SENDING " << flit << endl;
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

  flit.src_id       = packet.src_id;
  flit.dst_id       = packet.dst_id;
  flit.timestamp    = packet.timestamp;
  flit.sequence_no  = packet.size - packet.flit_left;
  flit.hop_no       = 0;
  //  flit.payload     = DEFAULT_PAYLOAD;
  flit.ack          = packet.ack;
  flit.claims_ack   = packet.claims_ack;
  flit.comm_id      = packet.comm_id;
  flit.comm_size    = packet.comm_size;
  flit.packet_seqno = packet.packet_seqno;

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

//---------------------------------------------------------------------------

// bool TProcessingElement::canShot(TPacket& packet)
bool TProcessingElement::canShot(vector<TPacket>& comm)
{
  bool    shot;
  TPacket packet;

  if (TGlobalParams::traffic_distribution != TRAFFIC_TABLE_BASED)
    {
      double  threshold;

      if (!transmittedAtPreviousCycle)
	threshold = TGlobalParams::packet_injection_rate;
      else
	threshold = TGlobalParams::probability_of_retransmission;

      shot = (((double)rand())/RAND_MAX < threshold);
      if (shot)
	{
	  switch(TGlobalParams::traffic_distribution)
	    {
	    case TRAFFIC_RANDOM:
	      packet = trafficRandom();
	      break;
	      
	    case TRAFFIC_TRANSPOSE1:
	      packet = trafficTranspose1();
	      break;
	      
	    case TRAFFIC_TRANSPOSE2:
	      packet = trafficTranspose2();
	      break;
	      
	    case TRAFFIC_BIT_REVERSAL:
	      packet = trafficBitReversal();
	      break;

	    case TRAFFIC_SHUFFLE:
	      packet = trafficShuffle();
	      break;

	    case TRAFFIC_BUTTERFLY:
	      packet = trafficButterfly();
	      break;

	    default:
	      assert(false);
	    }
	}
    }
  else
    { // Table based communication traffic
      if (never_transmit)
	return false;

      double now         = sc_time_stamp().to_double()/1000;
      bool   use_pir     = (transmittedAtPreviousCycle == false);
      vector<pair<int,double> > dst_prob;
      double threshold = traffic_table->getCumulativePirPor(local_id, (int)now, use_pir, dst_prob);

      double prob = (double)rand()/RAND_MAX;
      shot = (prob < threshold);
      if (shot)
	{
	  for (unsigned int i=0; i<dst_prob.size(); i++)
	    {
	      if (prob < dst_prob[i].second) 
		{
		  packet.make(local_id, dst_prob[i].first, now, getRandomPacketSize());
		  break;
		}
	    }
	}
    }
 

  if (shot) {
    // The packet is ready to be shot but we mush check if this node
    // is not waiting an ack from the destination of this packet
    bool ack_received = (acks_to_receive.find(packet.dst_id) == acks_to_receive.end());
    if (!ack_received)
    {
      packet_queue_waiting_for_ack[packet.dst_id].push(packet);
      shot = false;
      /*
      if (packet.src_id == 12 && packet.dst_id == 30) 
	cout << local_id << ": " << "packet_queue_waiting_for_ack, ts=" << packet.timestamp << " @ " << sc_time_stamp().to_double()/1000 << endl;
      */
    }
  } 

  if (!shot) // Note: do not use the else as shot is modified inside the above if
  {
    // Check if there is a packet in the packet_queue_waiting_for_ack
    // that can be sent because the ack has been received. In case
    // there are multiple packets in this state, select one randomly
    shot = getRndPacketFromPQWFA(packet);
  }

  if (shot)
  {
    //    packet = packet_queue_waiting_for_ack[packet.dst_id].front();
    //    packet_queue_waiting_for_ack[packet.dst_id].pop();

    // convert packet to communication
    int comm_size = getRandomCommunicationSize();
    bool blocking = (rand()/(double)RAND_MAX) < TGlobalParams::comm_blocking_probability;
    comm = packet.makeCommunication(vcomms_id[packet.dst_id], comm_size, blocking);
    vcomms_id[packet.dst_id]++;
  } 
  else {
    /*
    if (packet.src_id == 12 && packet.dst_id == 30) 
      cout << local_id << ": " << "cannot send waiting ack from " << packet.dst_id <<", ts=" << packet.timestamp << " @ " << sc_time_stamp().to_double()/1000 << endl;
    */
  }

  return shot;
}

//---------------------------------------------------------------------------

bool TProcessingElement::getRndPacketFromPQWFA(TPacket& packet)
{
  vector<map<int,queue<TPacket> >::iterator> vi;

  for (map<int,queue<TPacket> >::iterator i=packet_queue_waiting_for_ack.begin();
       i!=packet_queue_waiting_for_ack.end(); i++)
  {
    if (!i->second.empty())
    {
      TPacket pkt = i->second.front();
      bool ack_received = (acks_to_receive.find(pkt.dst_id) == acks_to_receive.end());
      if (ack_received)
	vi.push_back(i);
    }
  }

  if (vi.empty())
    return false;
  
  int idx_rnd = rand() % vi.size();
  packet = vi[idx_rnd]->second.front();    
  vi[idx_rnd]->second.pop();
  return true;
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficRandom()
{
  TPacket p;
  p.src_id = local_id;
  double rnd = rand()/(double)RAND_MAX;
  double range_start = 0.0;

  //cout << "\n " << sc_time_stamp().to_double()/1000 << " PE " << local_id << " rnd = " << rnd << endl;

  int max_id = (TGlobalParams::mesh_dim_x * TGlobalParams::mesh_dim_y)-1;

  // Random destination distribution
  do
  {
    p.dst_id = randInt(0, max_id);

    // check for hotspot destination
    for (uint i = 0; i<TGlobalParams::hotspots.size(); i++)
    {
	//cout << sc_time_stamp().to_double()/1000 << " PE " << local_id << " Checking node " << TGlobalParams::hotspots[i].first << " with P = " << TGlobalParams::hotspots[i].second << endl;

	if (rnd>=range_start && rnd < range_start + TGlobalParams::hotspots[i].second)
	{
	    if (local_id != TGlobalParams::hotspots[i].first)
	    {
		//cout << sc_time_stamp().to_double()/1000 << " PE " << local_id <<" That is ! " << endl;
		p.dst_id = TGlobalParams::hotspots[i].first;
	    }
	    break;
	}
	else 
	    range_start+=TGlobalParams::hotspots[i].second; // try next
    }
  } while(p.dst_id==p.src_id);

  p.timestamp = sc_time_stamp().to_double()/1000;
  p.size = p.flit_left = getRandomPacketSize();

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

  p.timestamp = sc_time_stamp().to_double()/1000;
  p.size = p.flit_left = getRandomPacketSize();

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

  p.timestamp = sc_time_stamp().to_double()/1000;
  p.size = p.flit_left = getRandomPacketSize();

  return p;
}

//---------------------------------------------------------------------------

void TProcessingElement::setBit(int &x, int w, int v)
{
  int mask = 1 << w;
  
  if (v == 1)
    x = x | mask;
  else if (v == 0)
    x = x & ~mask;
  else
    assert(false);    
}

//---------------------------------------------------------------------------

int TProcessingElement::getBit(int x, int w)
{
  return (x >> w) & 1;
}

//---------------------------------------------------------------------------

inline double TProcessingElement::log2ceil(double x)
{
  return ceil(log(x)/log(2.0));
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficBitReversal()
{
  
  int nbits = (int)log2ceil((double)(TGlobalParams::mesh_dim_x*TGlobalParams::mesh_dim_y));
  int dnode = 0;
  for (int i=0; i<nbits; i++)
    setBit(dnode, i, getBit(local_id, nbits-i-1));

  TPacket p;
  p.src_id = local_id;
  p.dst_id = dnode;

  p.timestamp = sc_time_stamp().to_double()/1000;
  p.size = p.flit_left = getRandomPacketSize();

  return p;
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficShuffle()
{
  
  int nbits = (int)log2ceil((double)(TGlobalParams::mesh_dim_x*TGlobalParams::mesh_dim_y));
  int dnode = 0;
  for (int i=0; i<nbits-1; i++)
    setBit(dnode, i+1, getBit(local_id, i));
  setBit(dnode, 0, getBit(local_id, nbits-1));

  TPacket p;
  p.src_id = local_id;
  p.dst_id = dnode;

  p.timestamp = sc_time_stamp().to_double()/1000;
  p.size = p.flit_left = getRandomPacketSize();

  return p;
}

//---------------------------------------------------------------------------

TPacket TProcessingElement::trafficButterfly()
{
  
  int nbits = (int)log2ceil((double)(TGlobalParams::mesh_dim_x*TGlobalParams::mesh_dim_y));
  int dnode = 0;
  for (int i=1; i<nbits-1; i++)
    setBit(dnode, i, getBit(local_id, i));
  setBit(dnode, 0, getBit(local_id, nbits-1));
  setBit(dnode, nbits-1, getBit(local_id, 0));

  TPacket p;
  p.src_id = local_id;
  p.dst_id = dnode;

  p.timestamp = sc_time_stamp().to_double()/1000;
  p.size = p.flit_left = getRandomPacketSize();

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

int TProcessingElement::getRandomPacketSize()
{
  return randInt(TGlobalParams::min_packet_size, 
                 TGlobalParams::max_packet_size);
}

//---------------------------------------------------------------------------

int TProcessingElement::getRandomCommunicationSize()
{
  return randInt(TGlobalParams::min_communication_size, 
                 TGlobalParams::max_communication_size);
}

//---------------------------------------------------------------------------
