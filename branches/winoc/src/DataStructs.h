/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the top-level of Noxim
 */

#ifndef _DATASTRUCS_H__
#define _DATASTRUCS_H__

#include <systemc.h>
#include "GlobalParams.h"

// Coord -- XY coordinates type of the Tile inside the Mesh
class Coord {
  public:
    int x;			// X coordinate
    int y;			// Y coordinate

    inline bool operator ==(const Coord & coord) const {
	return (coord.x == x && coord.y == y);
}};

// FlitType -- Flit type enumeration
enum FlitType {
    FLIT_TYPE_HEAD, FLIT_TYPE_BODY, FLIT_TYPE_TAIL
};

// Payload -- Payload definition
struct Payload {
    sc_uint<32> data;	// Bus for the data to be exchanged

    inline bool operator ==(const Payload & payload) const {
	return (payload.data == data);
}};

// Packet -- Packet definition
struct Packet {
    int src_id;
    int dst_id;
    double timestamp;		// SC timestamp at packet generation
    int size;
    int flit_left;		// Number of remaining flits inside the packet
    bool use_low_voltage_path;

    // Constructors
    Packet() { }

    Packet(const int s, const int d, const double ts, const int sz) {
	make(s, d, ts, sz);
    }

    void make(const int s, const int d, const double ts, const int sz) {
	src_id = s;
	dst_id = d;
	timestamp = ts;
	size = sz;
	flit_left = sz;
	use_low_voltage_path = false;
    }
};

// RouteData -- data required to perform routing
struct RouteData {
    int current_id;
    int src_id;
    int dst_id;
    int dir_in;			// direction from which the packet comes from
};

struct ChannelStatus {
    int free_slots;		// occupied buffer slots
    bool available;		// 
    inline bool operator ==(const ChannelStatus & bs) const {
	return (free_slots == bs.free_slots && available == bs.available);
    };
};

// NoP_data -- NoP Data definition
struct NoP_data {
    int sender_id;
    ChannelStatus channel_status_neighbor[DIRECTIONS];

    inline bool operator ==(const NoP_data & nop_data) const {
	return (sender_id == nop_data.sender_id &&
		nop_data.channel_status_neighbor[0] ==
		channel_status_neighbor[0]
		&& nop_data.channel_status_neighbor[1] ==
		channel_status_neighbor[1]
		&& nop_data.channel_status_neighbor[2] ==
		channel_status_neighbor[2]
		&& nop_data.channel_status_neighbor[3] ==
		channel_status_neighbor[3]);
    };
};

// Flit -- Flit definition
struct Flit {
    int src_id;
    int dst_id;
    FlitType flit_type;	// The flit type (FLIT_TYPE_HEAD, FLIT_TYPE_BODY, FLIT_TYPE_TAIL)
    int sequence_no;		// The sequence number of the flit inside the packet
    Payload payload;	// Optional payload
    double timestamp;		// Unix timestamp at packet generation
    int hop_no;			// Current number of hops from source to destination
    bool use_low_voltage_path;

    inline bool operator ==(const Flit & flit) const {
	return (flit.src_id == src_id && flit.dst_id == dst_id
		&& flit.flit_type == flit_type
		&& flit.sequence_no == sequence_no
		&& flit.payload == payload && flit.timestamp == timestamp
		&& flit.hop_no == hop_no
		&& flit.use_low_voltage_path == use_low_voltage_path);
}};

#endif
