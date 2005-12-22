#ifndef __TGLOBALROUTINGTABLE_H__
#define __TGLOBALROUTINGTABLE_H__

//---------------------------------------------------------------------------

#include <vector>
#include <map>
#include <set>

using namespace std;

//---------------------------------------------------------------------------

typedef pair<int,int> TLinkId; // source, destination node

// Routing table
typedef set<TLinkId> TAdmissibleOutputs;

// Map a destination to a set of admissible outputs
typedef map<int,TAdmissibleOutputs> TRoutingTableLink; 

// Map an input link to its routing table
typedef map<TLinkId,TRoutingTableLink> TRoutingTableNode;

// Map a node of the network to its routing table
typedef map<int,TRoutingTableNode> TRoutingTableNoC;

//---------------------------------------------------------------------------

// Converts an input direction to a link 
TLinkId direction2ILinkId(const int node_id, const int dir);

// Converts an input direction to a link
int oLinkId2Direction(const TLinkId& out_link);

// Converts a set of output links to a set of directions
vector<int> admissibleOutputsSet2Vector(const TAdmissibleOutputs& ao);

//---------------------------------------------------------------------------

class TGlobalRoutingTable
{

public:

  TGlobalRoutingTable(); 

  // Load routing table from file. Returns true if ok, false otherwise
  bool load(const char* fname);

  TRoutingTableNode getNodeRoutingTable(const int node_id);

  bool isValid() { return valid; }


private:

  TRoutingTableNoC rt_noc;  
  bool             valid;

  // Search label rt_label through the file
  //  bool seek(const char* fname, const char* rt_label, ifstream& fin);

};

//---------------------------------------------------------------------------

#endif
