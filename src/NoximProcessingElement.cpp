/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the processing element
 */

#include "NoximProcessingElement.h"

int NoximProcessingElement::randInt(int min, int max)
{
    return min +
	(int) ((double) (max - min + 1) * rand() / (RAND_MAX + 1.0));
}

void NoximProcessingElement::rxProcess()
{
    if (reset.read()) {
	ack_rx.write(0);
	current_level_rx = 0;
    } else {
	if (req_rx.read() == 1 - current_level_rx) {
	    NoximFlit flit_tmp = flit_rx.read();
	    if (NoximGlobalParams::verbose_mode > VERBOSE_OFF) {
		cout << sc_simulation_time() << ": ProcessingElement[" <<
		    local_id << "] RECEIVING " << flit_tmp << endl;
	    }
	    current_level_rx = 1 - current_level_rx;	// Negate the old value for Alternating Bit Protocol (ABP)
	}
	ack_rx.write(current_level_rx);
    }
}

void NoximProcessingElement::txProcess()
{
    if (reset.read()) {
	req_tx.write(0);
	current_level_tx = 0;
	transmittedAtPreviousCycle = false;
    } else {
	NoximPacket packet;

	if (canShot(packet)) {
	    packet_queue.push(packet);
	    transmittedAtPreviousCycle = true;
	} else
	    transmittedAtPreviousCycle = false;


	if (ack_tx.read() == current_level_tx) {
	    if (!packet_queue.empty()) {
		NoximFlit flit = nextFlit();	// Generate a new flit
		if (NoximGlobalParams::verbose_mode > VERBOSE_OFF) {
		    cout << sc_time_stamp().to_double() /
			1000 << ": ProcessingElement[" << local_id <<
			"] SENDING " << flit << endl;
		}
		flit_tx->write(flit);	// Send the generated flit
		current_level_tx = 1 - current_level_tx;	// Negate the old value for Alternating Bit Protocol (ABP)
		req_tx.write(current_level_tx);
	    }
	}
    }
}

