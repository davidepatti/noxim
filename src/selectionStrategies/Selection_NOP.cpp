#include "Selection_NOP.h"

SelectionStrategiesRegister Selection_NOP::selectionStrategiesRegister("NOP", getInstance());

Selection_NOP * Selection_NOP::selection_NOP = 0;

Selection_NOP * Selection_NOP::getInstance() {
	if ( selection_NOP == 0 )
		selection_NOP = new Selection_NOP();
    
	return selection_NOP;
}

int Selection_NOP::apply(Router * router, const vector < int >&directions, const RouteData & route_data) {
    vector < int >neighbors_on_path;
    vector < int >score;
    int direction_selected = NOT_VALID;

    int current_id = route_data.current_id;

    for (size_t i = 0; i < directions.size(); i++) {
	// get id of adjacent candidate
	int candidate_id = router->getNeighborId(current_id, directions[i]);

	// apply routing function to the adjacent candidate node
	RouteData tmp_route_data;
	tmp_route_data.current_id = candidate_id;
	tmp_route_data.src_id = route_data.src_id;
	tmp_route_data.dst_id = route_data.dst_id;
	tmp_route_data.dir_in = router->reflexDirection(directions[i]);


	vector < int >next_candidate_channels =
	    router->routingFunction(tmp_route_data);

	// select useful data from Neighbor-on-Path input 
	NoP_data nop_tmp = router->NoP_data_in[directions[i]].read();

	// store the score of node in the direction[i]
	score.push_back(router->NoPScore(nop_tmp, next_candidate_channels));
    }

    // check for direction with higher score
    //int max_direction = directions[0];
    int max = score[0];
    for (unsigned int i = 0; i < directions.size(); i++) {
	if (score[i] > max) {
	//    max_direction = directions[i];
	    max = score[i];
	}
    }

    // if multiple direction have the same score = max, choose randomly.

    vector < int >equivalent_directions;

    for (unsigned int i = 0; i < directions.size(); i++)
	if (score[i] == max)
	    equivalent_directions.push_back(directions[i]);

    direction_selected =
	equivalent_directions[rand() % equivalent_directions.size()];

    return direction_selected;
}

void Selection_NOP::perCycleUpdate(Router * router) {
	    // update current input buffers level to neighbors
	    for (int i = 0; i < DIRECTIONS + 1; i++)
		router->free_slots[i].write(router->buffer[i][DEFAULT_VC].getCurrentFreeSlots());

	    // NoP selection: send neighbor info to each direction 'i'
	    NoP_data current_NoP_data = router->getCurrentNoPData();

	    for (int i = 0; i < DIRECTIONS; i++)
		router->NoP_data_out[i].write(current_NoP_data);
}
