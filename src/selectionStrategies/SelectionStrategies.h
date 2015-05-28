#ifndef __NOXIMSELECTIONSTRATEGIES_H__
#define __NOXIMSELECTIONSTRATEGIES_H__

#include <map>
#include <string>
#include "SelectionStrategy.h"

using namespace std;

typedef map<string, SelectionStrategy* > SelectionStrategiesMap;

class SelectionStrategies {
	public:
		static SelectionStrategiesMap * selectionStrategiesMap;
		static SelectionStrategiesMap * getSelectionStrategiesMap();

		static SelectionStrategy * get(const string & selectionStrategyName);
};

struct SelectionStrategiesRegister : SelectionStrategies {
	SelectionStrategiesRegister(const string & selectionStrategyName, SelectionStrategy * selectionStrategy) {
		getSelectionStrategiesMap()->insert(make_pair(selectionStrategyName, selectionStrategy));
	}
};

#endif
