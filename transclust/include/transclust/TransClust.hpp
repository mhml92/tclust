#ifndef TRANSCLUST_HPP
#define TRANSCLUST_HPP
#include <string>
#include <queue>
#include <vector>
#include <map>
#include <cmath>
#include "transclust/TriangularMatrix.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/ClusteringResult.hpp"
#include "transclust/FPT.hpp"
#include "transclust/Result.hpp"

/* Config struct with fluint interface */
struct TransClustParams {
   bool       use_custom_fallback = false;
   double     sim_fallback        = 0.0;
   bool       use_default_interval    = true;
   double     th_min              = 0.0;
   double     th_max              = 100;
   double     th_step             = 1.0;
   double     p                   = 1.0;
   double     f_att               = 100.0;
   double     f_rep               = 100.0;
   unsigned   R                   = 100;
   unsigned   dim                 = 3;
   double     start_t             = 100;
   double     d_init              = 0.01;
   double     d_maximal           = 5.0;
   double     s_init              = 0.01;
   double     f_s                 = 0.01;
   double     fpt_time_limit      = 20;
   double     fpt_max_cost        = 5000;
   double     fpt_step_size       = 10;
   bool       disable_force       = false;
   bool       disable_fpt         = false;
   unsigned   seed                = 42;
   TransClustParams& set_use_custom_fallback(bool val){use_custom_fallback = val;return *this;}
   TransClustParams& set_sim_fallback(double val){sim_fallback = val;return *this;}
   TransClustParams& set_use_default_interval(bool val){use_default_interval= val;return *this;}
   TransClustParams& set_th_min(double val){th_min = val;return *this;}
   TransClustParams& set_th_max(double val){th_max = val;return *this;}
   TransClustParams& set_th_step(double val){th_step = val;return *this;}
   TransClustParams& set_p(double val){p = val;return *this;}
   TransClustParams& set_f_att(double val){f_att = val;return *this;}
   TransClustParams& set_f_rep(double val){f_rep = val;return *this;}
   TransClustParams& set_R(double val){R = val;return *this;}
   TransClustParams& set_dim(double val){dim = val;return *this;}
   TransClustParams& set_start_t(double val){start_t = val;return *this;}
   TransClustParams& set_d_init(double val){d_init = val;return *this;}
   TransClustParams& set_d_maximal(double val){d_maximal = val;return *this;}
   TransClustParams& set_s_init(double val){s_init = val;return *this;}
   TransClustParams& set_f_s(double val){f_s = val;return *this;}
   TransClustParams& set_fpt_time_limit(double val){fpt_time_limit = val;return *this;}
   TransClustParams& set_fpt_max_cost(double val){fpt_max_cost = val;return *this;}
   TransClustParams& set_fpt_step_size(double val){fpt_step_size = val;return *this;}
   TransClustParams& set_disable_force(bool val){disable_force = val;return *this;}
   TransClustParams& set_disable_fpt(bool val){disable_fpt = val;return *this;}
   TransClustParams& set_seed(bool val){seed = val;return *this;}
};

class TransClust{

public:
   TransClust(
      const std::string& filename,
      std::string& file_type,
      TransClustParams& tcp
   );

   TransClust(
      std::vector<double>& sim_matrix_1d,
      unsigned num_o,
      TransClustParams& tcp
   );

   clustering cluster();
private:
   void init(TransClustParams& tcp);

   inline double round(double d){
      return std::rint(d*100000)/100000;
   };

   // general vars
   bool use_custom_fallback;
   double sim_fallback;
   bool use_custom_range;
   double threshold_min;
   double threshold_max;
   double threshold_step;

   // Layout values
   double p;
   double f_att;
   double f_rep;
   unsigned R;
   double start_t;
   unsigned dim;

   // partitioning values
   double d_init;
   double d_maximal;
   double s_init;
   double f_s;

   // FPT values
   double fpt_time_limit;
   double fpt_max_cost;
   double fpt_step_size;

   bool disable_force;
   bool disable_fpt;

   unsigned seed;

   std::queue<ConnectedComponent> ccs;
   std::vector<std::string> id2object;
};
#endif
