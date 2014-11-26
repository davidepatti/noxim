/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the Network-on-Chip
 */

#include "NoC.h"

void NoC::buildMesh(char const * cfg_fname)
{
    char channel_name[16];
    channel = new Channel*[GlobalParams::channels_num];
    for (int i = 0; i < GlobalParams::channels_num; i++) {
        sprintf(channel_name, "Channel_%d", i);
        cout << "Creating " << channel_name << endl;
        channel[i] = new Channel(channel_name);
    }

    char hub_name[16];
    h = new Hub*[GlobalParams::hubs_num];

    for (int i = 0; i < GlobalParams::hubs_num; i++) {
        sprintf(hub_name, "HUB_%d", i);
        cout << "Creating " << hub_name << endl;
        h[i] = new Hub(hub_name, i);
        h[i]->clock(clock);
        h[i]->reset(reset);
    }

    // Check for routing table availability
    if (GlobalParams::routing_algorithm == ROUTING_TABLE_BASED)
	assert(grtable.load(GlobalParams::routing_table_filename));

    // Check for traffic table availability
    if (GlobalParams::traffic_distribution == TRAFFIC_TABLE_BASED)
	assert(gttable.load(GlobalParams::traffic_table_filename));

    int **hub_for_tile;

    hub_for_tile = new int*[GlobalParams::mesh_dim_y];
    assert(hub_for_tile != NULL);

    for(int i = 0; i < GlobalParams::mesh_dim_y; i++) {
        hub_for_tile[i] = new int[GlobalParams::mesh_dim_x];
        assert(hub_for_tile[i] != NULL);
    }

    for(int hub_id = 0; hub_id < GlobalParams::hubs_num; hub_id++) {
        // Determine, from configuration file, which Hub is connected to which Tile
        for (int i = 0; i < GlobalParams::hub_conf[hub_id].attachedNodes_num; i++){
            int tile_id = GlobalParams::hub_conf[hub_id].attachedNodes[i];
            hub_for_tile[tile_id % GlobalParams::mesh_dim_x][tile_id / GlobalParams::mesh_dim_x] = hub_id;
        }
        // Determine, from configuration file, which Hub is connected to which Channel
        //for (int i = 0; i < GlobalParams::hub_conf[hub_id].txChannels; i++){
        for (int i = 0; i < GlobalParams::hub_conf[hub_id].txChannels_num; i++){
            int channel_id = GlobalParams::hub_conf[hub_id].txChannels[i];
            cout << "Binding HUB_" << hub_id << " to txChannel " << channel_id << endl;
            h[hub_id]->init[channel_id]->socket.bind(channel[channel_id]->targ_socket);
        }
        //for (int i = 0; i < GlobalParams::hub_conf[hub_id].rxChannels; i++){
        for (int i = 0; i < GlobalParams::hub_conf[hub_id].rxChannels_num; i++){
            int channel_id = GlobalParams::hub_conf[hub_id].rxChannels[i];
            cout << "Binding HUB_" << hub_id << " to rxChannel " << channel_id << endl;
            channel[channel_id]->init_socket.bind(h[hub_id]->target[channel_id]->socket);
        }
    } 

    // DEBUG Print Tile / Hub connections 
    for (int i = 0; i < GlobalParams::mesh_dim_x; i++) {
	    for (int j = 0; j < GlobalParams::mesh_dim_y; j++) {
            cout << "Tile [" << i << "][" << j << "] is connected to HUB_" << hub_for_tile[i][j] << endl;
        }
    }

    // Var to track Hub connected ports
    int * hub_connected_ports = (int *) calloc(GlobalParams::hubs_num, sizeof(int));

    // Initialize signals
    req_to_east = new sc_signal<bool>*[GlobalParams::mesh_dim_x + 1];
    req_to_west = new sc_signal<bool>*[GlobalParams::mesh_dim_x + 1];
    req_to_south = new sc_signal<bool>*[GlobalParams::mesh_dim_x + 1];
    req_to_north = new sc_signal<bool>*[GlobalParams::mesh_dim_x + 1];

    ack_to_east = new sc_signal<bool>*[GlobalParams::mesh_dim_x + 1];
    ack_to_west = new sc_signal<bool>*[GlobalParams::mesh_dim_x + 1];
    ack_to_south = new sc_signal<bool>*[GlobalParams::mesh_dim_x + 1];
    ack_to_north = new sc_signal<bool>*[GlobalParams::mesh_dim_x + 1];

    flit_to_east = new sc_signal<Flit>*[GlobalParams::mesh_dim_x + 1];
    flit_to_west = new sc_signal<Flit>*[GlobalParams::mesh_dim_x + 1];
    flit_to_south = new sc_signal<Flit>*[GlobalParams::mesh_dim_x + 1];
    flit_to_north = new sc_signal<Flit>*[GlobalParams::mesh_dim_x + 1];

    free_slots_to_east = new sc_signal<int>*[GlobalParams::mesh_dim_x + 1];
    free_slots_to_west = new sc_signal<int>*[GlobalParams::mesh_dim_x + 1];
    free_slots_to_south = new sc_signal<int>*[GlobalParams::mesh_dim_x + 1];
    free_slots_to_north = new sc_signal<int>*[GlobalParams::mesh_dim_x + 1];

    req_to_hub = new sc_signal<bool>*[GlobalParams::mesh_dim_x + 1];
    ack_to_hub = new sc_signal<bool>*[GlobalParams::mesh_dim_x + 1];
    flit_to_hub = new sc_signal<Flit>*[GlobalParams::mesh_dim_x + 1];

    req_from_hub = new sc_signal<bool>*[GlobalParams::mesh_dim_x + 1];
    ack_from_hub = new sc_signal<bool>*[GlobalParams::mesh_dim_x + 1];
    flit_from_hub = new sc_signal<Flit>*[GlobalParams::mesh_dim_x + 1];

    NoP_data_to_east = new sc_signal<NoP_data>*[GlobalParams::mesh_dim_x + 1];
    NoP_data_to_west = new sc_signal<NoP_data>*[GlobalParams::mesh_dim_x + 1];
    NoP_data_to_south = new sc_signal<NoP_data>*[GlobalParams::mesh_dim_x + 1];
    NoP_data_to_north = new sc_signal<NoP_data>*[GlobalParams::mesh_dim_x + 1];

    for (int i=0; i < GlobalParams::mesh_dim_x + 1; i++) {
    	req_to_east[i] = new sc_signal<bool>[GlobalParams::mesh_dim_y + 1];
    	req_to_west[i] = new sc_signal<bool>[GlobalParams::mesh_dim_y + 1];
    	req_to_south[i] = new sc_signal<bool>[GlobalParams::mesh_dim_y + 1];
    	req_to_north[i] = new sc_signal<bool>[GlobalParams::mesh_dim_y + 1];

        ack_to_east[i] = new sc_signal<bool>[GlobalParams::mesh_dim_y + 1];
        ack_to_west[i] = new sc_signal<bool>[GlobalParams::mesh_dim_y + 1];
        ack_to_south[i] = new sc_signal<bool>[GlobalParams::mesh_dim_y + 1];
        ack_to_north[i] = new sc_signal<bool>[GlobalParams::mesh_dim_y + 1];

        flit_to_east[i] = new sc_signal<Flit>[GlobalParams::mesh_dim_y + 1];
        flit_to_west[i] = new sc_signal<Flit>[GlobalParams::mesh_dim_y + 1];
        flit_to_south[i] = new sc_signal<Flit>[GlobalParams::mesh_dim_y + 1];
        flit_to_north[i] = new sc_signal<Flit>[GlobalParams::mesh_dim_y + 1];

        free_slots_to_east[i] = new sc_signal<int>[GlobalParams::mesh_dim_y + 1];
        free_slots_to_west[i] = new sc_signal<int>[GlobalParams::mesh_dim_y + 1];
        free_slots_to_south[i] = new sc_signal<int>[GlobalParams::mesh_dim_y + 1];
        free_slots_to_north[i] = new sc_signal<int>[GlobalParams::mesh_dim_y + 1];

        req_to_hub[i] = new sc_signal<bool>[GlobalParams::mesh_dim_y + 1];
        ack_to_hub[i] = new sc_signal<bool>[GlobalParams::mesh_dim_y + 1];
        flit_to_hub[i] = new sc_signal<Flit>[GlobalParams::mesh_dim_y + 1];

        req_from_hub[i] = new sc_signal<bool>[GlobalParams::mesh_dim_y + 1];
        ack_from_hub[i] = new sc_signal<bool>[GlobalParams::mesh_dim_y + 1];
        flit_from_hub[i] = new sc_signal<Flit>[GlobalParams::mesh_dim_y + 1];

        NoP_data_to_east[i] = new sc_signal<NoP_data>[GlobalParams::mesh_dim_y + 1];
        NoP_data_to_west[i] = new sc_signal<NoP_data>[GlobalParams::mesh_dim_y + 1];
        NoP_data_to_south[i] = new sc_signal<NoP_data>[GlobalParams::mesh_dim_y + 1];
        NoP_data_to_north[i] = new sc_signal<NoP_data>[GlobalParams::mesh_dim_y + 1];
    }

    t = new Tile**[GlobalParams::mesh_dim_x];
    for (int i = 0; i < GlobalParams::mesh_dim_x; i++) {
    	t[i] = new Tile*[GlobalParams::mesh_dim_y];
    }

    // Create the mesh as a matrix of tiles
    for (int i = 0; i < GlobalParams::mesh_dim_x; i++) {
	for (int j = 0; j < GlobalParams::mesh_dim_y; j++) {
	    // Create the single Tile with a proper name
	    char tile_name[20];
	    sprintf(tile_name, "Tile[%02d][%02d]", i, j);
	    t[i][j] = new Tile(tile_name);

	    cout << "> Setting " << tile_name << endl;

	    // Tell to the router its coordinates
	    t[i][j]->r->configure(j * GlobalParams::mesh_dim_x + i,
				  GlobalParams::stats_warm_up_time,
				  GlobalParams::buffer_depth,
				  grtable);


	    // Tell to the PE its coordinates
	    t[i][j]->pe->local_id = j * GlobalParams::mesh_dim_x + i;
	    t[i][j]->pe->traffic_table = &gttable;	// Needed to choose destination
	    t[i][j]->pe->never_transmit = (gttable.occurrencesAsSource(t[i][j]->pe->local_id) == 0);

	    // Map clock and reset
	    t[i][j]->clock(clock);
	    t[i][j]->reset(reset);

	    // Map Rx signals
	    t[i][j]->req_rx[DIRECTION_NORTH] (req_to_south[i][j]);
	    t[i][j]->flit_rx[DIRECTION_NORTH] (flit_to_south[i][j]);
	    t[i][j]->ack_rx[DIRECTION_NORTH] (ack_to_north[i][j]);

	    t[i][j]->req_rx[DIRECTION_EAST] (req_to_west[i + 1][j]);
	    t[i][j]->flit_rx[DIRECTION_EAST] (flit_to_west[i + 1][j]);
	    t[i][j]->ack_rx[DIRECTION_EAST] (ack_to_east[i + 1][j]);

	    t[i][j]->req_rx[DIRECTION_SOUTH] (req_to_north[i][j + 1]);
	    t[i][j]->flit_rx[DIRECTION_SOUTH] (flit_to_north[i][j + 1]);
	    t[i][j]->ack_rx[DIRECTION_SOUTH] (ack_to_south[i][j + 1]);

	    t[i][j]->req_rx[DIRECTION_WEST] (req_to_east[i][j]);
	    t[i][j]->flit_rx[DIRECTION_WEST] (flit_to_east[i][j]);
	    t[i][j]->ack_rx[DIRECTION_WEST] (ack_to_west[i][j]);

	    // Map Tx signals
	    t[i][j]->req_tx[DIRECTION_NORTH] (req_to_north[i][j]);
	    t[i][j]->flit_tx[DIRECTION_NORTH] (flit_to_north[i][j]);
	    t[i][j]->ack_tx[DIRECTION_NORTH] (ack_to_south[i][j]);

	    t[i][j]->req_tx[DIRECTION_EAST] (req_to_east[i + 1][j]);
	    t[i][j]->flit_tx[DIRECTION_EAST] (flit_to_east[i + 1][j]);
	    t[i][j]->ack_tx[DIRECTION_EAST] (ack_to_west[i + 1][j]);

	    t[i][j]->req_tx[DIRECTION_SOUTH] (req_to_south[i][j + 1]);
	    t[i][j]->flit_tx[DIRECTION_SOUTH] (flit_to_south[i][j + 1]);
	    t[i][j]->ack_tx[DIRECTION_SOUTH] (ack_to_north[i][j + 1]);

	    t[i][j]->req_tx[DIRECTION_WEST] (req_to_west[i][j]);
	    t[i][j]->flit_tx[DIRECTION_WEST] (flit_to_west[i][j]);
	    t[i][j]->ack_tx[DIRECTION_WEST] (ack_to_east[i][j]);

	    // TODO: check if hub signal is always required
	    // signals/port when tile receives(rx) from hub
	    t[i][j]->hub_req_rx(req_from_hub[i][j]);
	    t[i][j]->hub_flit_rx(flit_from_hub[i][j]);
	    t[i][j]->hub_ack_rx(ack_to_hub[i][j]);

	    // signals/port when tile transmits(tx) to hub
	    t[i][j]->hub_req_tx(req_to_hub[i][j]); // 7, sc_out
	    t[i][j]->hub_flit_tx(flit_to_hub[i][j]);
	    t[i][j]->hub_ack_tx(ack_from_hub[i][j]);

        // TODO: Review port index. Connect each Hub to all its Channels 
        int hub_id = hub_for_tile[i][j];
	
        // The next time that the same HUB is considered, the next
        // port will be connected
        int port = hub_connected_ports[hub_id]++;

        h[hub_id]->req_rx[port](req_to_hub[i][j]);
        h[hub_id]->flit_rx[port](flit_to_hub[i][j]);
        h[hub_id]->ack_rx[port](ack_from_hub[i][j]);

        h[hub_id]->flit_tx[port](flit_from_hub[i][j]);
        h[hub_id]->req_tx[port](req_from_hub[i][j]);
        h[hub_id]->ack_tx[port](ack_to_hub[i][j]);

        // Map buffer level signals (analogy with req_tx/rx port mapping)
	    t[i][j]->free_slots[DIRECTION_NORTH] (free_slots_to_north[i][j]);
	    t[i][j]->free_slots[DIRECTION_EAST] (free_slots_to_east[i + 1][j]);
	    t[i][j]->free_slots[DIRECTION_SOUTH] (free_slots_to_south[i][j + 1]);
	    t[i][j]->free_slots[DIRECTION_WEST] (free_slots_to_west[i][j]);

	    t[i][j]->free_slots_neighbor[DIRECTION_NORTH] (free_slots_to_south[i][j]);
	    t[i][j]->free_slots_neighbor[DIRECTION_EAST] (free_slots_to_west[i + 1][j]);
	    t[i][j]->free_slots_neighbor[DIRECTION_SOUTH] (free_slots_to_north[i][j + 1]);
	    t[i][j]->free_slots_neighbor[DIRECTION_WEST] (free_slots_to_east[i][j]);

	    // NoP 
	    t[i][j]->NoP_data_out[DIRECTION_NORTH] (NoP_data_to_north[i][j]);
	    t[i][j]->NoP_data_out[DIRECTION_EAST] (NoP_data_to_east[i + 1][j]);
	    t[i][j]->NoP_data_out[DIRECTION_SOUTH] (NoP_data_to_south[i][j + 1]);
	    t[i][j]->NoP_data_out[DIRECTION_WEST] (NoP_data_to_west[i][j]);

	    t[i][j]->NoP_data_in[DIRECTION_NORTH] (NoP_data_to_south[i][j]);
	    t[i][j]->NoP_data_in[DIRECTION_EAST] (NoP_data_to_west[i + 1][j]);
	    t[i][j]->NoP_data_in[DIRECTION_SOUTH] (NoP_data_to_north[i][j + 1]);
	    t[i][j]->NoP_data_in[DIRECTION_WEST] (NoP_data_to_east[i][j]);

	}
    }

    // dummy NoP_data structure
    NoP_data tmp_NoP;

    tmp_NoP.sender_id = NOT_VALID;

    for (int i = 0; i < DIRECTIONS; i++) {
	tmp_NoP.channel_status_neighbor[i].free_slots = NOT_VALID;
	tmp_NoP.channel_status_neighbor[i].available = false;
    }

    // Clear signals for borderline nodes
    for (int i = 0; i <= GlobalParams::mesh_dim_x; i++) {
	req_to_south[i][0] = 0;
	ack_to_north[i][0] = 0;
	req_to_north[i][GlobalParams::mesh_dim_y] = 0;
	ack_to_south[i][GlobalParams::mesh_dim_y] = 0;

	free_slots_to_south[i][0].write(NOT_VALID);
	free_slots_to_north[i][GlobalParams::mesh_dim_y].write(NOT_VALID);

	NoP_data_to_south[i][0].write(tmp_NoP);
	NoP_data_to_north[i][GlobalParams::mesh_dim_y].write(tmp_NoP);

    }

    for (int j = 0; j <= GlobalParams::mesh_dim_y; j++) {
	req_to_east[0][j] = 0;
	ack_to_west[0][j] = 0;
	req_to_west[GlobalParams::mesh_dim_x][j] = 0;
	ack_to_east[GlobalParams::mesh_dim_x][j] = 0;

	free_slots_to_east[0][j].write(NOT_VALID);
	free_slots_to_west[GlobalParams::mesh_dim_x][j].write(NOT_VALID);

	NoP_data_to_east[0][j].write(tmp_NoP);
	NoP_data_to_west[GlobalParams::mesh_dim_x][j].write(tmp_NoP);

    }
    

    // invalidate reservation table entries for non-exhistent channels
    for (int i = 0; i < GlobalParams::mesh_dim_x; i++) {
	t[i][0]->r->reservation_table.invalidate(DIRECTION_NORTH);
	t[i][GlobalParams::mesh_dim_y - 1]->r->reservation_table.invalidate(DIRECTION_SOUTH);
    }
    for (int j = 0; j < GlobalParams::mesh_dim_y; j++) {
	t[0][j]->r->reservation_table.invalidate(DIRECTION_WEST);
	t[GlobalParams::mesh_dim_x - 1][j]->r->reservation_table.invalidate(DIRECTION_EAST);
    }
}

Tile *NoC::searchNode(const int id) const
{
    for (int i = 0; i < GlobalParams::mesh_dim_x; i++)
	for (int j = 0; j < GlobalParams::mesh_dim_y; j++)
	    if (t[i][j]->r->local_id == id)
		return t[i][j];

    return NULL;
}
