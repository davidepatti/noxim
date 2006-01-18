#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <vector>
#include <cassert>

using namespace std;

struct Tdim
{
    int dimx,dimy;
};

void print_values(const vector<int>& v)
{
    for (unsigned int i=0;i<v.size();i++) cout << v[i] << " ";
}

void print_values(const vector<string>& v)
{
    for (unsigned int i=0;i<v.size();i++) cout << v[i] << " ";
}
void print_values(const vector<Tdim>& v)
{
    for (unsigned int i=0;i<v.size();i++) cout << "(" << v[i].dimx << ","<< v[i].dimy<<") ";
}
    
string routing2string(int r)
{
    switch (r)
    {
	case 1: return string("xy");
	case 2: return string("westfirst");
	case 3: return string("northlast");
	case 4: return string("negativefirst");
	case 5: return string("oddeven");
	case 6: return string("dyad");
	case 7: return string("fullyadaptive");
	case 8: return string("table");
	default: assert(false);
    }
    return string("au");
}

string traffic2string(int r)
{
    switch (r)
    {
	case 1: return string("uniform");
	case 2: return string("transpose1");
	case 3: return string("transpose2");
	case 4: return string("table");
	default: assert(false);
    }
}

string selection2string(int r)
{
    switch (r)
    {
	case 1: return string("random");
	case 2: return string("bufferlevel");
	case 3: return string("nop");
	default: assert(false);
    }
}


