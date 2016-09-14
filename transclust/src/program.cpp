#include <string>
#include <time.h>
#include <stdio.h>
#include <iostream>
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
				100,
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

		TCLAP::SwitchArg use_custom_range_Arg(
				"",
				"use_custom_range",
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


		cmd.add(fallback_value_Arg);
		cmd.add(simFilenameArg);
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
		cmd.add(fpt_max_cost_Arg);

		cmd.parse( argc, argv );
		/*************************************************************************
		 * Initialize transclust
		 ************************************************************************/
		TransClust transclust(
			// filename
			simFilenameArg.getValue(),
			// use default fallback
			use_custom_fallback_Arg.getValue(),
			// fallback value
			fallback_value_Arg.getValue(),
			// use default interval
			use_custom_range_Arg.getValue(),
			// threshold min
			threshold_min_Arg.getValue(),
			// threshold max
			threshold_max_Arg.getValue(),
			// threshold step size
			threshold_step_Arg.getValue(),
			// p radius
			p_Arg.getValue(),
			// attraction
			f_att_Arg.getValue(),
			// repulsion
			f_rep_Arg.getValue(),
			// force iterations
			R_Arg.getValue(),
			// dimantion
			dim_Arg.getValue(),
			// start temperature
			start_t_Arg.getValue(),
			// d_init
			d_init_Arg.getValue(),
			// d_maximal
			d_maximal_Arg.getValue(),
			// s_init
			s_init_Arg.getValue(),
			// f_s
			f_s_Arg.getValue(),
			fpt_time_limit_Arg.getValue(),
			fpt_max_cost_Arg.getValue(),
			fpt_step_size_Arg.getValue(),
			disable_force_Arg.getValue(),
			disable_fpt_Arg.getValue()
		);

		/*************************************************************************
		 * Cluster
		 ************************************************************************/
		clustering clusters = transclust.cluster();

		/*************************************************************************
		 * Print result
		 ************************************************************************/
		for(unsigned i = 0; i < clusters.threshold.size(); i++){
			std::cout << 
				clusters.threshold.at(i) << 
				"\t" << 
				clusters.cost.at(i) << 
				"\t";

			for(auto & cluster:clusters.clusters.at(i)){
				std::string c = "";
				for(auto & o:cluster){
					c += std::to_string(o) + ",";
				}
				c.pop_back();
				std::cout << c + ";";
			}
			std::cout << std::endl;
			
		}

	}catch (TCLAP::ArgException &e){
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}
	return 0;
}

