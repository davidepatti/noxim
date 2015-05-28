#ifndef __NOXIMSELECTIONSTRATEGY_H__
#define __NOXIMSELECTIONSTRATEGY_H__

#include <vector>
#include "../DataStructs.h"
#include "../Utils.h"

using namespace std;

struct Router;

class SelectionStrategy
{
	public:
        virtual int apply(Router * router, const vector < int >&directions, const RouteData & route_data) = 0;
        virtual void perCycleUpdate(Router * router) = 0;
};

#endif
