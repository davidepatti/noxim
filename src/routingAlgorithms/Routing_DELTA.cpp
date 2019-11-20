#include "Routing_DELTA.h"

RoutingAlgorithmsRegister Routing_DELTA::routingAlgorithmsRegister("DELTA", getInstance());

Routing_DELTA * Routing_DELTA::routing_DELTA = 0;

Routing_DELTA * Routing_DELTA::getInstance() {
    if ( routing_DELTA == 0 )
	routing_DELTA = new Routing_DELTA();

    return routing_DELTA;
}

vector<int> Routing_DELTA::route(Router * router, const RouteData & routeData)
{
    vector <int> directions;

   // int switch_offset = GlobalParams::n_delta_tiles;

    // first hop (core->1st stage)
    if (routeData.current_id  < GlobalParams::n_delta_tiles)
	directions.push_back(0); // for inputs cores
    else
    { // for switch bloc
	int destination = routeData.dst_id;
	// LOG << "I am switch: " <<routeData.current_id << "  _Going to destination: " <<destination<<endl;
	int currentStage = id2Coord(routeData.current_id).x;

	int shift_amount= log2(GlobalParams::n_delta_tiles)-1-currentStage;
	int direction = 1 & (destination >> shift_amount);

	// LOG << "I am again switch: " <<routeData.current_id << "  _Going to destination: " <<destination<< "  _Via direction "<<direction <<endl;

	directions.push_back(direction);
    }
    return directions;
}
