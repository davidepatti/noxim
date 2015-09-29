#ifndef __UTILS_H__
#define __UTILS_H__

#include <systemc.h>
#include <tlm>

#include "DataStructs.h"
#include <iomanip>

#ifdef DEBUG

#define LOG (std::cout << std::setw(7) << left << sc_time_stamp().to_double() / GlobalParams::clock_period_ps << " " << name() << "::" << __func__<< "() --> ")

#else
template <class cT, class traits = std::char_traits<cT> >
class basic_nullbuf: public std::basic_streambuf<cT, traits> {
    typename traits::int_type overflow(typename traits::int_type c)
    {
        return traits::not_eof(c); // indicate success
    }
};

template <class cT, class traits = std::char_traits<cT> >
class basic_onullstream: public std::basic_ostream<cT, traits> {
public:
    basic_onullstream():
    std::basic_ios<cT, traits>(&m_sbuf),
    std::basic_ostream<cT, traits>(&m_sbuf)
    {
        // note: the original code is missing the required this->
        this->init(&m_sbuf);
    }

private:
    basic_nullbuf<cT, traits> m_sbuf;
};

typedef basic_onullstream<char> onullstream;
typedef basic_onullstream<wchar_t> wonullstream;

static onullstream LOG;

#endif

// Output overloading

inline ostream & operator <<(ostream & os, const Flit & flit)
{

    if (GlobalParams::verbose_mode == VERBOSE_HIGH) {

	os << "### FLIT ###" << endl;
	os << "Source Tile[" << flit.src_id << "]" << endl;
	os << "Destination Tile[" << flit.dst_id << "]" << endl;
	switch (flit.flit_type) {
	case FLIT_TYPE_HEAD:
	    os << "Flit Type is HEAD" << endl;
	    break;
	case FLIT_TYPE_BODY:
	    os << "Flit Type is BODY" << endl;
	    break;
	case FLIT_TYPE_TAIL:
	    os << "Flit Type is TAIL" << endl;
	    break;
	}
	os << "Sequence no. " << flit.sequence_no << endl;
	os << "Payload printing not implemented (yet)." << endl;
	os << "Unix timestamp at packet generation " << flit.
	    timestamp << endl;
	os << "Total number of hops from source to destination is " <<
	    flit.hop_no << endl;
    } else {
	os << "(";
	switch (flit.flit_type) {
	case FLIT_TYPE_HEAD:
	    os << "H";
	    break;
	case FLIT_TYPE_BODY:
	    os << "B";
	    break;
	case FLIT_TYPE_TAIL:
	    os << "T";
	    break;
	}

	os <<  flit.sequence_no << ", " << flit.src_id << "->" << flit.dst_id << ")";
    }

    return os;
}

inline ostream & operator <<(ostream & os,
			     const ChannelStatus & status)
{
    char msg;
    if (status.available)
	msg = 'A';
    else
	msg = 'N';
    os << msg << "(" << status.free_slots << ")";
    return os;
}

inline ostream & operator <<(ostream & os, const NoP_data & NoP_data)
{
    os << "      NoP data from [" << NoP_data.sender_id << "] [ ";

    for (int j = 0; j < DIRECTIONS; j++)
	os << NoP_data.channel_status_neighbor[j] << " ";

    os << "]" << endl;
    return os;
}

inline ostream & operator <<(ostream & os, const Coord & coord)
{
    os << "(" << coord.x << "," << coord.y << ")";

    return os;
}

// Trace overloading

inline void sc_trace(sc_trace_file * &tf, const Flit & flit, string & name)
{
    sc_trace(tf, flit.src_id, name + ".src_id");
    sc_trace(tf, flit.dst_id, name + ".dst_id");
    sc_trace(tf, flit.sequence_no, name + ".sequence_no");
    sc_trace(tf, flit.timestamp, name + ".timestamp");
    sc_trace(tf, flit.hop_no, name + ".hop_no");
}

inline void sc_trace(sc_trace_file * &tf, const NoP_data & NoP_data, string & name)
{
    sc_trace(tf, NoP_data.sender_id, name + ".sender_id");
}

inline void sc_trace(sc_trace_file * &tf, const ChannelStatus & bs, string & name)
{
    sc_trace(tf, bs.free_slots, name + ".free_slots");
    sc_trace(tf, bs.available, name + ".available");
}

// Misc common functions

inline Coord id2Coord(int id)
{
    Coord coord;

    coord.x = id % GlobalParams::mesh_dim_x;
    coord.y = id / GlobalParams::mesh_dim_x;

    assert(coord.x < GlobalParams::mesh_dim_x);
    assert(coord.y < GlobalParams::mesh_dim_y);

    return coord;
}

inline int coord2Id(const Coord & coord)
{
    int id = (coord.y * GlobalParams::mesh_dim_x) + coord.x;

    assert(id < GlobalParams::mesh_dim_x * GlobalParams::mesh_dim_y);

    return id;
}

inline bool sameRadioHub(int id1, int id2)
{
    map<int, int>::iterator it1 = GlobalParams::hub_for_tile.find(id1); 
    map<int, int>::iterator it2 = GlobalParams::hub_for_tile.find(id2); 

    assert( (it1 != GlobalParams::hub_for_tile.end()) && "Specified Tile is not connected to any Hub");
    assert( (it2 != GlobalParams::hub_for_tile.end()) && "Specified Tile is not connected to any Hub");

    return (it1->second == it2->second);
}

inline bool hasRadioHub(int id)
{
    map<int, int>::iterator it = GlobalParams::hub_for_tile.find(id);

    return (it != GlobalParams::hub_for_tile.end());
}


inline int tile2Hub(int id)
{
    map<int, int>::iterator it = GlobalParams::hub_for_tile.find(id); 
    assert( (it != GlobalParams::hub_for_tile.end()) && "Specified Tile is not connected to any Hub");
    return it->second;
}

inline int selectChannel(int src_hub, int dst_hub)
{  
    
    vector<int> & first = GlobalParams::hub_configuration[src_hub].txChannels;
    vector<int> & second = GlobalParams::hub_configuration[dst_hub].rxChannels;

    vector<int> intersection;

	for (unsigned int i=0;i<first.size();i++)
    {
	for (unsigned int j=0;j<second.size();j++)
	{
	    if (first[i] ==second[j])
		intersection.push_back(first[i]);
	}
    }

    if (intersection.size()==0) return NOT_VALID;

    return intersection[rand()%intersection.size()];

}

inline void printMap(string label, const map<string,double> & m,std::ostream & out)
{
    out << label << " = [" << endl;
    for (map<string,double>::const_iterator i = m.begin();i!=m.end();i++)
	out << "\t" << std::scientific << i->second << "\t % " << i->first << endl;

    out << "];" << endl;
}

#endif
