#include "TLocalRoutingTable.h"
#include "NoximDefs.h"

//---------------------------------------------------------------------------

TLocalRoutingTable::TLocalRoutingTable()
{
}

//---------------------------------------------------------------------------

void TLocalRoutingTable::configure(TGlobalRoutingTable& rtable, 
				   const int _node_id)
{
  rt_node = rtable.getNodeRoutingTable(_node_id);
  node_id = _node_id;
}

//---------------------------------------------------------------------------

TAdmissibleOutputs TLocalRoutingTable::getAdmissibleOutputs(const TLinkId& in_link,
							    const int      destination_id)
{
  return rt_node[in_link][destination_id];
}

//---------------------------------------------------------------------------

TAdmissibleOutputs TLocalRoutingTable::getAdmissibleOutputs(const int in_direction,
							    const int destination_id)
{
  TLinkId lid = direction2ILinkId(node_id, in_direction);

  return getAdmissibleOutputs(lid, destination_id);
}

//---------------------------------------------------------------------------
