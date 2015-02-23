#ifndef __NOXIMROUTING_ODD_EVEN_H__
#define __NOXIMROUTING_ODD_EVEN_H__

#include "RoutingAlgorithm.h"
#include "RoutingAlgorithms.h"
#include "../Router.h"

using namespace std;

class Routing_ODD_EVEN : RoutingAlgorithm {
	public:
		vector<int> route(Router * router, const RouteData & routeData);

		static Routing_ODD_EVEN * getInstance();
        inline string name() { return "Routing_ODD_EVEN";};

	private:
		Routing_ODD_EVEN(){};
		~Routing_ODD_EVEN(){};

		static Routing_ODD_EVEN * routing_ODD_EVEN;
		static RoutingAlgorithmsRegister routingAlgorithmsRegister;
};

#endif
