#include <Rcpp.h>
#include "transclust/TransClust.hpp"

using namespace Rcpp;

// Enable C++11 via this plugin (Rcpp 0.10.3 or later)
// [[Rcpp::plugins(cpp11)]]

// Add a flag to enable OpenMP at compile time
// [[Rcpp::plugins(openmp)]]

List cppCluster(TransClust& tc){
   clustering res = tc.cluster();

   return List::create(
      Named("labels") = res.id2object,
      Named("costs") = res.cost,
      Named("thresholds") = res.threshold,
      Named("clusters") = res.clusters
   );
}

// [[Rcpp::export]]
List cppTransClustFile(
      std::string filename,
      std::string file_type,
      List params
   ) {
   TransClust tc(
         filename,
         TCC::TransClustParams()
            .set_file_type(file_type)
            .set_normalization(params["normalization"])
            .set_use_custom_fallback(as<bool>(params["use_custom_fallback"]))
            .set_sim_fallback(as<double>(params["sim_fallback"]))
            .set_use_default_interval(as<bool>(params["use_default_interval"]))
            .set_th_min(as<double>(params["th_min"]))
            .set_th_max(as<double>(params["th_max"]))
            .set_th_step(as<double>(params["th_step"]))
            .set_p(as<double>(params["p"]))
            .set_f_att(as<double>(params["f_att"]))
            .set_f_rep(as<double>(params["f_rep"]))
            .set_R(as<unsigned>(params["R"]))
            .set_dim(as<unsigned>(params["dim"]))
            .set_start_t(as<double>(params["start_t"]))
            .set_d_init(as<double>(params["d_init"]))
            .set_d_maximal(as<double>(params["d_maximal"]))
            .set_s_init(as<double>(params["s_init"]))
            .set_f_s(as<double>(params["f_s"]))
            .set_fpt_time_limit(as<double>(params["fpt_time_limit"]))
            .set_fpt_max_cost(as<double>(params["fpt_max_cost"]))
            .set_fpt_step_size(as<double>(params["fpt_step_size"]))
            .set_disable_force(as<bool>(params["disable_force"]))
            .set_disable_fpt(as<bool>(params["disable_fpt"]))
            .set_seed(as<unsigned>(params["seed"]))
   );
   return cppCluster(tc);
}

// [[Rcpp::export]]
List cppTransClustDist(
      NumericVector sim_matrix_1d,
      unsigned num_o,
      List params) {
   std::vector<double> sm(sim_matrix_1d.begin(),sim_matrix_1d.end());
   TransClust tc(
         sm,
         num_o,
         TCC::TransClustParams()
            .set_normalization(params["normalization"])
            .set_use_custom_fallback(as<bool>(params["use_custom_fallback"]))
            .set_sim_fallback(as<double>(params["sim_fallback"]))
            .set_use_default_interval(as<bool>(params["use_default_interval"]))
            .set_th_min(as<double>(params["th_min"]))
            .set_th_max(as<double>(params["th_max"]))
            .set_th_step(as<double>(params["th_step"]))
            .set_p(as<double>(params["p"]))
            .set_f_att(as<double>(params["f_att"]))
            .set_f_rep(as<double>(params["f_rep"]))
            .set_R(as<unsigned>(params["R"]))
            .set_dim(as<unsigned>(params["dim"]))
            .set_start_t(as<double>(params["start_t"]))
            .set_d_init(as<double>(params["d_init"]))
            .set_d_maximal(as<double>(params["d_maximal"]))
            .set_s_init(as<double>(params["s_init"]))
            .set_f_s(as<double>(params["f_s"]))
            .set_fpt_time_limit(as<double>(params["fpt_time_limit"]))
            .set_fpt_max_cost(as<double>(params["fpt_max_cost"]))
            .set_fpt_step_size(as<double>(params["fpt_step_size"]))
            .set_disable_force(as<bool>(params["disable_force"]))
            .set_disable_fpt(as<bool>(params["disable_fpt"]))
            .set_seed(as<unsigned>(params["seed"]))
   );
   return cppCluster(tc);
}
