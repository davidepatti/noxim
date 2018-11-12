#include "Routing_FRCT.h"

RoutingAlgorithmsRegister Routing_FRCT::routingAlgorithmsRegister("FRCT", getInstance());

Routing_FRCT * Routing_FRCT::routing_FRCT = 0;

Routing_FRCT * Routing_FRCT::getInstance() {
	if ( routing_FRCT == 0 )
		routing_FRCT = new Routing_FRCT();
    
	return routing_FRCT;
}

vector<int> Routing_FRCT::route(Router * router, const RouteData & routeData)
{
    Coord current = id2Coord(routeData.current_id);
    Coord destination = id2Coord(routeData.dst_id);
    
     
// check if the SRC = DEST 

    if (routeData.current_id == routeData.dst_id)
    {
        
        return vector<int>(DIRECTION_LOCAL);
    }

// check SRC and DEST in the same cluster //


    if (sameCluster(routeData.src_id, routeData.dst_id))
    {  
       return wiredRouting(current, destination);
        
    }
   else 
    {

        int dist_wired = getWiredDistanceI(routeData.current_id, routeData.dst_id);
        int dist_wireless = getWirelessDistance(routeData.current_id, routeData.dst_id);
        if (dist_wired < dist_wireless)
            return wiredRouting(current, destination);
        else 
            return wirelessRouting(current, destination);

    }

}

vector<int> Routing_FRCT::wiredRouting(Coord current, Coord destination)
{
    vector <int> directions;

    if (destination.x > current.x)
            directions.push_back(DIRECTION_EAST);
    else if (destination.x < current.x)
            directions.push_back(DIRECTION_WEST);
    else if (destination.y > current.y)
            directions.push_back(DIRECTION_SOUTH);
    else
            directions.push_back(DIRECTION_NORTH);

    return directions;
}

vector<int> Routing_FRCT::wirelessRouting(Coord current, Coord destination)
{
    vector <int> directions;
    Coord closest_n_attached_rh = getClosestNodeAttachedToRadioHubC(current);

    if (current == closest_n_attached_rh)
    {
        return vector<int>(DIRECTION_WIRELESS);
    }


    return wiredRouting (current, closest_n_attached_rh);
    // return wiredRouting (getClosestNodeAttachedToRadioHub(destination), destination);
    

}

bool Routing_FRCT::sameCluster(int node1_id, int node2_id)
{
    Coord node1 = id2Coord(node1_id);
    Coord node2 = id2Coord(node2_id);
  return (node1.x/CLUSTER_WIDTH == node2.x/CLUSTER_WIDTH && node1.y/CLUSTER_HEIGHT == node2.y/CLUSTER_HEIGHT);
}



int Routing_FRCT::getWirelessDistance(int node1_id, int node2_id)
{
    Coord node1 = id2Coord (node1_id);
    Coord node2 = id2Coord (node2_id);

    Coord closest_n_attached_rh1 = getClosestNodeAttachedToRadioHubC (node1);
    Coord closest_n_attached_rh2 = getClosestNodeAttachedToRadioHubC (node2);

    return (getWiredDistanceC(node1, closest_n_attached_rh1) + getWiredDistanceC(node2, closest_n_attached_rh2) + 1);

}

