#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>


using namespace std;

// From noxim/src/DataStructs.h
// Coord -- XY coordinates type of the Tile inside the Mesh
class Coord {
  public:
    int x;			// X coordinate
    int y;			// Y coordinate

    inline bool operator ==(const Coord & coord) const {
	return (coord.x == x && coord.y == y);
}};

// adapted from noxim/src/Utils.h
inline Coord id2Coord(int id, int mesh_dim_x, int mesh_dim_y)
{
    Coord coord;

    coord.x = id % mesh_dim_x;
    coord.y = id / mesh_dim_x;

    assert(coord.x < mesh_dim_x);
    assert(coord.y < mesh_dim_y);
    
    return coord;
}

// Adapted from noxim/src/Utils.h
inline int getWiredDistanceC(Coord node1, Coord node2, int frct_threshold)
{
    // cout << "FRCT_THR: " << GlobalParams::frct_threshold << endl;
    return (abs (node1.x - node2.x) + abs (node1.y - node2.y) + frct_threshold);
}

// From noxim/src/Utils.h
inline int getWiredDistanceI(int node1_id, int  node2_id, int mesh_dim_x, int mesh_dim_y, int frct_threshold)
{
    Coord node1 = id2Coord (node1_id, mesh_dim_x, mesh_dim_y);
    Coord node2 = id2Coord (node2_id, mesh_dim_x, mesh_dim_y);

    return getWiredDistanceC(node1, node2, frct_threshold);
}

// Adapted from noxim/src/Utils.h
inline Coord getClosestNodeAttachedToRadioHubC(Coord node, int CLUSTER_WIDTH, int CLUSTER_HEIGHT, int frct_threshold)
{
    int cluster_x = node.x/CLUSTER_WIDTH;
    int cluster_y = node.y/CLUSTER_HEIGHT;

    Coord routerTop;
    routerTop.x = cluster_x*CLUSTER_WIDTH;
    routerTop.y = cluster_y*CLUSTER_HEIGHT;
    Coord router4;
    router4.x = routerTop.x + CLUSTER_WIDTH/2;
    router4.y = routerTop.y + CLUSTER_HEIGHT/2;

    // other router 1 2 3 //
    Coord router3, router2, router1;
    router3.x = router4.x;
    router3.y = router4.y - 1 ;

    router2.x = router4.x - 1 ;
    router2.y = router4.y;

    router1.x = router4.x - 1 ;
    router1.y = router4.y - 1 ;

    // distance from node to 4 attached router
    int dis1 = getWiredDistanceC(node, router1, frct_threshold);
    int dis2 = getWiredDistanceC(node, router2, frct_threshold);
    int dis3 = getWiredDistanceC(node, router3, frct_threshold);
    int dis4 = getWiredDistanceC(node, router4, frct_threshold);

    // Closest distance
    int min12 = min (dis1, dis2);
    int min34 = min (dis3, dis4);
    int win = min (min12, min34);

    // Closest router
    Coord winner ;
    if (win == dis1)
        winner = router1;
    else if (win == dis2)
        winner = router2;
    else if (win == dis3)
        winner = router3;
    else
        winner = router4;

    return winner;



}

// From noxim/src/Utils.h
inline int getWirelessDistance(int node1_id, int node2_id, int mesh_dim_x, int mesh_dim_y, int CLUSTER_WIDTH, int CLUSTER_HEIGHT, int frct_threshold)
{
    Coord node1 = id2Coord (node1_id, mesh_dim_x, mesh_dim_y);
    Coord node2 = id2Coord (node2_id, mesh_dim_x, mesh_dim_y);

    Coord closest_n_attached_rh1 = getClosestNodeAttachedToRadioHubC (node1, CLUSTER_WIDTH, CLUSTER_HEIGHT, frct_threshold);
    Coord closest_n_attached_rh2 = getClosestNodeAttachedToRadioHubC (node2, CLUSTER_WIDTH, CLUSTER_HEIGHT, frct_threshold);

    return (getWiredDistanceC(node1, closest_n_attached_rh1, frct_threshold) + getWiredDistanceC(node2, closest_n_attached_rh2, frct_threshold) + 1);

}

bool prefersWirelessPath(int src, int dst, int dim_x, int dim_y, int dim_region_x, int dim_region_y, int frct_threshold)
{
    int dist_wired = getWiredDistanceI(src, dst, dim_x, dim_y, frct_threshold);
    int dist_wireless = getWirelessDistance(src, dst, dim_x, dim_y, dim_region_x, dim_region_y, frct_threshold);

    return dist_wired >= dist_wireless;
}

void GetNodesInRegion(int dim_x, int br, int tl, int **nodes_array, int *nodes_array_len) 
{
    int X = br % dim_x - tl % dim_x;
    int Y = br / dim_x - tl / dim_x;

    *nodes_array_len = (X + 1) * (Y + 1);

#ifdef LOG
    LOG << "Region size: " << *nodes_array_len << endl;
#endif

    *nodes_array = (int*)malloc(*nodes_array_len * sizeof(int));
    
    int i = 0;
    
    for (int y = 0; y <= Y; y++)
    {
        for (int x = 0; x <= X; x++) 
        {
            (*nodes_array)[i] = tl + dim_x * y + x;
        #ifdef LOG
            LOG << (*nodes_array)[i] << endl;
        #endif
            i++;
        }
    }
}

