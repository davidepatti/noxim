#ifndef __NOXIMROUTING_BFLY_H__
#define __NOXIMROUTING_BFLY_H__

#include "RoutingAlgorithm.h"
#include "RoutingAlgorithms.h"
#include "../Router.h"

using namespace std;

class Routing_BFLY : RoutingAlgorithm {
	public:
		vector<int> route(Router * router, const RouteData & routeData);

		static Routing_BFLY * getInstance();

	private:
		Routing_BFLY(){};
		~Routing_BFLY(){};

		static Routing_BFLY * routing_BFLY;
		static RoutingAlgorithmsRegister routingAlgorithmsRegister;
};

#endif
