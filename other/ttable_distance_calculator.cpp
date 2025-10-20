#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <stdlib.h>

using namespace std;

int Coords2Node(int dim_x, int x, int y)
{
    return dim_x * y + x;
}

void Node2Coords(int dim_x, int node, int *x, int* y)
{
    *x = node % dim_x;
    *y = node / dim_x;
}

int HopDistance(int dim_x, int src, int dst)
{
    int src_x, src_y, dst_x, dst_y;
    
    Node2Coords(dim_x, src, &src_x, &src_y);
    Node2Coords(dim_x, dst, &dst_x, &dst_y);

    return abs(dst_x - src_x) + abs(dst_y - src_y);
}

void ParseTrafficTable(ostream &os, char *ttable_name, int dim_x, int dim_y, int distance)
{
    ifstream ifs(ttable_name);

    int src, dst;
    float pir;

    int short_range_counter = 0, long_range_counter = 0;
    float short_range_weighted_counter = 0, long_range_weighted_counter = 0;

    while (ifs >> src >> dst >> pir)
    {
        int hop_distance = HopDistance(dim_x, src, dst);

        os  << src << "\t" << dst << "\t" << pir 
            << " # " << (hop_distance <= distance ? "Short" : "Long") << " range (" << hop_distance << " hops)" << endl;

        if (hop_distance <= distance) 
        {
            short_range_counter++;
            short_range_weighted_counter += pir;
        }
        else 
        {
            long_range_counter++;
            long_range_weighted_counter += pir;
        }
    }

    int total_communications = short_range_counter + long_range_counter;
    float total_weighted_communications = short_range_weighted_counter + long_range_weighted_counter;

    os << "# " << 100.0 * short_range_counter / (total_communications) << "% of the total communication entries are short range (i.e., require " << distance << " or less hops)" << endl;
    
    os << "# " << 100.0 * short_range_weighted_counter / (total_weighted_communications) << "% of the total communications are short range (i.e., require " << distance << " or less hops)" << endl;

    os << "#"  << endl
       << "# " << "Average PIR (pkts/cycle):" << endl 
       << "# - Short communications: " << short_range_weighted_counter / float(short_range_counter) << endl
       << "# - Long communications:  " << long_range_weighted_counter / float(long_range_counter) << endl
       << "# - Total:  " << total_weighted_communications / float(total_communications) << endl;

}

bool ParseCommandLine(int argc, char **argv, char **fname, char **ttable_name, int *dim_x, int *dim_y, int *distance)
{
    *fname = argv[0];

    if (argc != 4 && argc != 5)
        return false;

    *ttable_name = argv[1];
    *dim_x = atoi(argv[2]);
    *dim_y = atoi(argv[3]);
    *distance = argc == 5 ? atoi(argv[4]) : 0;

    return true;
}

void HelpMessage(char *fname)
{
    cout << "Usage: " << fname << " ttable dim_x dim_y [distance]" << endl
         << endl
         << "Where:" << endl
         << "\t\"ttable\" is the file name of the traffic table to evaluate" << endl
         << "\t\"dim_x\" and \"dim_y\" are the dimensions of the mesh taken into account;" << endl
         << "\t\"distance\" is # of hops to discriminate between short and long range communications;" << endl
         << endl
         << "Example:" << endl
         << fname << " ttable.txt 8 8 3" << endl;
}

void PrintParams(ostream &os, char *ttable_name, int dim_x, int dim_y, int distance)
{
    os  <<  "# Params"                          << endl
        <<  "#   ttable_name: " << ttable_name  << endl
        <<  "#   dim_x: "       << dim_x        << endl
        <<  "#   dim_y: "       << dim_y        << endl
        <<  "#   distance: "    << distance     << endl
        <<  "# End of Params"                   << endl;
}

int main(int argc, char **argv)
{
    char *fname, *ttable_name;
    int dim_x, dim_y, distance;
    
    srand (time(NULL));

    if (!ParseCommandLine(argc, argv, &fname, &ttable_name, &dim_x, &dim_y, &distance))
    {
        HelpMessage(fname);
        return 1;
    }

    PrintParams(cerr, ttable_name, dim_x, dim_y, distance);

    ParseTrafficTable(cout, ttable_name, dim_x, dim_y, distance);
    
    /*
    ofstream ofs;
    ofs.open ("getNHopNeighbors.log", std::ofstream::out);
    test_GetNHopNeighbors(ofs, dim_x, dim_y, &traffic_data);
    ofs.close();
    */

    //PrintPairs(cout, dim_x, dim_y, distance);

    return 0;
}