NoximFlit NoximProcessingElement::nextFlit()
{
    NoximFlit flit;
    NoximPacket packet = packet_queue.front();

    flit.src_id = packet.src_id;
    flit.dst_id = packet.dst_id;
    flit.timestamp = packet.timestamp;
    flit.sequence_no = packet.size - packet.flit_left;
    flit.hop_no = 0;
    //  flit.payload     = DEFAULT_PAYLOAD;
    flit.use_low_voltage_path = packet.use_low_voltage_path;

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

bool NoximProcessingElement::canShot(NoximPacket & packet)
{
    bool shot;
    double threshold;

    if (NoximGlobalParams::traffic_distribution != TRAFFIC_TABLE_BASED) {
	if (!transmittedAtPreviousCycle)
	    threshold = NoximGlobalParams::packet_injection_rate;
	else
	    threshold = NoximGlobalParams::probability_of_retransmission;

	shot = (((double) rand()) / RAND_MAX < threshold);
	if (shot) {
	    switch (NoximGlobalParams::traffic_distribution) {
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
	    setUseLowVoltagePath(packet);
	}
    } else {			// Table based communication traffic
	if (never_transmit)
	    return false;

	double now = sc_time_stamp().to_double() / 1000;
	bool use_pir = (transmittedAtPreviousCycle == false);
	vector < pair < int, double > > dst_prob;
	vector <bool> use_low_voltage_path;
	double threshold =
	    traffic_table->getCumulativePirPor(local_id, (int) now,
					       use_pir, dst_prob, use_low_voltage_path);

	double prob = (double) rand() / RAND_MAX;
	shot = (prob < threshold);
	if (shot) {
	    for (unsigned int i = 0; i < dst_prob.size(); i++) {
		if (prob < dst_prob[i].second) {
		    packet.make(local_id, dst_prob[i].first, now,
				getRandomSize());
		    packet.use_low_voltage_path = use_low_voltage_path[i];
		    break;
		}
	    }
	}
    }

    return shot;
}

void NoximProcessingElement::setUseLowVoltagePath(NoximPacket& packet)
{
  if (NoximGlobalParams::qos < 1.0)
    {
      double rnd = (double)rand() / (double)RAND_MAX;

      packet.use_low_voltage_path = (rnd >= NoximGlobalParams::qos);
      /*
      if (rnd >= NoximGlobalParams::qos)
	packet.use_low_voltage_path = true;
      else
	packet.use_low_voltage_path = false;
      */
    }
  else
    packet.use_low_voltage_path = false;
}

NoximPacket NoximProcessingElement::trafficRandom()
{
    NoximPacket p;
    p.src_id = local_id;
    double rnd = rand() / (double) RAND_MAX;
    double range_start = 0.0;

    //cout << "\n " << sc_time_stamp().to_double()/1000 << " PE " << local_id << " rnd = " << rnd << endl;

    int max_id =
	(NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y) -
	1;

    // Random destination distribution
    do {
	p.dst_id = randInt(0, max_id);

	// check for hotspot destination
	for (uint i = 0; i < NoximGlobalParams::hotspots.size(); i++) {
	    //cout << sc_time_stamp().to_double()/1000 << " PE " << local_id << " Checking node " << NoximGlobalParams::hotspots[i].first << " with P = " << NoximGlobalParams::hotspots[i].second << endl;

	    if (rnd >= range_start
		&& rnd <
		range_start + NoximGlobalParams::hotspots[i].second) {
		if (local_id != NoximGlobalParams::hotspots[i].first) {
		    //cout << sc_time_stamp().to_double()/1000 << " PE " << local_id <<" That is ! " << endl;
		    p.dst_id = NoximGlobalParams::hotspots[i].first;
		}
		break;
	    } else
		range_start += NoximGlobalParams::hotspots[i].second;	// try next
	}
    } while (p.dst_id == p.src_id);

    p.timestamp = sc_time_stamp().to_double() / 1000;
    p.size = p.flit_left = getRandomSize();

    return p;
}

NoximPacket NoximProcessingElement::trafficTranspose1()
{
    NoximPacket p;
    p.src_id = local_id;
    NoximCoord src, dst;

    // Transpose 1 destination distribution
    src.x = id2Coord(p.src_id).x;
    src.y = id2Coord(p.src_id).y;
    dst.x = NoximGlobalParams::mesh_dim_x - 1 - src.y;
    dst.y = NoximGlobalParams::mesh_dim_y - 1 - src.x;
    fixRanges(src, dst);
    p.dst_id = coord2Id(dst);

    p.timestamp = sc_time_stamp().to_double() / 1000;
    p.size = p.flit_left = getRandomSize();

    return p;
}

NoximPacket NoximProcessingElement::trafficTranspose2()
{
    NoximPacket p;
    p.src_id = local_id;
    NoximCoord src, dst;

    // Transpose 2 destination distribution
    src.x = id2Coord(p.src_id).x;
    src.y = id2Coord(p.src_id).y;
    dst.x = src.y;
    dst.y = src.x;
    fixRanges(src, dst);
    p.dst_id = coord2Id(dst);

    p.timestamp = sc_time_stamp().to_double() / 1000;
    p.size = p.flit_left = getRandomSize();

    return p;
}

void NoximProcessingElement::setBit(int &x, int w, int v)
{
    int mask = 1 << w;

    if (v == 1)
	x = x | mask;
    else if (v == 0)
	x = x & ~mask;
    else
	assert(false);
}

int NoximProcessingElement::getBit(int x, int w)
{
    return (x >> w) & 1;
}

inline double NoximProcessingElement::log2ceil(double x)
{
    return ceil(log(x) / log(2.0));
}

NoximPacket NoximProcessingElement::trafficBitReversal()
{

    int nbits =
	(int)
	log2ceil((double)
		 (NoximGlobalParams::mesh_dim_x *
		  NoximGlobalParams::mesh_dim_y));
    int dnode = 0;
    for (int i = 0; i < nbits; i++)
	setBit(dnode, i, getBit(local_id, nbits - i - 1));

    NoximPacket p;
    p.src_id = local_id;
    p.dst_id = dnode;

    p.timestamp = sc_time_stamp().to_double() / 1000;
    p.size = p.flit_left = getRandomSize();

    return p;
}

NoximPacket NoximProcessingElement::trafficShuffle()
{

    int nbits =
	(int)
	log2ceil((double)
		 (NoximGlobalParams::mesh_dim_x *
		  NoximGlobalParams::mesh_dim_y));
    int dnode = 0;
    for (int i = 0; i < nbits - 1; i++)
	setBit(dnode, i + 1, getBit(local_id, i));
    setBit(dnode, 0, getBit(local_id, nbits - 1));

    NoximPacket p;
    p.src_id = local_id;
    p.dst_id = dnode;

    p.timestamp = sc_time_stamp().to_double() / 1000;
    p.size = p.flit_left = getRandomSize();

    return p;
}

NoximPacket NoximProcessingElement::trafficButterfly()
{

    int nbits =
	(int)
	log2ceil((double)
		 (NoximGlobalParams::mesh_dim_x *
		  NoximGlobalParams::mesh_dim_y));
    int dnode = 0;
    for (int i = 1; i < nbits - 1; i++)
	setBit(dnode, i, getBit(local_id, i));
    setBit(dnode, 0, getBit(local_id, nbits - 1));
    setBit(dnode, nbits - 1, getBit(local_id, 0));

    NoximPacket p;
    p.src_id = local_id;
    p.dst_id = dnode;

    p.timestamp = sc_time_stamp().to_double() / 1000;
    p.size = p.flit_left = getRandomSize();

    return p;
}

void NoximProcessingElement::fixRanges(const NoximCoord src,
				       NoximCoord & dst)
{
    // Fix ranges
    if (dst.x < 0)
	dst.x = 0;
    if (dst.y < 0)
	dst.y = 0;
    if (dst.x >= NoximGlobalParams::mesh_dim_x)
	dst.x = NoximGlobalParams::mesh_dim_x - 1;
    if (dst.y >= NoximGlobalParams::mesh_dim_y)
	dst.y = NoximGlobalParams::mesh_dim_y - 1;
}

int NoximProcessingElement::getRandomSize()
{
    return randInt(NoximGlobalParams::min_packet_size,
		   NoximGlobalParams::max_packet_size);
}
