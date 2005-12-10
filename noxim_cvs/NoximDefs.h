/*****************************************************************************

  NoximDefs.h -- Common constants and structs definitions

 *****************************************************************************/

#ifndef __NOXIM_DEFS_H__
#define __NOXIM_DEFS_H__

//---------------------------------------------------------------------------

#include <cassert>
#include <systemc.h>

// Define the directions as numbers
#define DIRECTIONS      4
#define DIRECTION_NORTH 0
#define DIRECTION_EAST  1
#define DIRECTION_SOUTH 2
#define DIRECTION_WEST  3
#define DIRECTION_LOCAL 4

// Channel states for queues
#define CHANNEL_EMPTY         0
#define CHANNEL_HAS_HEAD      1
#define CHANNEL_HAS_TAIL      2
#define CHANNEL_NOT_RESERVED -1

// Routing algorithms
#define XY                    0
#define WEST_FIRST            1
#define NORTH_LAST            2
#define NEGATIVE_FIRST        3
#define ODD_EVEN              4
#define DYAD                  5
#define LOOK_AHEAD            6
#define NOPCAR                7
#define FULLY_ADAPTIVE        8

// Selection strategies
#define SEL_RANDOM            0
#define SEL_BUFFER_LEVEL      1
#define SEL_NOPCAR            2

//---------------------------------------------------------------------------

// Default configuration can be overridden with command-line arguments
#define DEFAULT_VERBOSE_MODE               false
#define DEFAULT_MESH_DIM_X                     4
#define DEFAULT_MESH_DIM_Y                     4
#define DEFAULT_BUFFER_DEPTH                   4
#define DEFAULT_MAX_PACKET_SIZE               10
#define DEFAULT_ROUTING_ALGORITHM             XY
#define DEFAULT_SELECTION_STRATEGY    SEL_RANDOM
#define DEFAULT_PACKET_INJECTION_RATE       0.01
#define DEFAULT_SIMULATION_TIME            10000

// TGlobalParams -- used to forward configuration to every sub-block
class TGlobalParams
{
public:
  static int verbose_mode;
  static int mesh_dim_x;
  static int mesh_dim_y;
  static int buffer_depth;
  static int max_packet_size;
  static int routing_algorithm;
  static int selection_strategy;
  static float packet_injection_rate;
  static int simulation_time;
};


// TODO by Fafa - this MUST be removed!!!
#define MAX_STATIC_DIM 20

// TCoord -- XY coordinates type of the Tile inside the Mesh
struct TCoord
{
  int                x;            // X coordinate
  int                y;            // Y coordinate

  inline bool operator == (const TCoord& coord) const
  {
    return (coord.x==x && coord.y==y);
  }
};

// TFlitType -- Flit type enumeration
enum TFlitType
{
  FLIT_TYPE_HEAD, FLIT_TYPE_BODY, FLIT_TYPE_TAIL
};

// TPayload -- Payload definition
struct TPayload
{
  sc_uint<32>        data;         // Bus for the data to be exchanged

  inline bool operator == (const TPayload& payload) const
  {
    return (payload.data==data);
  }
};

// TPacket -- TPacket definition
struct TPacket
{
  /*
  TCoord             src_coord;    // The XY coordinates of the source tile
  TCoord             dst_coord;    // The XY coordinates of the destination tile
  */
  int                src_id;
  int                dst_id;
  double             timestamp;    // SC timestamp at packet generation
  int                size;
  int                flit_left;    // Number of remaining flits inside the packet
};

// TFlit -- Flit definition
struct TFlit
{
  /*
  TCoord             src_coord;    // The XY coordinates of the source tile
  TCoord             dst_coord;    // The XY coordinates of the destination tile
  */
  int                src_id;
  int                dst_id;
  TFlitType          flit_type;    // The flit type (FLIT_TYPE_HEAD, FLIT_TYPE_BODY, FLIT_TYPE_TAIL)
  int                sequence_no;  // The sequence number of the flit inside the packet
  TPayload           payload;      // Optional payload
  double             timestamp;    // Unix timestamp at packet generation
  int                hop_no;       // Current number of hops from source to destination

  inline bool operator == (const TFlit& flit) const
  {
    return (flit.src_id==src_id && flit.dst_id==dst_id && flit.flit_type==flit_type && flit.sequence_no==sequence_no && flit.payload==payload && flit.timestamp==timestamp && flit.hop_no==hop_no);
  }
};

inline ostream& operator << (ostream& os, const TFlit& flit)
{
  /*
  os << "### FLIT ###" << endl;
  os << "Source Tile[" << flit.src_coord.x << "][" << flit.src_coord.y << "]" << endl;
  os << "Destination Tile[" << flit.dst_coord.x << "][" << flit.dst_coord.y << "]" << endl;
  switch(flit.flit_type)
  {
    case FLIT_TYPE_HEAD: os << "Flit Type is HEAD" << endl; break;
    case FLIT_TYPE_BODY: os << "Flit Type is BODY" << endl; break;
    case FLIT_TYPE_TAIL: os << "Flit Type is TAIL" << endl; break;
  }
  os << "Sequence no. " << flit.sequence_no << endl;
  os << "Payload printing not implemented (yet)." << endl;
  os << "Unix timestamp at packet generation " << flit.timestamp << endl;
  os << "Total number of hops from source to destination is " << flit.hop_no << endl;
  */
  /*
  os << "flit " << flit.sequence_no << " (" << flit.src_coord.x << "," << flit.src_coord.y << ") --> (" 
     << flit.dst_coord.x << "," << flit.dst_coord.y << ")"; 
  */
  os << "[flit seq=" << flit.sequence_no << ", " << flit.src_id << "-->" 
     << flit.dst_id << "]"; 

  return os;
}

inline void sc_trace(sc_trace_file*& tf, const TFlit& flit, std::string& name)
{
  /*
  sc_trace(tf, flit.src_coord.x, name+".src_coord.x");
  sc_trace(tf, flit.src_coord.y, name+".src_coord.y");
  sc_trace(tf, flit.dst_coord.x, name+".dst_coord.x");
  sc_trace(tf, flit.dst_coord.y, name+".dst_coord.y");
  */
  sc_trace(tf, flit.src_id, name+".src_id");
  sc_trace(tf, flit.dst_id, name+".dst_id");
  sc_trace(tf, flit.sequence_no, name+".sequence_no");
  sc_trace(tf, flit.timestamp, name+".timestamp");
  sc_trace(tf, flit.hop_no, name+".hop_no");
}

inline TCoord id2Coord(int id) 
{
  TCoord coord;

  coord.x = id % TGlobalParams::mesh_dim_x;
  coord.y = id / TGlobalParams::mesh_dim_x;

  assert(coord.y < TGlobalParams::mesh_dim_y);

  return coord;
}

#endif  // NOXIMDEFS_H



