#ifndef __NOXIMROUTING_DEFT_H__
#define __NOXIMROUTING_DEFT_H__

#include "RoutingAlgorithm.h"
#include "RoutingAlgorithms.h"
#include "../Router.h"

using namespace std;

class Routing_DEFT : RoutingAlgorithm {
    public:
        vector<int> route(Router * router, const RouteData & routeData);

        static Routing_DEFT * getInstance();

    private:
        Routing_DEFT(){};
        ~Routing_DEFT(){};

        static Routing_DEFT * routing_DEFT;
        static RoutingAlgorithmsRegister routingAlgorithmsRegister;
};

#endif
