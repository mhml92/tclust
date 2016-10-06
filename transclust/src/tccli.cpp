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
		TCLAP::CmdLine cmd("cppTransClust", ' ', "1.0");

		TCLAP::UnlabeledValueArg<std::string> simFilenameArg(
				"simfile",                 // name
				"Input similarity file",   // desc
				true,                      // required
				"not there",               // default val
				"string"                   // type descripttion
				);

		TCLAP::ValueArg<std::string> simFileType_Arg(
				"",
				"simfile_type",
				"",
				false,
				"SIMPLE",
				"'SIMPLE','LEGACY' (default: 'SIMPLE')");

		TCLAP::ValueArg<std::string> tmp_dir_Arg(
				"",
				"tmp_dir",
				"",
				false,
				"/tmp/tclust/",
				"Temporary directory for external merge sort (default: '/tmp/tclust/')");


		// layout vars
		TCLAP::ValueArg<double> f_att_Arg(
				"",
				"f_att",
				"Attraction force multiplier",
				false,
				100.0,
				"double");
		TCLAP::ValueArg<double> f_rep_Arg(
				"",
				"f_rep",
				"Repulsion force multiplier",
				false,
				100.0,
				"double");
		TCLAP::ValueArg<unsigned> R_Arg(
				"",
				"R",
				"Layout iterations",
				false,
				200,
				"unsigned integer");
		TCLAP::ValueArg<unsigned> dim_Arg(
				"",
				"dim",
				"Layout dimensions",
				false,
				3,
				"unsigend interger");
		TCLAP::ValueArg<double> p_Arg(
				"",
				"p",
				"Layout initial radius",
				false,
				1,
				"double");

		TCLAP::ValueArg<double> start_t_Arg(
				"",
				"start_t",
				"Start temperature for force",
				false,
				100,
				"double");

		TCLAP::ValueArg<double> d_init_Arg(
				"",
				"d_init",
				"Initital distance for geometric linking",
				false,
				0.01,
				"double");
		
		TCLAP::ValueArg<double> d_maximal_Arg(
				"",
				"d_maximal",
				"Maximal distance for geometric linking",
				false,
				5.0,
				"double");

		TCLAP::ValueArg<double> s_init_Arg(
				"",
				"s_init",
				"Initial stepsize for geometric linking",
				false,
				0.01,
				"double");

		TCLAP::ValueArg<double> f_s_Arg(
				"",
				"f_s",
				"Stepsize factor for geometric linking",
				false,
				0.01,
				"double");

		TCLAP::ValueArg<double> fallback_value_Arg(
				"",
				"fallback_value",
				"",
				false,
				0.00,
				"double");

		TCLAP::ValueArg<double> threshold_min_Arg(
				"",
				"threshold_min",
				"",
				false,
				0.0,
				"double");

		TCLAP::ValueArg<double> threshold_max_Arg(
				"",
				"threshold_max",
				"",
				false,
				100.0,
				"double");

		TCLAP::ValueArg<double> threshold_step_Arg(
				"",
				"threshold_step",
				"",
				false,
				1.0,
				"double");

		TCLAP::ValueArg<double> fpt_time_limit_Arg(
				"",
				"fpt_time_limit",
				"",
				false,
				60.0,
				"double");

		TCLAP::ValueArg<double> fpt_max_cost_Arg(
				"",
				"fpt_max_cost",
				"",
				false,
				5000.0,
				"double");

		TCLAP::ValueArg<double> fpt_step_size_Arg(
				"",
				"fpt_step_size",
				"",
				false,
				10.0,
				"double");

		TCLAP::ValueArg<unsigned> seed_Arg(
				"",
				"seed",
				"",
				false,
				42,
				"unsigned int");

		TCLAP::SwitchArg use_default_interval_Arg(
				"",
				"use_default_interval",
				"If false; set threshold vars",
				cmd,
				false);

		TCLAP::SwitchArg use_custom_fallback_Arg(
				"",
				"use_custom_fallback",
				"",
				cmd,
				false);

		TCLAP::SwitchArg disable_fpt_Arg(
				"",
				"disable_fpt",
				"",
				cmd,
				false);

		TCLAP::SwitchArg disable_force_Arg(
				"",
				"disable_force",
				"",
				cmd,
				false);

		TCLAP::SwitchArg external_Arg(
				"",
				"external",
				"",
				cmd,
				false);
		

		cmd.add(fallback_value_Arg);
		cmd.add(simFileType_Arg);
		cmd.add(simFilenameArg);
		cmd.add(tmp_dir_Arg);
		cmd.add(threshold_min_Arg);
		cmd.add(threshold_max_Arg);
		cmd.add(threshold_step_Arg);
		cmd.add(f_att_Arg);
		cmd.add(f_rep_Arg);
		cmd.add(R_Arg);
		cmd.add(dim_Arg);
		cmd.add(p_Arg);
		cmd.add(start_t_Arg);
		cmd.add(d_init_Arg);
		cmd.add(d_maximal_Arg);
		cmd.add(s_init_Arg);
		cmd.add(f_s_Arg);
		cmd.add(fpt_time_limit_Arg);
		cmd.add(fpt_step_size_Arg);
		cmd.add(fpt_max_cost_Arg);
		cmd.add(seed_Arg);

		cmd.parse( argc, argv );
		/*************************************************************************
		 * Initialize transclust
		 ************************************************************************/

		TCC::TransClustParams tcp;

		TransClust transclust(
			// filename
			simFilenameArg.getValue(),
			tcp.set_use_custom_fallback(use_custom_fallback_Arg.getValue())
				.set_sim_fallback(fallback_value_Arg.getValue())
				.set_file_type(simFileType_Arg.getValue())
				.set_use_default_interval(use_default_interval_Arg.getValue())
				.set_th_min(threshold_min_Arg.getValue())
				.set_th_max(threshold_max_Arg.getValue())
				.set_th_step(threshold_step_Arg.getValue())
				.set_p(p_Arg.getValue())
				.set_f_att(f_att_Arg.getValue())
				.set_f_rep(f_rep_Arg.getValue())
				.set_R(R_Arg.getValue())
				.set_dim(dim_Arg.getValue())
				.set_start_t(start_t_Arg.getValue())
				.set_d_init(d_init_Arg.getValue())
				.set_d_maximal(d_maximal_Arg.getValue())
				.set_s_init(s_init_Arg.getValue())
				.set_f_s(f_s_Arg.getValue())
				.set_fpt_time_limit(fpt_time_limit_Arg.getValue())
				.set_fpt_max_cost(fpt_max_cost_Arg.getValue())
				.set_fpt_step_size(fpt_step_size_Arg.getValue())
				.set_disable_force(disable_force_Arg.getValue())
				.set_disable_fpt(disable_fpt_Arg.getValue())
				.set_seed(seed_Arg.getValue())
				.set_tmp_dir(tmp_dir_Arg.getValue())
				.set_external(external_Arg.getValue())
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

