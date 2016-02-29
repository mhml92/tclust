#include <TransClust.hpp>
#include <TriangularMatrix.hpp>
#include <FindConnectedComponents.hpp>

TransClust::TransClust(const std::string &filename){

   // Read input similarity file
   TriangularMatrix sim_matrix(filename);
   cluster(sim_matrix);
}


int TransClust::cluster(TriangularMatrix &costMatrix){

   // decompose into connected components
   std::vector<TriangularMatrix> ccs = findConnectedComponents(costMatrix,0);

   // debug print connected components
   for(auto &cc:ccs){
      cc.dump();
   }
   return 0;
}
/*
List cppTransclustr( NumericMatrix cpp_sim_matrix,        // sim matrix
      NumericVector cpp_REF,               // golden standard ref
      double   cpp_threshold_current,      // main loop
      double   cpp_threshold_max,          // main loop
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
      ){
      */
