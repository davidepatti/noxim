#include "SelectionStrategies.h"

SelectionStrategiesMap * SelectionStrategies::selectionStrategiesMap = 0;

SelectionStrategy * SelectionStrategies::get(const string & selectionStrategyName) {
	SelectionStrategiesMap::iterator it = getSelectionStrategiesMap()->find(selectionStrategyName);

	if(it == getSelectionStrategiesMap()->end())
		return 0;

	return it->second;
}

SelectionStrategiesMap * SelectionStrategies::getSelectionStrategiesMap() {
	if(selectionStrategiesMap == 0) 
		selectionStrategiesMap = new SelectionStrategiesMap();
	return selectionStrategiesMap; 
}
