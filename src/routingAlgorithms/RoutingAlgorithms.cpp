#include "RoutingAlgorithms.h"

RoutingAlgorithmsMap * RoutingAlgorithms::routingAlgorithmsMap = 0;

RoutingAlgorithm * RoutingAlgorithms::get(const string & routingAlgorithmName) {
	RoutingAlgorithmsMap::iterator it = getRoutingAlgorithmsMap()->find(routingAlgorithmName);

	if(it == getRoutingAlgorithmsMap()->end())
		return 0;

	return it->second;
}

RoutingAlgorithmsMap * RoutingAlgorithms::getRoutingAlgorithmsMap() {
	if(routingAlgorithmsMap == 0) 
		routingAlgorithmsMap = new RoutingAlgorithmsMap();
	return routingAlgorithmsMap; 
}
