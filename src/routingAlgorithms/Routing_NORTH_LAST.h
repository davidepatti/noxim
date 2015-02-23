#ifndef __NOXIMROUTING_NORTH_LAST_H__
#define __NOXIMROUTING_NORTH_LAST_H__

#include "RoutingAlgorithm.h"
#include "RoutingAlgorithms.h"
#include "../Router.h"

using namespace std;

class Routing_NORTH_LAST : RoutingAlgorithm {
	public:
		vector<int> route(Router * router, const RouteData & routeData);

		static Routing_NORTH_LAST * getInstance();

	private:
		Routing_NORTH_LAST(){};
		~Routing_NORTH_LAST(){};

		static Routing_NORTH_LAST * routing_NORTH_LAST;
		static RoutingAlgorithmsRegister routingAlgorithmsRegister;
        static RoutingAlgorithm * xy;
};

#endif
