#ifndef __NOXIMROUTING_NEGATIVE_FIRST_H__
#define __NOXIMROUTING_NEGATIVE_FIRST_H__

#include "RoutingAlgorithm.h"
#include "RoutingAlgorithms.h"
#include "../Router.h"

using namespace std;

class Routing_NEGATIVE_FIRST : RoutingAlgorithm {
	public:
		vector<int> route(Router * router, const RouteData & routeData);

		static Routing_NEGATIVE_FIRST * getInstance();

	private:
		Routing_NEGATIVE_FIRST(){};
		~Routing_NEGATIVE_FIRST(){};

		static Routing_NEGATIVE_FIRST * routing_NEGATIVE_FIRST;
		static RoutingAlgorithmsRegister routingAlgorithmsRegister;
        static RoutingAlgorithm * xy;
};

#endif
