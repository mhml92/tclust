#include <string>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <tclap/CmdLine.h>
#include <boost/mpi.hpp>
#include <boost/algorithm/gather.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/deque.hpp>
#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include "transclust/TransClust.hpp"

/*******************************************************************************
 * Set logging verbosity
 ******************************************************************************/
//const plog::Severity VERBOSITY = plog::none;
//const plog::Severity VERBOSITY = plog::fatal;
//const plog::Severity VERBOSITY = plog::error;
//const plog::Severity VERBOSITY = plog::warning;
const plog::Severity VERBOSITY = plog::info;
//const plog::Severity VERBOSITY = plog::debug;
//const plog::Severity VERBOSITY = plog::verbose;

void parse_args(int argc, char** argv,TCC::TransClustParams& tcp)
{
	try {


		TCLAP::CmdLine cmd("Distributed TransClust", ' ', "1.0");

		TCLAP::ValueArg<unsigned> seed("","seed","(default: 42) Seed for random generation",false,42,"int",cmd);

		TCLAP::ValueArg<unsigned> fpt_max_cc_size(
				"",
				"FPT_max_cc_size",
				"(default: 200) The maximum size of connected components in the FPT.",
				false,
				200,
				"int",
				cmd);

		TCLAP::ValueArg<float> fpt_step_size(
				"",
				"FPT_step_size",
				"(default: 500) ",
				false,
				500.0,
				"float",
				cmd);

		TCLAP::ValueArg<float> fpt_time_limit(
				"",
				"FPT_time_limit",
				"(default: 1) Specifies the maximal runtime for the FPT algorithm in" 
				"seconds",
				false,
				1,
				"float",
				cmd);

		TCLAP::ValueArg<float> f_s(
				"",
				"f_s",
				"(default: 0.01) Stepsize factor for geometric linking",
				false,
				0.01,
				"float",
				cmd);


		TCLAP::ValueArg<float> s_init(
				"",
				"s_init",
				"(default: 0.01) Initial stepsize for geometric linking",
				false,
				0.01,
				"float",
				cmd);

		TCLAP::ValueArg<float> d_maximal(
				"",
				"d_maximal",
				"(default: 5.0) Maximal distance for geometric linking",
				false,
				5.0,
				"float",
				cmd);


		TCLAP::ValueArg<float> d_init(
				"",
				"d_init",
				"(default: 0.01) Initital distance for geometric linking",
				false,
				0.01,
				"float",
				cmd);

		TCLAP::ValueArg<unsigned> FORCE_min_size_parallel(
				"",
				"FORCE_min_size_parallel",
				"(default: 1000)",
				false,
				1000,
				"int",
				cmd);

		TCLAP::ValueArg<float> FORCE_start_t(
				"",
				"FORCE_temperature",
				"(default: 100) Defines the speed of how fast the movements are "
				"cooled down in order to achieve a stable result.",
				false,
				100.0,
				"float",
				cmd);

		TCLAP::ValueArg<float> FORCE_p(
				"",
				"p",
				"(default: 1) Layout initial radius",
				false,
				1,
				"float",
				cmd);

		TCLAP::ValueArg<unsigned> FORCE_dim(
				"",
				"FORCE_dimension",
				"(default: 3) Specifies the number of dimensions used for the "
				"layouter. 2 or 3 dimensions are normally fine values.",
				false,
				3,
				"int",
				cmd);

		TCLAP::ValueArg<unsigned> FORCE_R(
				"",
				"FORCE_iterations",
				"(default: 100) Number of iterations performed for the layouting "
				"process. For large instances it might be beneficial to reduce this "
				"number.",
				false,
				100,
				"int",
				cmd);

		TCLAP::ValueArg<float> FORCE_rep(
				"",
				"FORCE_repulsion",
				"(default: 100) Sets the repulsion parameter for the FORCE heuristic.",
				false,
				100.0,
				"float",
				cmd);
		// layout vars
		TCLAP::ValueArg<float> FORCE_att(
				"",
				"FORCE_attraction",
				"(default: 100) Sets the attraction parameter for the FORCE heuristic.",
				false,
				100.0,
				"float",
				cmd);

		TCLAP::SwitchArg disable_force(
				"",
				"disable_force",
				"disables clustering with the FORCE heuristic",
				cmd,
				false);

		TCLAP::SwitchArg disable_fpt(
				"",
				"disable_fpt",
				"Disables clustering with the exact FPT algorithm",
				cmd,
				false);

		TCLAP::ValueArg<std::string> tmp_dir(
				"",
				"tmp_dir",
				"(default: '/tmp/tclust') Temporary directory for auxiliary files "
				"created during the run of the program",
				false,
				"/tmp/tclust/",
				"string",
				cmd);

		TCLAP::ValueArg<float> fallback_value(
				"",
				"defaultmissing",
				"(default: 0.0) Sets the similarity value of edges not present in "
				"the similarity file",
				false,
				0.0,
				"float",
				cmd);

		TCLAP::ValueArg<double> memory_limit(
				"",
				"memory_limit",
				"(default: 0) Sets the momory limit for each conneced component in MB." 
				"The defualt value of 0 sets the limit to MAX",
				false,
				0,
				"double",
				cmd);

		TCLAP::ValueArg<std::string> outfile(
				"o",
				"output",
				"Path and filename of the ouput file. If not specified will the result be saved to transclust_result.txt",
				false,
				"transclust_result.txt",
				"string",
				cmd);									// parser

		TCLAP::ValueArg<float> threshold(
				"t",                
				"threshold",    
				"Threshold for the clustering.",
				true,               
				0,               
				"float",
				cmd
				);

		TCLAP::ValueArg<std::string> simfile(
				"s",
				"simfile",
				"Path and filename of the input Similarity file.",
				true,
				"",
				"string",
				cmd);

		cmd.parse( argc, argv );

		tcp.set_threshold(threshold.getValue())
			.set_simfile(simfile.getValue())
			.set_outfile(outfile.getValue())
			.set_sim_fallback(fallback_value.getValue())

			.set_p(FORCE_p.getValue())
			.set_f_att(FORCE_att.getValue())
			.set_f_rep(FORCE_rep.getValue())
			.set_R(FORCE_R.getValue())
			.set_dim(FORCE_dim.getValue())
			.set_start_t(FORCE_start_t.getValue())
			.set_force_min_size_parallel(FORCE_min_size_parallel.getValue())

			.set_d_init(d_init.getValue())
			.set_d_maximal(d_maximal.getValue())
			.set_s_init(s_init.getValue())
			.set_f_s(f_s.getValue())

			.set_fpt_time_limit(fpt_time_limit.getValue())
			.set_fpt_step_size(fpt_step_size.getValue())
			.set_fpt_max_cc_size(fpt_max_cc_size.getValue())
			.set_memory_limit(memory_limit.getValue())

			.set_disable_fpt(disable_fpt.getValue())
			.set_disable_force(disable_force.getValue())

			.set_seed(seed.getValue())
			.set_tmp_dir(tmp_dir.getValue());

	}catch (TCLAP::ArgException &e){
		std::cerr 
			<< "error: " << e.error() 
			<< " for arg " << e.argId() 
			<< std::endl;
	}
} // parse args


