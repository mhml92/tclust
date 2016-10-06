#ifndef TRANSCLUST_HPP
#define TRANSCLUST_HPP
#include <string>
#include <queue>
#include <vector>
#include <map>
#include <cmath>
#include "transclust/Common.hpp"
#include "transclust/TriangularMatrix.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/ClusteringResult.hpp"
#include "transclust/FPT.hpp"
#include "transclust/Result.hpp"


class TransClust{

public:
   TransClust(
      const std::string& filename,
      TCC::TransClustParams& tcp
   );

   TransClust(
      std::vector<double>& sim_matrix_1d,
      unsigned num_o,
      TCC::TransClustParams& tcp
   );

   clustering cluster();
private:
   void init(TCC::TransClustParams& tcp);

//   inline double round(double d){
//      return std::rint(d*100000)/100000;
//   };

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
