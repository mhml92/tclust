#include <Rcpp.h>
#include <omp.h>
#include "transclust/TransClust.hpp"
using namespace Rcpp;

// Enable C++11 via this plugin (Rcpp 0.10.3 or later)
// [[Rcpp::plugins(cpp11)]]

// [[Rcpp::export]]
List cppTransclustFile(
      std::string filename,
      double threshold,
      bool       use_custom_fallback = false,
      double     sim_fallback        = 0.0,
      double     p                   = 1.0,
      double     f_att               = 100.0,
      double     f_rep               = 100.0,
      unsigned   R                   = 100,
      unsigned   dim                 = 3,
      double     start_t             = 100,
      double     d_init              = 0.01,
      double     d_maximal           = 5.0,
      double     s_init              = 0.01,
      double     f_s                 = 0.01,
      double     fpt_time_limit      = 20,
      double     fpt_max_cost        = 5000,
      double     fpt_step_size       = 10,
      bool       disable_force       = false,
      bool       disable_fpt         = false,
      std::string file_type              = "SIMPLE"
) {
   FileType ft;
   if(file_type == "SIMPLE"){
      ft = FileType::SIMPLE;
   }else if(file_type == "LEGACY"){
      ft = FileType::LEGACY;
   }
   TransClust tc(
         filename,
         use_custom_fallback,
         sim_fallback,
         true,
         threshold,
         threshold,
         threshold,
         p,
         f_att,
         f_rep,
         R,
         dim,
         start_t,
         d_init,
         d_maximal,
         s_init,
         f_s,
         fpt_time_limit,
         fpt_max_cost,
         fpt_step_size,
         disable_force,
         disable_fpt,
         ft
   );
   clustering res = tc.cluster();
   return List::create(
      Named("id2object") = res.id2object,
      Named("cost") = res.cost,
      Named("clusters") = res.clusters,
      Named("threshold") = res.threshold
   );
}

// [[Rcpp::export]]
List cppTransclustMatrix(
      NumericMatrix simmatrix,
      double threshold,
      bool       use_custom_fallback = false,
      double     sim_fallback        = 0.0,
      double     p                   = 1.0,
      double     f_att               = 100.0,
      double     f_rep               = 100.0,
      unsigned   R                   = 100,
      unsigned   dim                 = 3,
      double     start_t             = 100,
      double     d_init              = 0.01,
      double     d_maximal           = 5.0,
      double     s_init              = 0.01,
      double     f_s                 = 0.01,
      double     fpt_time_limit      = 20,
      double     fpt_max_cost        = 5000,
      double     fpt_step_size       = 10,
      bool       disable_force       = false,
      bool       disable_fpt         = false
) {
   std::vector<std::vector<double>> sm;

   for(unsigned i = 0; i < simmatrix.cols();i++){
      sm.push_back(std::vector<double>());
      for(unsigned j = 0; j< simmatrix.rows();j++){
         sm.at(i).push_back(simmatrix.at(i,j));
      }
   }
   TransClust tc(
         sm,
         use_custom_fallback,
         sim_fallback,
         true,
         threshold,
         threshold,
         threshold,
         p,
         f_att,
         f_rep,
         R,
         dim,
         start_t,
         d_init,
         d_maximal,
         s_init,
         f_s,
         fpt_time_limit,
         fpt_max_cost,
         fpt_step_size,
         disable_force,
         disable_fpt
      );
   clustering res = tc.cluster();
   return List::create(
      Named("id2object") = res.id2object,
      Named("cost") = res.cost,
      Named("clusters") = res.clusters,
      Named("threshold") = res.threshold
   );
}
