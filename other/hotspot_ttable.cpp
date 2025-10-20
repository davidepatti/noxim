#include <iostream>
#include <string>
#include <stdlib.h>

using namespace std;

struct traffic {
    float pairs_prob;
    int src_tl, src_br;
    int dst_tl, dst_br;
    float pir_min, pir_max;
};

bool ParseCommandLine(int argc, char **argv, char **fname, int *dim_x, int *dim_y, traffic *traffic_data)
{
    *fname = argv[0];

    if (argc != 9 && argc != 10)
        return false;

    *dim_x = atoi(argv[1]);
    *dim_y = atoi(argv[2]);

    traffic_data->pairs_prob = stof(argv[3]);
    traffic_data->src_tl = atoi(argv[4]); 
    traffic_data->src_br = atoi(argv[5]); 
    traffic_data->dst_tl = atoi(argv[6]); 
    traffic_data->dst_br = atoi(argv[7]); 
    traffic_data->pir_min = stof(argv[8]); 
    traffic_data->pir_max = argc == 9 ? traffic_data->pir_min : stof(argv[9]);

    return true;
}

void HelpMessage(char *fname)
{
    cout << "Usage: " << fname << " dim_x dim_y pairs_prob src dst pir" << endl
         << endl
         << "Where:" << endl
         << "\t\"dim_x\" and \"dim_y\" are the dimensions of the mesh taken into account;"<< endl
         << "\t\"pairs_prob\" is the probability for each pair of nodes from \"src\" and \"dst\" regions to communicate;" << endl
         << "\t\"src\" and \"dst\" are rectangular regions specified by top left corner and bottom right corners;" << endl
         << "\t\"pir\" or packet injection rate, may be a specific value or a range (two values)." << endl
         << endl
         << "Example:" << endl
         << fname << " 8 8 0.5 0 18 52 63 0.001" << endl
         << fname << " 8 8 0.5 0 18 52 63 0.001 0.005" << endl;
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

void PrintPairs(ostream &os, int dim_x, int dim_y, traffic *traffic_data)
{
    int *src_region, *dst_region;
    int src_region_size, dst_region_size;

    GetNodesInRegion(dim_x, traffic_data->src_br, traffic_data->src_tl, &src_region, &src_region_size);
    GetNodesInRegion(dim_x, traffic_data->dst_br, traffic_data->dst_tl, &dst_region, &dst_region_size);

    for (int s = 0; s < src_region_size; s++) 
    {
        for (int d = 0; d < dst_region_size; d++)
        {
            if ((float)rand() / RAND_MAX < traffic_data->pairs_prob)
            {
                if (src_region[s] != dst_region[d])
                {
                    os  << src_region[s] << "\t"
                        << dst_region[d] << "\t"
                        << (float)rand() / RAND_MAX * (traffic_data->pir_max - traffic_data->pir_min) + traffic_data->pir_min << endl;
                }
            }
        }
    } 

}

void PrintParams(ostream &os, int dim_x, int dim_y, traffic *traffic_data)
{
    os  <<  "# Params" << endl
        <<  "dim_x: "                       << dim_x                        << endl
        <<  "dim_y: "                       << dim_y                        << endl
        <<  "traffic_data->pairs_prob: "    << traffic_data->pairs_prob     << endl
        <<  "traffic_data->src_tl: "        << traffic_data->src_tl         << endl 
        <<  "traffic_data->src_br: "        << traffic_data->src_br         << endl 
        <<  "traffic_data->dst_tl: "        << traffic_data->dst_tl         << endl 
        <<  "traffic_data->dst_br: "        << traffic_data->dst_br         << endl 
        <<  "traffic_data->pir_min: "       << traffic_data->pir_min        << endl 
        <<  "traffic_data->pir_max: "       << traffic_data->pir_max        << endl 
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

    //PrintParams(cerr, dim_x, dim_y, &traffic_data);

    PrintPairs(cout, dim_x, dim_y, &traffic_data);

    return 0;
}
