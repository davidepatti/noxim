#include "SelectionStrategies.h"

SelectionStrategiesMap * SelectionStrategies::selectionStrategiesMap = 0;

SelectionStrategy * SelectionStrategies::get(const string & routingAlgorithmName) {
	SelectionStrategiesMap::iterator it = getSelectionStrategiesMap()->find(routingAlgorithmName);

	if(it == getSelectionStrategiesMap()->end())
		return 0;

	return it->second;
}

SelectionStrategiesMap * SelectionStrategies::getSelectionStrategiesMap() {
	if(selectionStrategiesMap == 0) 
		selectionStrategiesMap = new SelectionStrategiesMap();
	return selectionStrategiesMap; 
}
