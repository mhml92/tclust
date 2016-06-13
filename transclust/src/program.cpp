#include <string>
#include <time.h>
#include <stdio.h>
#include <TransClust.hpp>
#include <iostream>
#include <tclap/CmdLine.h>
#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>

/*******************************************************************************
 * Set logging verbosity
 ******************************************************************************/
const plog::Severity VERBOSITY = plog::verbose;

int main(int argc, char** argv){
	try {
		/*************************************************************************
		 * Parse input arguments
		 ************************************************************************/
		TCLAP::CmdLine cmd("Command description message", ' ', "0.9");

		TCLAP::UnlabeledValueArg<std::string> simFilenameArg(
				"simfile",                 // name
				"Input similarity file",   // desc
				true,                      // required
				"not there",               // default val
				"string"                   // type descripttion
				);

		// layout vars
		TCLAP::ValueArg<float> f_att_Arg(
				"",
				"f_att",
				"Attraction force multiplier",
				false,
				1.0,
				"float");
		TCLAP::ValueArg<float> f_rep_Arg(
				"",
				"f_rep",
				"Repulsion force multiplier",
				false,
				1.0,
				"float");
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
				2,
				"unsigend interger");
		TCLAP::ValueArg<float> p_Arg(
				"",
				"p",
				"Layout initial radius",
				false,
				1,
				"float");

		cmd.add(simFilenameArg);
		cmd.add(f_att_Arg);
		cmd.add(f_rep_Arg);
		cmd.add(R_Arg);
		cmd.add(dim_Arg);
		cmd.add(p_Arg);

		cmd.parse( argc, argv );
		/*************************************************************************
		 * Initialize logging
		 ************************************************************************/
		static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
		plog::init(VERBOSITY, &consoleAppender);
		/*************************************************************************
		 * Initialize transclust
		 ************************************************************************/
		TransClust transclust(
			simFilenameArg.getValue(),
			"",
			0,
			100.0,
			1,
			p_Arg.getValue(),
			f_att_Arg.getValue(),
			f_rep_Arg.getValue(),
			R_Arg.getValue(),
			dim_Arg.getValue()
		);



		/*************************************************************************
		 * Cluster
		 ************************************************************************/
		transclust.cluster();

	}catch (TCLAP::ArgException &e){
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}
	return 0;
}

