#ifndef __TLOCALROUTINGTABLE_H__
#define __TLOCALROUTINGTABLE_H__

//---------------------------------------------------------------------------

#include "TGlobalRoutingTable.h"

//---------------------------------------------------------------------------

class TLocalRoutingTable
{

public:

  TLocalRoutingTable();

  // Extracts the routing table of node _node_id from the global
  // routing table rtable
  void configure(TGlobalRoutingTable& rtable, const int _node_id);

  // Returns the set of admissible output channels for destination
  // destination_id and input channel in_link
  TAdmissibleOutputs getAdmissibleOutputs(const TLinkId& in_link,
					  const int      destination_id);

  // Returns the set of admissible output channels for a destination
  // destination_id and a given input direction
  TAdmissibleOutputs getAdmissibleOutputs(const int in_direction,
					  const int destination_id);


private:

  TRoutingTableNode rt_node;
  int               node_id;
};

//---------------------------------------------------------------------------

#endif
