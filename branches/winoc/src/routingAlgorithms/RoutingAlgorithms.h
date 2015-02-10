#ifndef __NOXIMROUTINGALGORITHMS_H__
#define __NOXIMROUTINGALGORITHMS_H__

#include <map>
#include <string>
#include "RoutingAlgorithm.h"

using namespace std;

typedef map<string, RoutingAlgorithm * > RoutingAlgorithmsMap;

class RoutingAlgorithms {
	public:
		static RoutingAlgorithmsMap * routingAlgorithmsMap;
		static RoutingAlgorithmsMap * getRoutingAlgorithmsMap();

		static RoutingAlgorithm * get(const string & routingAlgorithmName);
};

struct RoutingAlgorithmsRegister : RoutingAlgorithms {
	RoutingAlgorithmsRegister(const string & routingAlgorithmName, RoutingAlgorithm * routingAlgorithm) {
		getRoutingAlgorithmsMap()->insert(make_pair(routingAlgorithmName, routingAlgorithm));
	}
};

#endif