int main()
{
    vector<Tdim> topology;
    vector<int> traffic;
    vector<int> size;
    vector<int> buffer;
    vector<int> routing;
    vector<int> selection;
    vector<string> aggregate;
    string traffic_table;

    int repetitions,sim_length;
    string simulator_path;
    //int seed;
    int warmup;

    int tmp_par;
    string word;

    float min_pir,max_pir,step;

    system("clear");

    cout << "\n -------------------------------------------------------";
    cout << "\n  s p a c e f i l e g e n ";
    cout << "\n -------------------------------------------------------";
    cout << "\n   space file generator for Noxim NoC simulator";
    cout << endl << endl;

    // --------------------------------------------------------------------------
    Tdim tmp_dim;
    do
    {
	cout << "\n Add Mesh Topology";
	cout << "\n ----------------------------------------------------";
	cout << "\n current values = [ ";
	print_values(topology);
	cout << " ]" << endl;
	cout << "\n Dim X: ";
	cin >> tmp_dim.dimx;
	cout << "\n Dim Y: ";
	cin >> tmp_dim.dimy;
	topology.push_back(tmp_dim);

	cout << "\n Ok, " << tmp_dim.dimx << " x " << tmp_dim.dimy << " topology added!" << endl;
	cout << "\n (a)dd another topology";
	cout << "\n (c)ontinue with other parameters";
	cout << "\n choice:";
	cin >> word;
    }
    while (word=="a");

    // --------------------------------------------------------------------------

    do
    {
	system("clear");
	cout << "\n Add Traffic";
	cout << "\n ----------------------------------------------------";
	cout << "\n current values = [ ";
	print_values(traffic);
	cout << " ]" << endl;
	cout << "\n (1) uniform";
	cout << "\n (2) transpose1 ";
	cout << "\n (3) transpose2 ";
	cout << "\n (4) table";
	cout << "\n (c)ontinue with other parameters";
	cout << "\n\n choice : ";
	cin >> word;

	if (word!="c")
	{
	    tmp_par = atoi(word.c_str());

	    if (tmp_par == 4) 
	    {
		cout << "\n Enter file name for table-based traffic:";
		cin >> traffic_table;
	    }
	    traffic.push_back(tmp_par);
	}
    }
    while (word!="c");

    // --------------------------------------------------------------------------

    do
    {
	system("clear");
	cout << "\n  Add Routing type";
	cout << "\n ----------------------------------------------------";
	cout << "\n current values = [ ";
	print_values(routing);
	cout << " ]" << endl;
	cout << "\n (1) xy              XY routing algorithm";
	cout << "\n (2) westfirst       West-First routing algorithm";
	cout << "\n (3) northlast       North-Last routing algorithm";
	cout << "\n (4) negativefirst   Negative-First routing algorithm";
	cout << "\n (5) oddeven         Odd-Even routing algorithm";
	cout << "\n (6) dyad            DyAD routing algorithm";
	cout << "\n (7) fullyadaptive   Fully-Adaptive routing algorithm";
	cout << "\n (8) table FILENAME ";
	cout << "\n (c)ontinue with other parameters";
	cout << "\n\n choice : ";
	cin >> word;
	if (word!="c")
	{
	    tmp_par = atoi(word.c_str());
	    routing.push_back(tmp_par);
	}

    } while (word!="c");

    // --------------------------------------------------------------------------
    
    do
    {
	system("clear");
	cout << "\n   Add selection strategy";
	cout << "\n ---------------------------------------------------";
	cout << "\n current values = [ ";
	print_values(selection);
	cout << " ]" << endl;
	cout << "\n (1) random ";
	cout << "\n (2) bufferlevel ";
	cout << "\n (3) nop ";
	cout << "\n (c)ontinue with other parameters";
	cout << "\n\n choice : ";
	cin >> word;
	if (word!="c")
	{
	    tmp_par = atoi(word.c_str());
	    selection.push_back(tmp_par);
	}

    } while (word!="c");

    // --------------------------------------------------------------------------
    do
    {
	system("clear");
	cout << "\n   Add packet size value";
	cout << "\n ---------------------------------------------------";
	cout << "\n current values = [ ";
	print_values(size);
	cout << " ]";
	cout << endl;
	cout << "\n Enter a packet size (flits) (c to continue):";
	cin >> word;
	if (word!="c")
	{
	    tmp_par = atoi(word.c_str());
	    size.push_back(tmp_par);
	}

    } while (word!="c");

    // --------------------------------------------------------------------------

    do
    {
	system("clear");
	cout << "\n   Add buffer size value";
	cout << "\n ---------------------------------------------------";
	cout << "\n current values = [ ";
	print_values(buffer);
	cout << " ]" << endl;
	cout << "\n Enter a buffer size (flits) (c to continue):";
	cin >> word;
	if (word!="c")
	{
	    tmp_par = atoi(word.c_str());
	    buffer.push_back(tmp_par);
	}

    } while (word!="c");

    system("clear");
    cout << "\n   Traffic generation ";
    cout << "\n ---------------------------------------------------";
    cout << "\n Min PIR: ";
    cin >> min_pir;
    cout << "\n Max PIR: ";
    cin >> max_pir;
    cout << "\n PIR step: ";
    cin >> step;
    cout << "\n n. repetitions for each PIR point: ";
    cin >> repetitions;

    system("clear");
    cout << "\n    noxim simulator path";
    cout << "\n ---------------------------------------------------";
    cout << "\n (1) assume default [ /usr/design/noxim_cvs/noxim ]";
    cout << "\n (2) specify new path";
    cout << "\n choice:";
    cin >> word;

    if (word=="1") simulator_path = "/usr/design/noxim_cvs/noxim";
    else
    {
	cout << "\n Enter the new path:";
	cin >> simulator_path;
    }


    cout << "\n Simulation length (cycles): ";
    cin >> sim_length;

    //cout << "\n Seed: ";
    //cin >> seed;

    cout << "\n warmup cyles: ";
    cin >> warmup;

    do 
    {
	system("clear");
	cout << "\n Add aggregation parameter";
	cout << "\n ---------------------------------------------------";
	cout << "\n current values = [ ";
	print_values(aggregate);
	cout << " ]";
	cout << endl;
	cout << "\n (for a list of parameters see noxim -help) " << endl;
	cout << "\n Enter a parameter string (c to continue):";
	cin >> word;
	if (word!="c") 
	{
	    aggregate.push_back(word);
	}
    }
    while (word!="c");

    cout << "\n Choose filename to save the spacefile: ";
    cin >> word;
    std::ofstream space_file(word.c_str());

    space_file << "\n\% automatically generated by noxim spacefilegen ";

    space_file << endl;

    space_file << "\n[routing]";
    for (unsigned int i=0;i<routing.size();i++)
	space_file << "\n" << routing2string(routing[i]);
    space_file << "\n[/routing]";

    space_file << endl;

    space_file << "\n[sel]";
    for (unsigned int i=0;i<selection.size();i++)
	space_file << "\n" << selection2string(selection[i]);
    space_file << "\n[/sel]";

    space_file << endl;

    space_file << "\n[traffic]";
    for (unsigned int i=0;i<traffic.size();i++)
	space_file << "\n" << traffic2string(traffic[i]);
    space_file << "\n[/traffic]";

    space_file << endl;

    space_file << "\n[pir]";
    space_file << "\n\% min max step ";
    space_file << "\n" << min_pir << " " << max_pir << " " << step;
    space_file << "\n[/pir]";

    space_file << endl;
    
    space_file << "\n[topology]"; 
    for (unsigned int i=0;i<topology.size();i++)
	space_file << "\n" << topology[i].dimx << " x " << topology[i].dimy;
    space_file << "\n[/topology]"; 

    space_file << endl;
    space_file << "\n[buffer]"; 
    space_file << "\n\% buffer fifo size in flits";
    for (unsigned int i=0;i<buffer.size();i++)
	space_file << "\n" << buffer[i];
    space_file << "\n[/buffer]"; 

    space_file << endl;
    space_file << "\n[size]"; 
    space_file << "\n\% packet size in flits";
    for (unsigned int i=0;i<size.size();i++)
	space_file << "\n" << size[i];
    space_file << "\n[/size]"; 

    space_file << endl;
    space_file << "\n[default]";
    space_file << "\n\% parameters not changed during space exploration";
    //space_file << "\n-seed " << seed << " -warmup " << warmup << " -sim " << sim_length;
    space_file << "\n-sim " << sim_length << " -warmup " << warmup;
    space_file << "\n[/default]";

    space_file << endl;
    space_file << "\n[aggregation]";
    space_file << "\n\% parameters that change value inside each generated file";
    for (unsigned int i=0;i<aggregate.size();i++)
    {
	space_file << "\n" << aggregate[i];
    }
    space_file << "\n[/aggregation]";

    space_file << endl;
    space_file << "\n[explorer]";
    space_file << "\nrepetitions " << repetitions;
    space_file << "\nsimulator " << simulator_path;
    space_file << "\n[/explorer]";

    space_file << "\n\% end of file";
}

