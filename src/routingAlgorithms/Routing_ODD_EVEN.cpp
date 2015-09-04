#include "Routing_ODD_EVEN.h"

RoutingAlgorithmsRegister Routing_ODD_EVEN::routingAlgorithmsRegister("ODD_EVEN", getInstance());

Routing_ODD_EVEN * Routing_ODD_EVEN::routing_ODD_EVEN = 0;

Routing_ODD_EVEN * Routing_ODD_EVEN::getInstance() {
	if ( routing_ODD_EVEN == 0 )
		routing_ODD_EVEN = new Routing_ODD_EVEN();
    
	return routing_ODD_EVEN;
}

vector<int> Routing_ODD_EVEN::route(Router * router, const RouteData & routeData)
{
    Coord current = id2Coord(routeData.current_id);
    Coord destination = id2Coord(routeData.dst_id);
    Coord source = id2Coord(routeData.src_id);
    vector <int> directions;

    int c0 = current.x;
    int c1 = current.y;
    int s0 = source.x;
    int d0 = destination.x;
    int d1 = destination.y;
    int e0, e1;

    e0 = d0 - c0;
    e1 = -(d1 - c1);

    if (e0 == 0) {
        if (e1 > 0)
            directions.push_back(DIRECTION_NORTH);
        else
            directions.push_back(DIRECTION_SOUTH);
    } else {
        if (e0 > 0) {
            if (e1 == 0)
                directions.push_back(DIRECTION_EAST);
            else {
                if ((c0 % 2 == 1) || (c0 == s0)) {
                    if (e1 > 0)
                        directions.push_back(DIRECTION_NORTH);
                    else
                        directions.push_back(DIRECTION_SOUTH);
                }
                if ((d0 % 2 == 1) || (e0 != 1))
                    directions.push_back(DIRECTION_EAST);
            }
        } else {
            directions.push_back(DIRECTION_WEST);
            if (c0 % 2 == 0) {
                if (e1 > 0)
                    directions.push_back(DIRECTION_NORTH);
                if (e1 < 0)
                    directions.push_back(DIRECTION_SOUTH);
            }
        }
    }

    assert(directions.size() > 0 && directions.size() <= 2);

    return directions;
}
