#ifndef __NOXIMROUTING_FULLY_ADAPTIVE_H__
#define __NOXIMROUTING_FULLY_ADAPTIVE_H__

#include "RoutingAlgorithm.h"
#include "RoutingAlgorithms.h"

using namespace std;

class Routing_FULLY_ADAPTIVE : RoutingAlgorithm {
	public:
		vector<int> route(Router * router, const RouteData & routeData);

		static Routing_FULLY_ADAPTIVE * getInstance();

	private:
		Routing_FULLY_ADAPTIVE(){};
		~Routing_FULLY_ADAPTIVE(){};

		static Routing_FULLY_ADAPTIVE * routing_FULLY_ADAPTIVE;
		static RoutingAlgorithmsRegister routingAlgorithmsRegister;
};

#endif
