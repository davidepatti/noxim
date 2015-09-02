#include "Selection_RANDOM.h"

SelectionStrategiesRegister Selection_RANDOM::selectionStrategiesRegister("RANDOM", getInstance());

Selection_RANDOM * Selection_RANDOM::selection_RANDOM = 0;

Selection_RANDOM * Selection_RANDOM::getInstance() {
	if ( selection_RANDOM == 0 )
		selection_RANDOM = new Selection_RANDOM();
    
	return selection_RANDOM;
}

int Selection_RANDOM::apply(Router * router, const vector < int >&directions, const RouteData & route_data){

    return directions[rand() % directions.size()];
}

void Selection_RANDOM::perCycleUpdate(Router * router){ }
