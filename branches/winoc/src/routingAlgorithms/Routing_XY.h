#ifndef __NOXIMROUTING_XY_H__
#define __NOXIMROUTING_XY_H__

#include "RoutingAlgorithm.h"
#include "RoutingAlgorithms.h"
#include "../Router.h"

using namespace std;

class Routing_XY : RoutingAlgorithm {
	public:
		vector<int> route(Router * router, const RouteData & routeData);

		static Routing_XY * getInstance();

	private:
		Routing_XY(){};
		~Routing_XY(){};

		static Routing_XY * routing_XY;
		static RoutingAlgorithmsRegister routingAlgorithmsRegister;
};

#endif
