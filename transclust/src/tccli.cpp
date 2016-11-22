#include <string>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <tclap/CmdLine.h>
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

int main(int argc, char** argv){
	/****************************************************************************
	 * Initialize logging
	 ***************************************************************************/
	static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
	plog::init(VERBOSITY, &consoleAppender);

	/****************************************************************************
	 * Parse input arguments
	 ***************************************************************************/
	try {
		TCLAP::CmdLine cmd("Distributed TransClust", ' ', "1.0");

		TCLAP::ValueArg<unsigned> seed("","seed","(default: 42) Seed for random generation",false,42,"int",cmd);

	//	TCLAP::ValueArg<unsigned> fpt_max_edge_conflicts( 
	//			"",
	//			"()FPT_max_edge_conflicts", 
	//			"", 
	//			false, 
	//			5000, 
	//			"int (5000)", 
	//			cmd);

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
				10000,
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

		//TCLAP::ValueArg<std::string> normalization(
		//		"n",
		//		"normalization",
		//		"How normalization of cost values should be done. 'ABSOLUTE' normalizes by larges absolute value in a connected component, and 'RELATIVE' normalizes by larges positive value in a connected component.",
		//		false,
		//		"ABSOLUTE",
		//		"string (ABSOLUTE)",
		//		cmd);

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
		/*************************************************************************
		 * Initialize transclust
		 ************************************************************************/

		TCC::TransClustParams tcp;

		TransClust transclust(
				// filename
				simfile.getValue(),
				tcp.set_threshold(threshold.getValue())
				.set_sim_fallback(fallback_value.getValue())
				//.set_normalization(normalization.getValue())

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
		 		//.set_fpt_max_edge_conflicts(fpt_max_edge_conflicts.getValue())
				.set_fpt_max_cc_size(fpt_max_cc_size.getValue())
				.set_memory_limit(memory_limit.getValue())
				
				.set_disable_fpt(disable_fpt.getValue())
				.set_disable_force(disable_force.getValue())

				.set_seed(seed.getValue())
				.set_tmp_dir(tmp_dir.getValue())

				);

		/*************************************************************************
		 * Cluster
		 ************************************************************************/
		auto t1 = std::chrono::steady_clock::now();
		RES::Clustering clusters = transclust.cluster();
		auto t2 = std::chrono::steady_clock::now();
		double _time = (double)std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()/1000000;

		//////////////////////////////////////////////////////////////////////////
		// format timeduration string (...oh gawd)
		//////////////////////////////////////////////////////////////////////////
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
		//////////////////////////////////////////////////////////////////////////

		// print stats
		std::string num_clusters = std::to_string(clusters.clusters.size());
		std::stringstream tc;
		tc << std::setprecision(2)<< std::fixed << clusters.cost;
		std::string total_cost = tc.str();

		unsigned width = 1+std::max({stime.length(),num_clusters.length(),total_cost.length()});

		LOGI << "Threshold: " << std::setw(width) << threshold.getValue(); 
		LOGI << "Clusters   " << std::setw(width) << num_clusters;
		LOGI << "Cost       " << std::setw(width) << total_cost;
		LOGI << "Time       " << std::setw(width) << stime;

		//////////////////////////////////////////////////////////////////////////
		//* Print result (java transclust style)
		//////////////////////////////////////////////////////////////////////////
		FILE* ofile = fopen(outfile.getValue().c_str(),"w");
		std::string s = "";
		s += std::to_string(clusters.threshold) + "\t" + std::to_string(clusters.cost) + "\t";
		fputs(s.c_str(),ofile);

		for(auto cluster:clusters.clusters)
		{
			s = "";
			for(unsigned id:cluster)
			{
				s += clusters.id2name.at(id) + ","; 
			}
			s.pop_back();
			s += ";";
			fputs(s.c_str(),ofile);
		}
		fclose(ofile);

	}catch (TCLAP::ArgException &e){
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}
	return 0;
}

