/*****************************************************************************

  NoximDefs.h -- Common constants and structs definitions

 *****************************************************************************/

#ifndef __NOXIM_DEFS_H__
#define __NOXIM_DEFS_H__

//---------------------------------------------------------------------------

#include <cassert>
#include <systemc.h>

using namespace std;

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
#define CHANNEL_NOT_RESERVED  3
// To mark invalid or non exhistent values
#define NOT_VALID            -1

// Routing algorithms
#define ROUTING_XY             0
#define ROUTING_WEST_FIRST     1
#define ROUTING_NORTH_LAST     2
#define ROUTING_NEGATIVE_FIRST 3
#define ROUTING_ODD_EVEN       4
#define ROUTING_DYAD           5
#define ROUTING_LOOK_AHEAD     6
#define ROUTING_FULLY_ADAPTIVE 8
#define ROUTING_TABLE_BASED    9

// Selection strategies
#define SEL_RANDOM            0
#define SEL_BUFFER_LEVEL      1
#define SEL_NOP               2

// Traffic distribution
#define TRAFFIC_UNIFORM       0
#define TRAFFIC_TRANSPOSE1    1
#define TRAFFIC_TRANSPOSE2    2
#define TRAFFIC_HOTSPOT       3
#define TRAFFIC_TABLE_BASED   4

//---------------------------------------------------------------------------

// Default configuration can be overridden with command-line arguments
#define DEFAULT_VERBOSE_MODE                     false
#define DEFAULT_TRACE_MODE                       false
#define DEFAULT_TRACE_FILENAME                      ""
#define DEFAULT_MESH_DIM_X                           4
#define DEFAULT_MESH_DIM_Y                           4
#define DEFAULT_BUFFER_DEPTH                         4
#define DEFAULT_MAX_PACKET_SIZE                     10
#define DEFAULT_ROUTING_ALGORITHM           ROUTING_XY
#define DEFAULT_ROUTING_TABLE_FILENAME              ""
#define DEFAULT_SELECTION_STRATEGY          SEL_RANDOM
#define DEFAULT_PACKET_INJECTION_RATE             0.01
#define DEFAULT_TRAFFIC_DISTRIBUTION   TRAFFIC_UNIFORM
#define DEFAULT_TRAFFIC_TABLE_FILENAME              ""
#define DEFAULT_RESET_TIME                        1000
#define DEFAULT_SIMULATION_TIME                  10000
#define DEFAULT_STATS_WARM_UP_TIME  DEFAULT_RESET_TIME

// TGlobalParams -- used to forward configuration to every sub-block
struct TGlobalParams
{
  static int verbose_mode;
  static int trace_mode;
  static char trace_filename[128];
  static int mesh_dim_x;
  static int mesh_dim_y;
  static int buffer_depth;
  static int max_packet_size;
  static int routing_algorithm;
  static char routing_table_filename[128];
  static int selection_strategy;
  static float packet_injection_rate;
  static int traffic_distribution;
  static char traffic_table_filename[128];
  static int simulation_time;
  static int stats_warm_up_time;
};

// TODO by Fafa - this MUST be removed!!!
#define MAX_STATIC_DIM 20

// TCoord -- XY coordinates type of the Tile inside the Mesh
class TCoord
{
 public:
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

// TPacket -- Packet definition
struct TPacket
{
  int                src_id;
  int                dst_id;
  double             timestamp;    // SC timestamp at packet generation
  int                size;
  int                flit_left;    // Number of remaining flits inside the packet
};

struct TBufferStatus
{
    int level;  // occupied buffer slots
    int state; // (is empty, has head, has tail, not_reserved)
    inline bool operator == (const TBufferStatus& bs) const
    {
	return (level == bs.level && state == bs.state);
    };
};


// TNoP_data -- NoP Data definition
struct TNoP_data
{
    int sender_id;
    TBufferStatus buffer_status_neighbor[DIRECTIONS]; 

    inline bool operator == (const TNoP_data& nop_data) const
    {
	return ( sender_id==nop_data.sender_id  &&
	         nop_data.buffer_status_neighbor[0]==buffer_status_neighbor[0] &&
	         nop_data.buffer_status_neighbor[1]==buffer_status_neighbor[1] &&
	         nop_data.buffer_status_neighbor[2]==buffer_status_neighbor[2] &&
	         nop_data.buffer_status_neighbor[3]==buffer_status_neighbor[3]);
    };
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
  // TODO: decide what to do with the commented code. Move to a
  // separate print_flit() function for debugging purposes ?

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
  os << "[flit seq=" << flit.sequence_no << ", " << flit.src_id << "-->" << flit.dst_id << "]"; 

  return os;
}

inline ostream& operator << (ostream& os, const TBufferStatus& bs)
{
  string msg;
  switch (bs.state)
  {
      case CHANNEL_EMPTY: 
	  msg = "E";
	  break;
      case CHANNEL_HAS_HEAD:
	  msg = "H";
	  break;
      case CHANNEL_HAS_TAIL:
	  msg = "T";
	  break;
      case CHANNEL_NOT_RESERVED:
	  msg = "N";
	  break;
      case NOT_VALID:
	  msg = "x";
	  break;
      default:
	  cout << "Error: channel state = " <<  bs.state << endl;
	  assert(0);
  }
  os << msg << "(" << bs.level << ")"; 
  return os;
}

inline ostream& operator << (ostream& os, const TNoP_data& NoP_data)
{
  os << "   NoP data from [" << NoP_data.sender_id << "] [ ";

  for (int j=0; j<DIRECTIONS; j++)
      os << NoP_data.buffer_status_neighbor[j] << " ";

  cout << "]" << endl;
  return os;
}


inline void sc_trace(sc_trace_file*& tf, const TFlit& flit, string& name)
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

inline void sc_trace(sc_trace_file*& tf, const TNoP_data& NoP_data, string& name)
{
  sc_trace(tf, NoP_data.sender_id, name+".sender_id");
}

inline void sc_trace(sc_trace_file*& tf, const TBufferStatus& bs, string& name)
{
  sc_trace(tf, bs.level, name+".level");
  sc_trace(tf, bs.state, name+".state");
}

inline TCoord id2Coord(int id) 
{
  TCoord coord;

  coord.x = id % TGlobalParams::mesh_dim_x;
  coord.y = id / TGlobalParams::mesh_dim_x;

  assert(coord.x < TGlobalParams::mesh_dim_x);
  assert(coord.y < TGlobalParams::mesh_dim_y);

  return coord;
}

inline int coord2Id(const TCoord& coord) 
{
  int id = (coord.y * TGlobalParams::mesh_dim_x) + coord.x;

  assert(id < TGlobalParams::mesh_dim_x * TGlobalParams::mesh_dim_y);

  return id;
}

#endif  // NOXIMDEFS_H



