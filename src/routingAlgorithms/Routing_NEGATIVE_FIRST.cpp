#include "Routing_NEGATIVE_FIRST.h"

RoutingAlgorithmsRegister Routing_NEGATIVE_FIRST::routingAlgorithmsRegister("NEGATIVE_FIRST", getInstance());

Routing_NEGATIVE_FIRST * Routing_NEGATIVE_FIRST::routing_NEGATIVE_FIRST = 0;
RoutingAlgorithm * Routing_NEGATIVE_FIRST::xy = 0;

Routing_NEGATIVE_FIRST * Routing_NEGATIVE_FIRST::getInstance() {
	if ( routing_NEGATIVE_FIRST == 0 )
		routing_NEGATIVE_FIRST = new Routing_NEGATIVE_FIRST();
    
	return routing_NEGATIVE_FIRST;
}

vector<int> Routing_NEGATIVE_FIRST::route(Router * router, const RouteData & routeData)
{
    Coord current = id2Coord(routeData.current_id);
    Coord destination = id2Coord(routeData.dst_id);
    vector <int> directions;

    if ((destination.x <= current.x && destination.y <= current.y) ||
            (destination.x >= current.x && destination.y >= current.y))
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
    else 
    {
        directions.push_back(DIRECTION_SOUTH);
        directions.push_back(DIRECTION_WEST);
    }

    return directions;
}
