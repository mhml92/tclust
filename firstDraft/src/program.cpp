#include <TransClust.hpp>
#include <iostream>
#include <tclap/CmdLine.h>

int main(int argc, char** argv){
   try {  
      TCLAP::CmdLine cmd("Command description message", ' ', "0.9");

      // positional argument (required)
      TCLAP::UnlabeledValueArg<std::string> simFilenameArg("simfile","Input similarity file",true,"not there","string");
      TCLAP::ValueArg<float> thresholdArg("T","threshold","Initial threshold of heuristic",false,0.0,"double");
      TCLAP::ValueArg<float> threshold_maxArg("M","threshold_max","Maximim threshold of heuristic",false,0.0,"double");
      TCLAP::ValueArg<float> threshold_stepArg("S","threshold_step","Step size of threshold in heuristic",false,0.0,"double");
/*
      TCLAP::ValueArg<bool> trainParametersArg("P","train_parameters","Step size of threshold in heuristic",false,true,"bool");
      TCLAP::ValueArg<float> numGenerationsArg("S","threshold_step","Step size of threshold in heuristic",false,0.0,"double");
      TCLAP::ValueArg<float> generationSizeArg("S","threshold_step","Step size of threshold in heuristic",false,0.0,"double");
      */
      /*
                     NumericVector cpp_REF,               // golden standard ref
                    double   cpp_threshold_step,         // main loop

                    bool     cpp_trainParameters,        // training
                    int      cpp_numGenerations,         // training
                    int      cpp_generationSize,         // training
                    int      cpp_p,                      // force layout
                    double   cpp_f_att,                  // force layout
                    double   cpp_f_rep,                  // force layout
                    int      cpp_R,                      // force layout
                    double   cpp_start_t,                // force layout
                    int      cpp_dim,                    // force layout
                    double   cpp_d_init,                 // force partition
                    double   cpp_d_maximal,              // force partition
                    double   cpp_s_init,                 // force partition
                    double   cpp_f_s,                    // force partition
                    bool     cpp_postProcessing          // post processing
      */


      cmd.add( simFilenameArg);
      cmd.add( thresholdArg );
      cmd.add( threshold_maxArg );
      cmd.add( threshold_stepArg );
      cmd.parse( argc, argv );

      std::string simFilename = simFilenameArg.getValue();
      
      TransClust transclust(simFilename);

   }catch (TCLAP::ArgException &e){ 
      std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
   }
   return 0;
}
