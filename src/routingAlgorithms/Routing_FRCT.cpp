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
    //cout <<"coucou current_id "<<routeData.current_id<<" source : "<<routeData.src_id<<" dest : "<<routeData.dst_id<<"intrNode : "<<routeData.intr_id<<endl;
    Coord current = id2Coord(routeData.current_id);
    Coord destination = id2Coord(routeData.dst_id);
    
     
// check if the SRC = DEST 

    if (routeData.current_id == routeData.dst_id)
    {
        vector<int> dir;
        dir.push_back(DIRECTION_LOCAL);
        return dir;
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
        vector<int> dir;
        dir.push_back(DIRECTION_HUB);
        return dir;
    }


    return wiredRouting (current, closest_n_attached_rh);
    // return wiredRouting (getClosestNodeAttachedToRadioHub(destination), destination);
    

}
