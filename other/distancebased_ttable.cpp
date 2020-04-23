#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <stdlib.h>

using namespace std;

struct traffic {
    int communications;
    int distance;
    float short_range_comms;
    float pir;
};

int Coords2Node(int dim_x, int x, int y)
{
    return dim_x * y + x;
}

void Node2Coords(int dim_x, int node, int *x, int* y)
{
    *x = node % dim_x;
    *y = node / dim_x;
}

vector<int> GetNHopNodes(ostream &os, int dim_x, int dim_y, bool find_neighbors, int center, int distance)
{
    vector<int> n_hop_nodes; 

    int center_x, center_y;
    Node2Coords(dim_x, center, &center_x, &center_y);

    os  << "{" << endl
        << "    \"center\": { \"id\": " << center << ", \"x\": " << center_x << ", \"y\": " << center_y << "}" << endl
        << "   ,\"n_hop_nodes\": [ {}" << endl;

    for (int x = 0; x < dim_x; x++)
    {
        int delta_x = x - center_x;

        for (int y = 0; y < dim_y; y++)
        {   
            bool is_neighbor = false;
            if ( center_x - distance <= x && x <= center_x + distance)
            {
                if (center_y - distance + abs(delta_x) <= y && y <= center_y + distance - abs(delta_x))
                {
                    is_neighbor = true;
                }
            }

            if (is_neighbor != find_neighbors) { continue; }

            int n_hop_node = Coords2Node(dim_x, x, y);

            if (n_hop_node == center) { continue; }

            os << "       ,{\"id\": " << n_hop_node  << ", \"delta_x\": " << delta_x << ", \"delta_y\": " << y - center_y << " }" << endl;

            n_hop_nodes.push_back(n_hop_node);
        }
    }
    os  << "    ]" << endl 
        << "}"   << endl;

    return n_hop_nodes;
}

void test_GetNHopNodes(ostream &os, int dim_x, int dim_y, bool find_neighbors, traffic *traffic_data)
{
    os << "[ {}" << endl;

    for (int center = 0; center < dim_x * dim_y; center++) 
    {
        os << "," ;
     
        vector<int> n_hop_nodes = GetNHopNodes(os, dim_x, dim_y, find_neighbors, center, traffic_data->distance);
     
        /*
        os << ",{ \"center\": " << center << ", \"n_hop_nodes\": {";

        unsigned int i;
        for ( i = 0 ; i < n_hop_nodes.size() - 1 ; i++ )
        {
            os << " " << n_hop_nodes.at(i) << ",";
        }
        os << " " << n_hop_nodes.at(i) << " }" << endl;
        */
    } 
    os << "]" << endl;
}

void PrintPairs(ostream &os, int dim_x, int dim_y, traffic *traffic_data)
{
    ofstream ofs;
    
    ofs.open ("dbttable_transmitting_pairs.log", std::ofstream::out);

    int short_range_comms = traffic_data->communications * traffic_data->short_range_comms;

    ofs << "[ {}" << endl;

    for (int i = 0; i < traffic_data->communications; i++)
    {
        // Select a random node
        int node = rand() % (dim_x * dim_y);


        vector<int> n_hop_nodes;

        ofs << "," ;

        if (i < short_range_comms) {
            n_hop_nodes = GetNHopNodes(ofs, dim_x, dim_y, true, node, traffic_data->distance);
        } 
        else 
        {
            n_hop_nodes = GetNHopNodes(ofs, dim_x, dim_y, false, node, traffic_data->distance);
        }

        int dst_index = rand() % n_hop_nodes.size();
        os  << node << "\t"
            << n_hop_nodes.at(dst_index) << "\t"
            << traffic_data->pir << endl;
    }

    ofs << "]" << endl;

    ofs.close();
}

bool ParseCommandLine(int argc, char **argv, char **fname, int *dim_x, int *dim_y, traffic *traffic_data)
{
    *fname = argv[0];

    if (argc != 7)
        return false;

    *dim_x = atoi(argv[1]);
    *dim_y = atoi(argv[2]);

    traffic_data->communications = atoi(argv[3]); 
    traffic_data->distance = atoi(argv[4]); 
    traffic_data->short_range_comms = stof(argv[5]); 
    traffic_data->pir = stof(argv[6]); 

    return true;
}

void HelpMessage(char *fname)
{
    cout << "Usage: " << fname << " dim_x dim_y distance short_range_comms pir" << endl
         << endl
         << "Where:" << endl
         << "\t\"dim_x\" and \"dim_y\" are the dimensions of the mesh taken into account;" << endl
         << "\t\"communications\" is number of entries in the traffic table;" << endl
         << "\t\"distance\" is # of hops to discriminate between short and long range communications;" << endl
         << "\t\"short_range_comms\" is the percentage (0 to 1) of short range communications;" << endl
         << "\t\"pir\" is the packet injection rate." << endl
         << endl
         << "Example:" << endl
         << fname << " 8 8 100 3 0.25 0.001" << endl;
}

void PrintParams(ostream &os, int dim_x, int dim_y, traffic *traffic_data)
{
    os  <<  "# Params" << endl
        <<  "dim_x: "                           << dim_x                            << endl
        <<  "dim_y: "                           << dim_y                            << endl
        <<  "traffic_data->communications: "    << traffic_data->communications     << endl 
        <<  "traffic_data->distance: "          << traffic_data->distance           << endl
        <<  "traffic_data->short_range_comms: " << traffic_data->short_range_comms  << endl 
        <<  "traffic_data->pir: "               << traffic_data->pir                << endl 
        <<  "# End of Params" << endl;
}

int main(int argc, char **argv)
{
    char *fname;
    int dim_x, dim_y;
    traffic traffic_data;
    
    srand (time(NULL));

    if (!ParseCommandLine(argc, argv, &fname, &dim_x, &dim_y, &traffic_data))
    {
        HelpMessage(fname);
        return 1;
    }

    PrintParams(cerr, dim_x, dim_y, &traffic_data);

    ofstream ofs;
    
    /*
    ofs.open ("getNHopNeighbors.log", std::ofstream::out);
    test_GetNHopNeighbors(ofs, dim_x, dim_y, &traffic_data);
    ofs.close();
    */
    
    ofs.open ("dbttable_getNHopNodes_neighbors.log", std::ofstream::out);
    test_GetNHopNodes(ofs, dim_x, dim_y, true, &traffic_data);
    ofs.close();

    ofs.open ("dbttable_getNHopNodes_remotes.log", std::ofstream::out);
    test_GetNHopNodes(ofs, dim_x, dim_y, false, &traffic_data);
    ofs.close();


    PrintPairs(cout, dim_x, dim_y, &traffic_data);

    return 0;
}
