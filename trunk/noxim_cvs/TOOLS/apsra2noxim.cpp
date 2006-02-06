#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <cassert>
#include <string.h>

using namespace std;

//---------------------------------------------------------------------------

bool ParseCommandLine(int argc, char **argv,
		      char **apsra_fname,
		      char *cg_fname,
		      char *rt_fname)
{
  if (argc != 2)
    return false;

  *apsra_fname = argv[1];
  sprintf(cg_fname, "%s.cg", *apsra_fname);
  sprintf(rt_fname, "%s.rt", *apsra_fname);

  return true;
}

//---------------------------------------------------------------------------

bool seek(const char* fname, const char* rt_label, ifstream& fin)
{
  fin.open(fname, ifstream::in);

  if (!fin.is_open())
    {
      cerr << "Cannot open " << fname << endl;
      assert(false);
    }

  bool found = false;
  while (!fin.eof() && !found)
    {
      char line[256];
      fin.getline(line, sizeof(line)-1);

      if (strstr(line, rt_label) != NULL)
	found = true;
    }
  
  return found;
}

//---------------------------------------------------------------------------

bool ExtractRoutingTables(char* apsra_fname, char* rt_fname)
{
  ofstream fout(rt_fname, ios::out);
  if (!fout)
    {
      cerr << "Cannot write " << rt_fname << endl;
      assert(false);
    }

  ifstream fin;
  if (!seek(apsra_fname, "rtsize", fin))
    {
      cerr << "Invalid file format (cannot locate label 'rtsize_1')" << endl;
      assert(false);
    }

  fout << "% Routing tables extracted from " << apsra_fname << endl;
  fout << "% Node        In Dest Outs" << endl;

  bool stop = false;
  while (!fin.eof() && !stop)
    {
      char line[128];
      fin.getline(line, sizeof(line)-1);

      if (line[0] == '\0')
	stop = true;
      else
	{
	  int node_id, in_src, in_dst, dst_id;
	  
	  if (sscanf(line+1, "%d %d->%d %d", &node_id, &in_src, &in_dst, &dst_id) == 4)
	    fout << line+1 << endl;
	}
    }
  
  return true;
}

//---------------------------------------------------------------------------

bool ExtractCommunicationGraph(char* apsra_fname, char* cg_fname)
{
  ofstream fout(cg_fname, ios::out);
  if (!fout)
    {
      cerr << "Cannot write " << cg_fname << endl;
      assert(false);
    }

  ifstream fin;
  if (!seek(apsra_fname, "Communication Graph", fin))
    {
      cerr << "Error: Invalid file format (cannot locate label 'Communication Graph')" << endl;
      return false;
    }

  fout << "% Communication graph extracted from " << apsra_fname << endl;
  fout << "% source  target" << endl;

  bool stop = false;
  while (!fin.eof() && !stop)
    {
      char line[128];
      fin.getline(line, sizeof(line)-1);

      if (line[0] == '\0')
	stop = true;
      else
	{
	  int src_id, dst_id;
	  
	  if (sscanf(line+1, "%d --> %d", &src_id, &dst_id) == 2)
	    {
	      fout << setw(8) << src_id
		   << setw(8) << dst_id << endl;
	    }
	}
    }
  
  return true;
}

//---------------------------------------------------------------------------

int main(int argc, char **argv)
{
  char *apsra_fname;
  char cg_fname[256], rt_fname[256];

  if (!ParseCommandLine(argc, argv, &apsra_fname, cg_fname, rt_fname))
    {
      cout << "Use " << argv[0] << " <apsra fname>" << endl;
      return 1;      
    }

  if (!ExtractCommunicationGraph(apsra_fname, cg_fname))
    cerr << "Warning: Cannot extract communication graph" << endl;


  if (!ExtractRoutingTables(apsra_fname, rt_fname))
    cerr << "Warning: Cannot extract routing tables" << endl;
  
  return 0;
}