void finalize_master(
	std::vector<RES::ClusteringResult>& total_result,
	std::deque<std::string>& id2name,
	TCC::TransClustParams& tcp,
	std::chrono::time_point<std::chrono::steady_clock> t1)
{

	RES::ClusteringResult final_result;
	final_result.cost = 0;
	for(int i = 0; i < total_result.size();i++)
	{
		RES::ClusteringResult& r = total_result[i];
		//LOGI << 
		//	"\nprocess: " << i << "\n" <<
		//	"\thas cost: " << r.cost << "\n" << 
		//	"\tnum clusters: " << r.clusters.size();
		final_result.cost += r.cost;
		for(auto& cluster:r.clusters)
		{
			//LOGI << "\ncluster size: " << cluster.size();
			//LOGI << "first value: "<< cluster[0];
			final_result.clusters.push_back(cluster);
		}
	}
	auto t2 = std::chrono::steady_clock::now();
	double _time = (double)std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()/1000000;

	/////////////////////////////////////////////////////////////////////////////
	// format timeduration string (...oh gawd)
	/////////////////////////////////////////////////////////////////////////////
	unsigned hours = _time/3600;

	_time = _time-(hours*3600.0);

	unsigned minutes = _time/60;

	_time = _time - (minutes*60.0);
	unsigned seconds = _time;

	unsigned rest = ((_time - seconds)*100);

	std::stringstream ss;
	if(hours > 0 || minutes > 0){
		ss << std::setw(2) << std::setfill('0') << hours << ":";
		ss << std::setw(2) << std::setfill('0') << minutes << ":"; 
		ss << std::setw(2) << std::setfill('0') << seconds << ".";
		ss << ((unsigned)(rest*1000))/1000.0;
	}else{
		ss <<	((unsigned)(_time*1000))/1000.0 << "s";
	}
	std::string stime = ss.str();
	///////////////////////////////////////////////////////////////////////

	// print stats
	std::string num_clusters = std::to_string(final_result.clusters.size());
	std::stringstream tc;
	tc << std::setprecision(2)<< std::fixed << final_result.cost;
	std::string total_cost = tc.str();

	unsigned width = 1+std::max({stime.length(),num_clusters.length(),total_cost.length()});

	LOGI << "Threshold: " << std::setw(width) << tcp.threshold; 
	LOGI << "Clusters   " << std::setw(width) << num_clusters;
	LOGI << "Cost       " << std::setw(width) << total_cost;
	LOGI << "Time       " << std::setw(width) << stime;


	///////////////////////////////////////////////////////////////////////
	//* Print result (java transclust style)
	///////////////////////////////////////////////////////////////////////
	FILE* ofile = fopen(tcp.outfile.c_str(),"w");
	std::string s = "";
	s += std::to_string(tcp.threshold) + "\t" + std::to_string(final_result.cost) + "\t";
	fputs(s.c_str(),ofile);

	for(auto& cluster:final_result.clusters)
	{
		s = "";
		for(unsigned id:cluster)
		{
			s += id2name.at(id) + ","; 
		}
		s.pop_back();
		s += ";";
		fputs(s.c_str(),ofile);
	}
	fclose(ofile);

}// finalize_master

