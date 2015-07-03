#include "Routing_FULLY_ADAPTIVE.h"

RoutingAlgorithmsRegister Routing_FULLY_ADAPTIVE::routingAlgorithmsRegister("FULLY_ADAPTIVE", getInstance());

Routing_FULLY_ADAPTIVE * Routing_FULLY_ADAPTIVE::routing_FULLY_ADAPTIVE = 0;
RoutingAlgorithm * Routing_FULLY_ADAPTIVE::xy = 0;

Routing_FULLY_ADAPTIVE * Routing_FULLY_ADAPTIVE::getInstance() {
	if ( routing_FULLY_ADAPTIVE == 0 )
        routing_FULLY_ADAPTIVE = new Routing_FULLY_ADAPTIVE();
    
    return routing_FULLY_ADAPTIVE;
}

vector<int> Routing_FULLY_ADAPTIVE::route(Router * router, const RouteData & routeData)
{
    Coord current = id2Coord(routeData.current_id);
    Coord destination = id2Coord(routeData.dst_id);
    vector <int> directions;

    if (destination.x == current.x || destination.y == current.y)
    {
        if(!xy)
        {
            xy = RoutingAlgorithms::get("XY");
            
            if (!xy)
                assert(false);
        }
        
        return xy->route(router, routeData);
    }
    if (destination.x > current.x && destination.y < current.y) 
    {
        directions.push_back(DIRECTION_NORTH);
        directions.push_back(DIRECTION_EAST);
    } 
    else if (destination.x > current.x && destination.y > current.y) 
    {
        directions.push_back(DIRECTION_SOUTH);
        directions.push_back(DIRECTION_EAST);
    } 
    else if (destination.x < current.x && destination.y > current.y) 
    {
        directions.push_back(DIRECTION_SOUTH);
        directions.push_back(DIRECTION_WEST);
    } 
    else 
    {
        directions.push_back(DIRECTION_NORTH);
        directions.push_back(DIRECTION_WEST);
    }

    return directions;
}