void ParseTrafficTable(ostream &os, char *ttable_name, int dim_x, int dim_y, int dim_region_x, int dim_region_y, int communications, int frct_threshold)
{
    ifstream ifs(ttable_name);

    int src, dst;
    float pir;
    
    int hub_x = dim_x / dim_region_x;
    int region_last_node_offset = dim_x * (dim_region_y - 1) + (dim_region_x - 1);

    float *random_pirs = (float*)malloc(communications * sizeof(float));

    while (ifs >> src >> dst >> pir)
    {
        int *src_region, *dst_region;
        int src_region_size, dst_region_size;

        int src_tl = src % hub_x * dim_region_x + src / hub_x * dim_x * dim_region_y;
        int src_br = src_tl + region_last_node_offset;
        int dst_tl = dst % hub_x * dim_region_x + dst / hub_x * dim_x * dim_region_y;
        int dst_br = dst_tl + region_last_node_offset;

        cerr << "Creating communications "
             << "from HUB_" << src << " (corner nodes: " << src_tl << ", " << src_br << ") " 
             << "to HUB_" << dst << " (corner nodes: " << dst_tl << ", " << dst_br << ")" << endl; 

        GetNodesInRegion(dim_x, src_br, src_tl, &src_region, &src_region_size);
        GetNodesInRegion(dim_x, dst_br, dst_tl, &dst_region, &dst_region_size);

        float random_pirs_sum = 0;
        for (int i = 0; i < communications; i++) 
        {
            random_pirs[i] = (float)rand() / RAND_MAX;
            random_pirs_sum += random_pirs[i];
        }

        float pir_ratio = pir / random_pirs_sum;

        for (int i = 0; i < communications; i++) 
        {
            int src_id;
            int dst_id;
            int counter = 0;
            
            do 
            {
                src_id = rand() % src_region_size;
                dst_id = rand() % dst_region_size;
                counter++;
            } while (prefersWirelessPath(src_region[src_id], dst_region[dst_id], dim_x, dim_y, dim_region_x, dim_region_y, frct_threshold ) == false && counter < 100000);
            
            cerr << src_region[src_id] << "\t=>\t" << dst_region[dst_id] << ((counter < 100000) ? "\tWireless" : "\tWired") << endl; 

            os << src_region[src_id] << "\t" << dst_region[dst_id] << "\t" << random_pirs[i] * pir_ratio << endl;
        }
    }

}

bool ParseCommandLine(int argc, char **argv, char **fname, char **ttable_name, int *dim_x, int *dim_y, int *dim_region_x, int *dim_region_y, int *communications, int *frct_threshold)
{
    *fname = argv[0];

    if (argc != 7 && argc != 8)
        return false;

    *ttable_name = argv[1];
    *dim_x = atoi(argv[2]);
    *dim_y = atoi(argv[3]);
    *dim_region_x = atoi(argv[4]);
    *dim_region_y = atoi(argv[5]);
    *communications = atoi(argv[6]);
    *frct_threshold = argc == 8 ? atoi(argv[7]) : 0;


    return true;
}

void HelpMessage(char *fname)
{
    cout << "Usage: " << fname << " ttable dim_x dim_y dim_region_x dim_region_y communications" << endl
         << endl
         << "Where:" << endl
         << "\t\"ttable\" is the file name of the traffic table to evaluate" << endl
         << "\t\"dim_x\" and \"dim_y\" are the dimensions of the mesh taken into account;" << endl
         << "\t\"dim_region_x\" and \"dim_region_y\" are the dimensions for the regions of each hub;" << endl
         << "\t\"communications\" is # of communications for each line of the input ttable;" << endl
         << "\t\"frct_threshold\" is an OPTIONAL (default = 0) threshold for FRCT routing algorithm." << endl
         << endl
         << "Example:" << endl
         << fname << " ttable.txt 32 32 4 8 5" << endl
         << fname << " ttable.txt 32 32 4 8 5 4" << endl;
}

void PrintParams(ostream &os, char *ttable_name, int dim_x, int dim_y, int dim_region_x, int dim_region_y, int communications, int frct_threshold)
{
    os  <<  "# Params"                              << endl
        <<  "ttable_name: "     << ttable_name      << endl
        <<  "dim_x: "           << dim_x            << endl
        <<  "dim_y: "           << dim_y            << endl
        <<  "dim_region_x: "    << dim_region_x     << endl
        <<  "dim_region_y: "    << dim_region_y     << endl
        <<  "communications: "  << communications   << endl
        <<  "frct_threshold: "  << frct_threshold   << endl
        <<  "# End of Params"                       << endl;
}

int main(int argc, char **argv)
{
    char *fname, *ttable_name;
    int dim_x, dim_y, dim_region_x, dim_region_y, communications, frct_threshold;
    
    srand (time(NULL));

    if (!ParseCommandLine(argc, argv, &fname, &ttable_name, &dim_x, &dim_y, &dim_region_x, &dim_region_y, &communications, &frct_threshold))
    {
        HelpMessage(fname);
        return 1;
    }

    PrintParams(cerr, ttable_name, dim_x, dim_y, dim_region_x, dim_region_y, communications, frct_threshold);

    ParseTrafficTable(cout, ttable_name, dim_x, dim_y, dim_region_x, dim_region_y, communications, frct_threshold);
    
    return 0;
}
