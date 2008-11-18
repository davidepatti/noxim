#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <cstdlib>

using namespace std;

// ---------------------------------------------------------------------------
// Global vars
unsigned int   width;
unsigned int   height;
char           *map_fname;
char           *cg_fname;

// ---------------------------------------------------------------------------

typedef struct 
{
  pair<int,int> comm;
  double        pir;
} TCommunication;

// ---------------------------------------------------------------------------

vector<int> ReadMapping(char *fname)
{  
  ifstream fin;
  fin.open(fname, ifstream::in);

  if (!fin.is_open())
    {
      cerr << "Cannot open " << fname << endl;
      assert(false);
    }

  char line[256]; // first line is not used
  fin.getline(line, sizeof(line)-1);

  vector<int> mapping;
  while (!fin.eof())
    {
      int id;

      fin >> id;

      if (id != -1)
	mapping.push_back(id);
      else
	break;
    }
  assert(mapping.size() == width*height);

  return mapping;
}

// ---------------------------------------------------------------------------

vector<TCommunication> ReadCG(char *fname)
{
  ifstream fin(fname);

  if (!fin.is_open())
    {
      cerr << "Cannot open " << fname << endl;
      assert(false);
    }
  
  char                   line[128];
  int                    sv, tv;
  double                 pir;
  vector<TCommunication> comms;
  while ( !fin.eof() )
  {
    fin.getline(line, sizeof(line)-1);

    if (line[0] != '%')
      {
	pir = -1.0;
	int n = sscanf(line, "%d%d%lf", &sv, &tv, &pir);
	if (n != 2 && n != 3)
	  assert(false);

	TCommunication comm;
	comm.comm.first  = sv;
	comm.comm.second = tv;
	comm.pir         = pir;
	comms.push_back(comm);
      }
  }

  return comms;
}

// ---------------------------------------------------------------------------

vector<TCommunication> RemapCG(vector<TCommunication>& cg,
			       vector<int>& map)
{
  vector<TCommunication> rcg;

  for (unsigned int i=0; i<cg.size(); i++)
    {
      TCommunication comm;

      comm.comm.first  = map[cg[i].comm.first];
      comm.comm.second = map[cg[i].comm.second];
      comm.pir         = cg[i].pir;

      rcg.push_back(comm);
    }

  return rcg;
}

// ---------------------------------------------------------------------------

void ShowCG(vector<TCommunication>& cg)
{
  cout << "% File generated from " << map_fname << ", and " << cg_fname << endl;
  for (unsigned int i=0; i<cg.size(); i++)
    {
      cout << cg[i].comm.first << "\t" << cg[i].comm.second;
      if (cg[i].pir == -1)
	cout << endl;
      else
	cout << "\t" << cg[i].pir << endl;
    }
}

// ---------------------------------------------------------------------------

void ParseCmdLine(int argc, char* argv[])
{
  if (argc != 5)
    {
      cerr << "Usage " << argv[0] << " <width> <height> <map file> <cg file>" << endl;
      assert(false);
    }

  width = atoi(argv[1]);
  height = atoi(argv[2]);
  map_fname = argv[3];
  cg_fname = argv[4];
}

// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  
  ParseCmdLine(argc, argv);

  vector<int> mapping = ReadMapping(map_fname);
  vector<TCommunication> cg = ReadCG(cg_fname);
  vector<TCommunication> rcg = RemapCG(cg, mapping);
  ShowCG(rcg);

  return 0;
}
