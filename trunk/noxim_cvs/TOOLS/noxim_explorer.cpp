#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <cassert>

using namespace std;

//---------------------------------------------------------------------------

#define DEFAULT_KEY         "default"
#define AGGREGATION_KEY     "aggregation"
#define EXPLORER_KEY        "explorer"
#define SIMULATOR_LABEL     "simulator"
#define REPETITIONS_LABEL   "repetitions"

#define DEF_SIMULATOR       "./noxim"
#define DEF_REPETITIONS     5

#define TMP_FILE_NAME       ".noxim_explorer.tmp"

#define RPACKETS_LABEL      "% Total received packets:"
#define RFLITS_LABEL        "% Total received flits:"
#define DELAY_LABEL         "% Global average delay (cycles):"
#define THROUGHPUT_LABEL    "% Global average throughput (flits/cycle):"

#define MATLAB_VAR_NAME     "data"
#define MATRIX_COLUMN_WIDTH 15

//---------------------------------------------------------------------------

typedef unsigned int uint;

// parameter values
typedef vector<string> TParameterSpace;

// parameter name, parameter space
typedef map<string, TParameterSpace> TParametersSpace;

// parameter name, parameter value
typedef vector<pair<string, string> > TConfiguration;

typedef vector<TConfiguration> TConfigurationSpace;

struct TExplorerParams
{
  string simulator;
  int    repetitions;
};

