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
    
    // Negative directions:
    // WEST (current x > dest x)
    // SOUTH (current y > dest y)
    //
    // Algorithm: Never switch from a positive direction to a negative
    // So, any south or west direction should always have priority

    if (destination.x < current.x || destination.y > current.y) // check negative directions first
    {
	// note: one or both negative directions could be added
	if (destination.x < current.x) directions.push_back(DIRECTION_WEST);
	if (destination.y > current.y) directions.push_back(DIRECTION_SOUTH);
    } 
    else  // no negative direction to process, check if positive ones are needed
	if (destination.x > current.x || destination.y < current.y) 
	{
	    if (destination.x > current.x) directions.push_back(DIRECTION_EAST);
	    if (destination.y < current.y) directions.push_back(DIRECTION_NORTH);
	} 
	else // both x and y were already reached
	    directions.push_back(DIRECTION_LOCAL);

    return directions;
}
