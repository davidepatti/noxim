#include "Routing_WEST_FIRST.h"

RoutingAlgorithmsRegister Routing_WEST_FIRST::routingAlgorithmsRegister("WEST_FIRST", getInstance());

Routing_WEST_FIRST * Routing_WEST_FIRST::routing_WEST_FIRST = 0;
RoutingAlgorithm * Routing_WEST_FIRST::xy = 0;

Routing_WEST_FIRST * Routing_WEST_FIRST::getInstance() {
	if ( routing_WEST_FIRST == 0 )
        routing_WEST_FIRST = new Routing_WEST_FIRST();

    return routing_WEST_FIRST;
}

vector<int> Routing_WEST_FIRST::route(Router * router, const RouteData & routeData)
{
    Coord current = id2Coord(routeData.current_id);
    Coord destination = id2Coord(routeData.dst_id);
    vector <int> directions;

    if (destination.x <= current.x || destination.y == current.y)
    {
        if(!xy)
        {
            xy = RoutingAlgorithms::get("XY");
            
            if (!xy)
                assert(false);
        }

        return xy->route(router, routeData);
    }
    if (destination.y < current.y)
    {
        directions.push_back(DIRECTION_NORTH);
        directions.push_back(DIRECTION_EAST);
    }
    else 
    {
        directions.push_back(DIRECTION_SOUTH);
        directions.push_back(DIRECTION_EAST);
    }

    return directions;
}
