#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <cassert>

using namespace std;

//---------------------------------------------------------------------------

typedef vector<string> TParameterSpace;

typedef map<string, TParameterSpace> TParametersSpace;

//---------------------------------------------------------------------------

bool IsComment(const string& s)
{
  return (s == "" || s.at(0) == '%');
}

//---------------------------------------------------------------------------

string TrimLeftAndRight(const string& s)
{
  int len = s.length();
  
  int i, j;
  for (i=0; i<len && s.at(i) == ' '; i++) ;
  for (j=len-1; j>=0 && s.at(j) == ' '; j--) ;
  
  return s.substr(i,j-i+1);
}

//---------------------------------------------------------------------------

bool ExtractParameter(const string& s, string& parameter)
{
  int i = s.find("[");

  if (i != string::npos)
    {
      int j = s.rfind("]");
      
      if (j != string::npos)
	{
	  parameter = s.substr(i+1, j-i-1);
	  return true;
	}
    }

  return false;
}

//---------------------------------------------------------------------------

bool GetNextParameter(ifstream& fin, string& parameter)
{
  bool found = false;

  while (!fin.eof() && !found)
    {
      string s;
      getline(fin, s);

      if (!IsComment(s))
	found = ExtractParameter(s, parameter);
    }

  return found;
}

//---------------------------------------------------------------------------w

string MakeStopParameterTag(const string& parameter)
{
  string sparameter = "[/" + parameter + "]";

  return sparameter;
}

//---------------------------------------------------------------------------

bool ManagePlainParameterSet(ifstream& fin,
			     const string& parameter, 
			     TParametersSpace& params_space,
			     string& error_msg)
{
  string str_stop = MakeStopParameterTag(parameter);
  bool   stop = false;

  while (!fin.eof() && !stop)
    {
      string s;
      getline(fin, s);
      
      if (!IsComment(s))
	{
	  if (s.find(str_stop) != string::npos)
	    stop = true;
	  else
	    params_space[parameter].push_back(TrimLeftAndRight(s));
	}
    }
  
  return true;
}

//---------------------------------------------------------------------------

bool ExpandInterval(const string& sint,
		    TParameterSpace& ps,
		    string& error_msg)
{
  istringstream iss(sint);

  double min, max, step;

  iss >> min;
  iss >> max;
  iss >> step;

  for (double v=min; v<=max; v+=step)
    {
      ostringstream oss;
      oss << v;
      ps.push_back(oss.str());
    }

  return true;
}

//---------------------------------------------------------------------------

bool ManageCompressedParameterSet(ifstream& fin,
				  const string& parameter, 
				  TParametersSpace& params_space,
				  string& error_msg)
{
  string str_stop = MakeStopParameterTag(parameter);
  bool   stop = false;

  while (!fin.eof() && !stop)
    {
      string s;
      getline(fin, s);
      
      if (!IsComment(s))
	{
	  if (s.find(str_stop) != string::npos)
	    stop = true;
	  else	  
	    {
	      if (!ExpandInterval(s, params_space[parameter], error_msg))
		return false;
	    }
	}
    }
  
  return true;
}

//---------------------------------------------------------------------------

bool ManageParameter(ifstream& fin,
		     const string& parameter, 
		     TParametersSpace& params_space,
		     string& error_msg)
{
  bool err;

  if (parameter == "pir" ||
      parameter == "por")
    err = ManageCompressedParameterSet(fin, parameter, params_space, error_msg);
  else
    err = ManagePlainParameterSet(fin, parameter, params_space, error_msg);

  return err;
}

//---------------------------------------------------------------------------

bool ParseConfigurationFile(const string& fname,
			    TParametersSpace& params_space,
			    string& error_msg)
{
  ifstream fin(fname.c_str(), ios::in);

  if (!fin)
    {
      error_msg = "Cannot open " + fname;
      return false;
    }

  while (!fin.eof())
    {
      string parameter;

      if ( GetNextParameter(fin, parameter) )
	{
	  if (!ManageParameter(fin, parameter, params_space,
			       error_msg))
	    return false;
	}
    }

  return true;
}

//---------------------------------------------------------------------------

int main(int argc, char **argv)
{
  if (argc != 2)
    return -1;

  TParametersSpace ps;
  string           error_msg;
  string           fname(argv[1]);

  if (!ParseConfigurationFile(fname, ps, error_msg))
    {
      cout << "Error: " << error_msg << endl;
      return -1;
    }

  for (TParametersSpace::iterator i=ps.begin(); i!=ps.end(); i++)
    {
      cout << i->first << ": ";
      for (int j=0; j<i->second.size(); j++)
	cout << i->second[j] << ", ";
      cout << endl;
    }

  return 0;
}

//---------------------------------------------------------------------------