struct TSimulationResults
{
  double       delay;
  double       throughput;
  unsigned int rpackets;
  unsigned int rflits;
};

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
  uint i = s.find("[");

  if (i != string::npos)
    {
      uint j = s.rfind("]");
      
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

bool LastCombination(const vector<pair<int,int> >& indexes)
{
  for (uint i=0; i<indexes.size(); i++)
    if (indexes[i].first < indexes[i].second-1)
      return false;

  return true;
}

//---------------------------------------------------------------------------

bool IncrementCombinatorialIndexes(vector<pair<int,int> >& indexes)
{
  for (uint i=0; i<indexes.size(); i++)
    {
      if (indexes[i].first < indexes[i].second - 1)
	{
	  indexes[i].first++;
	  return true;
	}
      indexes[i].first = 0;	
    }

  return false;
}

//---------------------------------------------------------------------------

TConfigurationSpace Explore(const TParametersSpace& params_space)

{
  TConfigurationSpace conf_space;
  
  vector<pair<int,int> > indexes; // <index, max_index>
  
  for (TParametersSpace::const_iterator psi=params_space.begin();
       psi!=params_space.end(); psi++)
      indexes.push_back(pair<int,int>(0, psi->second.size()));
  
  do 
    {
      int i = 0;
      TConfiguration conf;
      for (TParametersSpace::const_iterator psi=params_space.begin();
	   psi!=params_space.end(); psi++)
	{
	  conf.push_back( pair<string,string>(psi->first, 
					      psi->second[indexes[i].first]));	  
	  i++;
	}
      conf_space.push_back(conf);
    } 
  while (IncrementCombinatorialIndexes(indexes));

  return conf_space;
}

//---------------------------------------------------------------------------

bool RemoveParameter(TParametersSpace& params_space, 
		     const string& param_name,
		     TParameterSpace& param_space,
		     string& error_msg)
{
  TParametersSpace::iterator i = params_space.find(param_name);

  if (i == params_space.end())
    {
      error_msg = "Cannot extract parameter '" + param_name + "'";
      return false;
    }

  param_space = params_space[param_name];
  params_space.erase(i);
  
  return true;
}

//---------------------------------------------------------------------------

bool RemoveAggregateParameters(TParametersSpace& params_space, 
			       TParameterSpace&  aggregated_params,
			       TParametersSpace& aggragated_params_space,
			       string& error_msg)
{
  for (uint i=0; i<aggregated_params.size(); i++)
    {
      string param_name = aggregated_params[i];
      TParameterSpace param_space;
      if (!RemoveParameter(params_space, param_name, param_space, error_msg))
	return false;

      aggragated_params_space[param_name] = param_space;
    }

  return true;
}

//---------------------------------------------------------------------------

string ParamValue2Cmd(const pair<string,string>& pv)
{
  string cmd;

  if (pv.first == "topology")
    {
      istringstream iss(pv.second);

      int  width, height;
      char times;
      iss >> width >> times >> height;

      ostringstream oss;
      oss << "-dimx " << width << " -dimy " << height;

      cmd = oss.str();
    }
  else
    cmd = "-" + pv.first + " " + pv.second;

  return cmd;
}

//---------------------------------------------------------------------------

string Configuration2CmdLine(const TConfiguration& conf)
{
  string cl;

  for (uint i=0; i<conf.size(); i++)
    cl = cl + ParamValue2Cmd(conf[i]) + " ";
  
  return cl;
}

//---------------------------------------------------------------------------

string Configuration2FileName(const TConfiguration& conf)
{
  string fn;

  for (uint i=0; i<conf.size(); i++)
    fn = fn + conf[i].first + "_" + conf[i].second + ".";
  fn = fn + "m";
  
  // Replace " " with "_"
  int len = fn.length();
  for (int i=0; i<len; i++)
    if (fn.at(i) == ' ')
      fn[i] = '_';
  
  return fn;
}

//---------------------------------------------------------------------------

bool ExtractExplorerParams(const TParameterSpace& explorer_params,
			   TExplorerParams& eparams,
			   string& error_msg)
{
  eparams.simulator   = DEF_SIMULATOR;
  eparams.repetitions = DEF_REPETITIONS;

  for (uint i=0; i<explorer_params.size(); i++)
    {
      istringstream iss(explorer_params[i]);

      string label;
      iss >> label;
      
      if (label == SIMULATOR_LABEL)
	iss >> eparams.simulator;
      else if (label == REPETITIONS_LABEL)
	iss >> eparams.repetitions;
      else
	{
	  error_msg = "Invalid explorer option '" + label + "'";
	  return false;
	}
    }

  return true;
}

//---------------------------------------------------------------------------

bool PrintHeader(const string& fname,
		 const TExplorerParams& eparams,
		 const string& def_cmd_line, const string& conf_cmd_line, 
		 ofstream& fout, 
		 string& error_msg)
{
  fout.open(fname.c_str(), ios::out);
  if (!fout)
    {
      error_msg = "Cannot create " + fname;
      return false;
    }

  fout << "% fname: " << fname << endl
       << "% " << eparams.simulator << " "
       << conf_cmd_line << " " << def_cmd_line
       << endl << endl;

  return true;
}

//---------------------------------------------------------------------------

bool ReadResults(const string& fname, 
		 TSimulationResults& sres, 
		 string& error_msg)
{
  ifstream fin(fname.c_str(), ios::in);
  if (!fin)
    {
      error_msg = "Cannot read " + fname;
      return false;
    }

  int nread = 0;
  while (!fin.eof())
    {
      string line;
      getline(fin, line);

      uint pos;
      
      pos = line.find(RPACKETS_LABEL);
      if (pos != string::npos) 
	{
	  nread++;
	  istringstream iss(line.substr(pos + string(RPACKETS_LABEL).size()));
	  iss >> sres.rpackets;
	  continue;
	}
      
      pos = line.find(RFLITS_LABEL);
      if (pos != string::npos) 
	{
	  nread++;
	  istringstream iss(line.substr(pos + string(RFLITS_LABEL).size()));
	  iss >> sres.rflits;
	  continue;
	}

      pos = line.find(DELAY_LABEL);
      if (pos != string::npos) 
	{
	  nread++;
	  istringstream iss(line.substr(pos + string(DELAY_LABEL).size()));
	  iss >> sres.delay;
	  continue;
	}

      pos = line.find(THROUGHPUT_LABEL);
      if (pos != string::npos) 
	{
	  nread++;
	  istringstream iss(line.substr(pos + string(THROUGHPUT_LABEL).size()));
	  iss >> sres.throughput;
	  continue;
	}
    }

  if (nread != 4)
    {
      error_msg = "Output file " + fname + " corrupted";
      return false;
    }

  return true;
}

//---------------------------------------------------------------------------

bool RunSimulation(const string& cmd_base, 
		   TSimulationResults& sres, 
		   string& error_msg)
{
  string cmd = cmd_base + " >& " + TMP_FILE_NAME;
  cout << cmd << endl;
  system(cmd.c_str());
  if (!ReadResults(TMP_FILE_NAME, sres, error_msg))
    return false;

  string rm_cmd = string("rm -f ") + TMP_FILE_NAME;
  system(cmd.c_str());

  return true;
}

//---------------------------------------------------------------------------

bool RunSimulations(pair<uint,uint>& sim_counter,
		    const string& cmd, const int repetitions,
		    const TConfiguration& aggr_conf, 
		    ofstream& fout, 
		    string& error_msg)
{
  for (int i=0; i<repetitions; i++)
    {
      cout << "# simulation " << (++sim_counter.first) << " of " << sim_counter.second << endl;

      TSimulationResults sres;
      if (!RunSimulation(cmd, sres, error_msg))
	return false;

      // Print aggragated parameters
      fout << "  ";
      for (uint i=0; i<aggr_conf.size(); i++)
	fout << setw(MATRIX_COLUMN_WIDTH) << aggr_conf[i].second;

      // Print results;
      fout << setw(MATRIX_COLUMN_WIDTH) << sres.delay
	   << setw(MATRIX_COLUMN_WIDTH) << sres.throughput
	   << setw(MATRIX_COLUMN_WIDTH) << sres.rpackets
	   << setw(MATRIX_COLUMN_WIDTH) << sres.rflits 
	   << endl;
    }

  return true;
}

//---------------------------------------------------------------------------

bool PrintMatlabVariableBegin(const TParametersSpace& aggragated_params_space, 
			      ofstream& fout, string& error_msg)
{
  fout << MATLAB_VAR_NAME << " = [" << endl;
  fout << "% ";
  for (TParametersSpace::const_iterator i=aggragated_params_space.begin();
       i!=aggragated_params_space.end(); i++)
    fout << setw(MATRIX_COLUMN_WIDTH) << i->first;

  fout << setw(MATRIX_COLUMN_WIDTH) << "delay"
       << setw(MATRIX_COLUMN_WIDTH) << "throughput"
       << setw(MATRIX_COLUMN_WIDTH) << "rpackets"
       << setw(MATRIX_COLUMN_WIDTH) << "rflits";

  fout << endl;

  return true;
}

//---------------------------------------------------------------------------

bool GenMatlabCode(const string& var_name,
		   const int repetitions, const int column,
		   ofstream& fout, string& error_msg)
{
  fout << var_name << " = [];" << endl
       << "for i = 1:rows/" << repetitions << "," << endl
       << "   ifirst = (i - 1) * " << repetitions << " + 1;" << endl
       << "   ilast  = ifirst + " << repetitions << " - 1;" << endl
       << "   tmp = " << MATLAB_VAR_NAME << "(ifirst:ilast, cols-4+" << column << ");" << endl
       << "   avg = mean(tmp);" << endl
       << "   [h sig ci] = ttest(tmp, 0.1);" << endl
       << "   ci = (ci(2)-ci(1))/2;" << endl
       << "   " << var_name << " = [" << var_name << "; " << MATLAB_VAR_NAME << "(ifirst, 1:cols-4), avg ci];" << endl
       << "end" << endl
       << endl;

  return true;
}

//---------------------------------------------------------------------------

bool PrintMatlabVariableEnd(const int repetitions,
			    ofstream& fout, string& error_msg)
{
  fout << "];" << endl << endl;

  fout << "rows = size(data, 1);" << endl
       << "cols = size(data, 2);" << endl
       << endl;

  if (!GenMatlabCode(string(MATLAB_VAR_NAME) + "_delay",
		     repetitions, 1, fout, error_msg))
    return false;

  if (!GenMatlabCode(string(MATLAB_VAR_NAME) + "_throughput",
		     repetitions, 2, fout, error_msg))
    return false;
  
  return true;
}

//---------------------------------------------------------------------------

bool RunSimulations(const TConfigurationSpace& conf_space,
		    const TParameterSpace&     default_params,
		    const TParametersSpace&    aggragated_params_space,
		    const TParameterSpace&     explorer_params,
		    string&                    error_msg)
{
  TExplorerParams eparams;
  if (!ExtractExplorerParams(explorer_params, eparams, error_msg))
    return false;

  // Make dafault parameters string
  string def_cmd_line;
  for (uint i=0; i<default_params.size(); i++)
    def_cmd_line = def_cmd_line + default_params[i] + " ";

  // Explore configuration space
  TConfigurationSpace aggr_conf_space = Explore(aggragated_params_space);

  pair<uint,uint> sim_counter(0, conf_space.size() * aggr_conf_space.size() * eparams.repetitions);
  
  for (uint i=0; i<conf_space.size(); i++)
    {
      string conf_cmd_line = Configuration2CmdLine(conf_space[i]);

      string   fname = Configuration2FileName(conf_space[i]);
      ofstream fout;
      if (!PrintHeader(fname, eparams, 
		       def_cmd_line, conf_cmd_line, fout, error_msg))
	return false;

      if (!PrintMatlabVariableBegin(aggragated_params_space, fout, error_msg))
	return false;

      for (uint j=0; j<aggr_conf_space.size(); j++)
	{
	  string aggr_cmd_line = Configuration2CmdLine(aggr_conf_space[j]);

	  string cmd = eparams.simulator + " "
	    + def_cmd_line + " "
	    + conf_cmd_line + " "
	    + aggr_cmd_line;
	  
	  if (!RunSimulations(sim_counter, cmd, eparams.repetitions,
			      aggr_conf_space[j], fout, error_msg))
	    return false;
	}

      if (!PrintMatlabVariableEnd(eparams.repetitions, fout, error_msg))
	return false;
    }

  return true;
}

//---------------------------------------------------------------------------

bool RunSimulations(const string& script_fname,
		    string&       error_msg)
{
  TParametersSpace ps;

  if (!ParseConfigurationFile(script_fname, ps, error_msg))
    return false;


  TParameterSpace default_params;
  if (!RemoveParameter(ps, DEFAULT_KEY, default_params, error_msg))
    cout << "Warning: " << error_msg << endl;
  

  TParameterSpace  aggregated_params;
  TParametersSpace aggragated_params_space;
  if (!RemoveParameter(ps, AGGREGATION_KEY, aggregated_params, error_msg))
    cout << "Warning: " << error_msg << endl;
  else
    if (!RemoveAggregateParameters(ps, aggregated_params, 
				  aggragated_params_space, error_msg))
      return false;

  TParameterSpace explorer_params;
  if (!RemoveParameter(ps, EXPLORER_KEY, explorer_params, error_msg))
    cout << "Warning: " << error_msg << endl;

  TConfigurationSpace conf_space = Explore(ps);

  if (!RunSimulations(conf_space, default_params, 
		      aggragated_params_space, explorer_params, error_msg))
    return false;


  return true;
}


//---------------------------------------------------------------------------

int main(int argc, char **argv)
{
  if (argc != 2)
    return -1;

  string fname(argv[1]);
  string error_msg;

  if (!RunSimulations(fname, error_msg))
    cout << "Error: " << error_msg << endl;

  return 0;
}

//---------------------------------------------------------------------------
