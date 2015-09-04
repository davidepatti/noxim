#include "Routing_DYAD.h"

RoutingAlgorithmsRegister Routing_DYAD::routingAlgorithmsRegister("DYAD", getInstance());

Routing_DYAD * Routing_DYAD::routing_DYAD = 0;
RoutingAlgorithm * Routing_DYAD::odd_even = 0;

Routing_DYAD * Routing_DYAD::getInstance() {
	if ( routing_DYAD == 0 )
		routing_DYAD = new Routing_DYAD();
    
    return routing_DYAD;
}

vector<int> Routing_DYAD::route(Router * router, const RouteData & routeData)
{
    vector <int> directions;

    if (!odd_even)
    {
        odd_even = RoutingAlgorithms::get("ODD_EVEN");
        
        if (!odd_even)
            assert(false);
    }

    directions = odd_even->route(router, routeData);

    if (!router->inCongestion()) directions.resize(1);

    return directions;
}
