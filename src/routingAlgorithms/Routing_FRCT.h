#ifndef __NOXIMROUTING_FRCT_H__
#define __NOXIMROUTING_FRCT_H__

#include "RoutingAlgorithm.h"
#include "RoutingAlgorithms.h"
#include "../Router.h"



using namespace std;

class Routing_FRCT : RoutingAlgorithm {
	public:
		vector<int> route(Router * router, const RouteData & routeData);

		static Routing_FRCT * getInstance();

	private:
		Routing_FRCT(){};
		~Routing_FRCT(){};
		vector<int> wirelessRouting(Coord current, Coord destination);
		vector<int> wiredRouting(Coord current, Coord destination);


		static Routing_FRCT * routing_FRCT;
		static RoutingAlgorithmsRegister routingAlgorithmsRegister;
};

#endif
