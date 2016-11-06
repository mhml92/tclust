#include <string>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <tclap/CmdLine.h>
#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include "transclust/TransClust.hpp"

/*******************************************************************************
 * Set logging verbosity
 ******************************************************************************/
const plog::Severity VERBOSITY = plog::none;
//const plog::Severity VERBOSITY = plog::fatal;
//const plog::Severity VERBOSITY = plog::error;
//const plog::Severity VERBOSITY = plog::warning;
//const plog::Severity VERBOSITY = plog::info;
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


		TCLAP::ValueArg<unsigned> seed(
				"",
				"seed",
				"",
				false,
				42,
				"int (42)",
				cmd);

		TCLAP::ValueArg<unsigned> fpt_max_edge_conflicts(
				"",
				"fpt_max_edge_conflicts",
				"",
				false,
				5000,
				"int (5000)",
				cmd);

		TCLAP::ValueArg<unsigned> fpt_max_cc_size(
				"",
				"fpt_max_cc_size",
				"",
				false,
				200,
				"int (200)",
				cmd);

		TCLAP::ValueArg<float> fpt_step_size(
				"",
				"fpt_step_size",
				"",
				false,
				500.0,
				"float (500.0)",
				cmd);

		TCLAP::ValueArg<float> fpt_time_limit(
				"",
				"FPTConfig.MAX_TIME",
				"Specifies the maximal runtime for the FPT algorithm in seconds",
				false,
				1,
				"float (1)",
				cmd);

		TCLAP::ValueArg<float> f_s(
				"",
				"f_s",
				"Stepsize factor for geometric linking",
				false,
				0.01,
				"float (0.01)",
				cmd);


		TCLAP::ValueArg<float> s_init(
				"",
				"s_init",
				"Initial stepsize for geometric linking",
				false,
				0.01,
				"float (0.01)",
				cmd);

		TCLAP::ValueArg<float> d_maximal(
				"",
				"d_maximal",
				"Maximal distance for geometric linking",
				false,
				5.0,
				"float (5.0)",
				cmd);


		TCLAP::ValueArg<float> d_init(
				"",
				"d_init",
				"Initital distance for geometric linking",
				false,
				0.01,
				"float (0.01)",
				cmd);

		TCLAP::ValueArg<float> FORCE_start_t(
				"",
				"FORCEConfig.TEMPERATURE",
				"Defines the speed of how fast the movements are cooled down in order to achieve a stable result.",
				false,
				100.0,
				"float (100.0)",
				cmd);

		TCLAP::ValueArg<float> FORCE_p(
				"",
				"p",
				"Layout initial radius",
				false,
				1,
				"float (1)",
				cmd);

		TCLAP::ValueArg<unsigned> FORCE_dim(
				"",
				"FORCEConfig.DIMENSION",
				"Specifies the number of dimensions used for the layouter. 2 or 3 dimensions are normally fine values.",
				false,
				3,
				"int (3)",
				cmd);

		TCLAP::ValueArg<unsigned> FORCE_R(
				"",
				"FORCEConfig.ITERATIONS",
				"Number of iterations performed for the layouting process. For large instances it might be beneficial to reduce this number.",
				false,
				100,
				"int (100)",
				cmd);
		
		TCLAP::ValueArg<float> FORCE_rep(
				"",
				"FORCEConfig.REPULSION_FACTOR",
				"Controls how strongly the repulsion of nodes determines the layouting.",
				false,
				100.0,
				"float (100.0)",
				cmd);
		// layout vars
		TCLAP::ValueArg<float> FORCE_att(
				"",
				"FORCEConfig.ATTRACTION_FACTOR",
				"Temporary directory for external merge sort (default: '/tmp/tclust/')",
				false,
				100.0,
				"float (100.0)",
				cmd);

		TCLAP::ValueArg<std::string> normalization(
				"n",
				"normalization",
				"How normalization of cost values should be done. 'ABSOLUTE' normalizes by larges absolute value in a connected component, and 'RELATIVE' normalizes by larges positive value in a connected component.",
				false,
				"ABSOLUTE",
				"string (ABSOLUTE)",
				cmd);

		TCLAP::SwitchArg disable_force(
				"",
				"disable_force",
				"",
				cmd,
				false);

		TCLAP::SwitchArg disable_fpt(
				"",
				"disable_fpt",
				"",
				cmd,
				false);

		TCLAP::ValueArg<std::string> tmp_dir(
				"",
				"tmp_dir",
				"Temporary directory for auxiliary files created during the run of the program",
				false,
				"/tmp/tclust/",
				"string (/tmp/tclust/)",
				cmd);

		TCLAP::ValueArg<float> fallback_value(
				"",
				"defaultmissing",
				"Sets the default similarity value to the given double value",
				false,
				0.0,
				"float (0.0)",
				cmd);

		TCLAP::ValueArg<float> threshold(
				"t",                
				"threshold",    
				"Threshold for the clustering or costmatrix creation.",
				true,               
				0,               
				"float",
				cmd
				);

		TCLAP::ValueArg<std::string> simfile(
				"s",									// flag
				"simfile",							// name
				"Path and filename of the input Similarity file.",		// desc
				true,									// req
				"",									// value
				"string",							// typeDesc
				cmd);									// parser

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
				.set_normalization(normalization.getValue())

				.set_p(FORCE_p.getValue())
				.set_f_att(FORCE_att.getValue())
				.set_f_rep(FORCE_rep.getValue())
				.set_R(FORCE_R.getValue())
				.set_dim(FORCE_dim.getValue())
				.set_start_t(FORCE_start_t.getValue())

				.set_d_init(d_init.getValue())
				.set_d_maximal(d_maximal.getValue())
				.set_s_init(s_init.getValue())
				.set_f_s(f_s.getValue())

				.set_fpt_time_limit(fpt_time_limit.getValue())
				.set_fpt_step_size(fpt_step_size.getValue())
		 		.set_fpt_max_edge_conflicts(fpt_max_edge_conflicts.getValue())
				.set_fpt_max_cc_size(fpt_max_cc_size.getValue())
				
				.set_disable_fpt(disable_fpt.getValue())
				.set_disable_force(disable_force.getValue())

				.set_seed(seed.getValue())
				.set_tmp_dir(tmp_dir.getValue())
				);

		/*************************************************************************
		 * Cluster
		 ************************************************************************/
		clustering clusters = transclust.cluster();

		/*************************************************************************
		 * Print result (java transclust style)
		 ************************************************************************/
		for(unsigned i = 0; i < clusters.threshold.size(); i++){
			std::cout << 
				clusters.threshold.at(i) << 
				"\t" << 
				clusters.cost.at(i) << 
				"\t";

			// membership vector
			std::vector<unsigned>& m_vec = clusters.clusters.at(i);

			std::list<unsigned> objects;
			// fill list of indexes
			for (unsigned i = 0; i < m_vec.size();i++)
			{
				objects.push_back(i);
			}

			std::string s = "";
			while(!objects.empty())
			{
				unsigned c_num = m_vec.at(*objects.begin());
				for (auto it = objects.begin(); it != objects.end();)
				{
					if(m_vec.at(*it) == c_num)
					{
						s += clusters.id2object.at(*it); 
						s += ",";
						it = objects.erase(it);

					}else{
						++it;
					}
				}

				s.pop_back();
				s += ";";
			}
			std::cout << s << std::endl;
		}

	}catch (TCLAP::ArgException &e){
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}
	return 0;
}