void init_master(
	boost::mpi::communicator& world,
	std::deque<ConnectedComponent>& ccs,
	TCC::TransClustParams& tcp,
	std::deque<std::string>& id2name)
{
	// read input file and initialize connected components
	InputParser ip(tcp);
	ip.getConnectedComponents(ccs,id2name);

	// sort connectede components by size
	std::sort(ccs.begin(), ccs.end(), 
			[](ConnectedComponent& a,ConnectedComponent& b) { 
			return a.size() < b.size();
			}
			);

	// datastructure for holding the connected components for each process
	std::deque<std::deque<ConnectedComponent>> process_cc(world.size(),std::deque<ConnectedComponent>());

	// process counter
	unsigned process_index = 0;

	// counter for transitive connected components - just for logging
	unsigned transitive_cc = 0;

	// assigne connected components to processes
	for(ConnectedComponent& cc:ccs)
	{
		// We let process 0 handle all transitive connected components
		// (there is no need to send them around)
		if(cc.isTransitive())
		{
			process_cc.at(0).push_back(cc);
			transitive_cc++;
		}else{
			// assign connected components to processes
			process_cc.at(process_index).push_back(cc);

			process_index++;
			process_index = process_index % world.size();
		}
	}
	LOGI << transitive_cc << " Connected Components were already transitive";
	LOGI << "Clustering " << ccs.size()-transitive_cc << " Connected Components";

	// send the connected components to the processes  (except for 0)
	for(unsigned process = 1; process < process_cc.size();process++)
	{
		world.send(process,0,process_cc.at(process));	
	}

	for(unsigned i = 0; i < process_cc.size(); i++)
	{
		LOGI << "Process " << i << " has " << process_cc[i].size() << " Connected Components";
	}
	// assign the connected component to master process
	ccs = process_cc[0];

}// init_master

int main(int argc, char** argv)
{
	/////////////////////////////////////////////////////////////////////////////
	// SETUP
	/////////////////////////////////////////////////////////////////////////////
	//Initialize mpi
	boost::mpi::environment env{argc, argv};
	boost::mpi::communicator world;


	// timeing
	auto t1 = std::chrono::steady_clock::now();
	
	//Initialize logging
	static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
	plog::init(VERBOSITY, &consoleAppender);

	// Parse input arguments
	TCC::TransClustParams tcp;
	parse_args(argc,argv,tcp);

	// initialize commons
	TransClust transclust(tcp);
	RES::ClusteringResult result;
	std::deque<std::string> id2name;
	std::deque<ConnectedComponent> ccs;

	/////////////////////////////////////////////////////////////////////////////
	// INIT PROCESSES
	/////////////////////////////////////////////////////////////////////////////
	if(world.rank() == 0)
	{
		init_master(
				world,
				ccs,
				tcp,
				id2name
				);
	}else{
		// wait and recieve connected components from master thread
		world.recv(0,0,ccs);
	}

	/////////////////////////////////////////////////////////////////////////////
	// CLUSTER CONNECTED COMPONENTS
	/////////////////////////////////////////////////////////////////////////////
	transclust.cluster(ccs,result);

	/////////////////////////////////////////////////////////////////////////////
	// FINALIZE PROCESSES AND PRNIT RESULT
	/////////////////////////////////////////////////////////////////////////////
	if(world.rank() == 0)
	{
		std::vector<RES::ClusteringResult> total_result;
		boost::mpi::gather(world,result,total_result,0);	

		finalize_master(
				total_result,
				id2name,
				tcp,
				t1);
	}else{
		boost::mpi::gather(world,result,0);
	}

	return 0;
}

