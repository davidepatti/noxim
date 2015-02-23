#ifndef __NOXIMROUTING_DYAD_H__
#define __NOXIMROUTING_DYAD_H__

#include "RoutingAlgorithm.h"
#include "RoutingAlgorithms.h"
#include "../Router.h"

using namespace std;

class Routing_DYAD : RoutingAlgorithm {
	public:
		vector<int> route(Router * router, const RouteData & routeData);

		static Routing_DYAD * getInstance();

	private:
		Routing_DYAD(){};
		~Routing_DYAD(){};

		static Routing_DYAD * routing_DYAD;
		static RoutingAlgorithmsRegister routingAlgorithmsRegister;
        static RoutingAlgorithm * odd_even;
};

#endif
