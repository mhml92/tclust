#include <Rcpp.h>
#include <omp.h>
#include "transclust/TransClust.hpp"
using namespace Rcpp;

// Enable C++11 via this plugin (Rcpp 0.10.3 or later)
// [[Rcpp::plugins(cpp11)]]

//' @export
// [[Rcpp::export]]
List transclust(NumericMatrix simmatrix) {
   std::vector<std::vector<double>> sm;

   for(unsigned i = 0; i < simmatrix.cols();i++){
      sm.push_back(std::vector<double>());
      for(unsigned j = 0; j< simmatrix.rows();j++){
         sm.at(i).push_back(simmatrix.at(i,j));
      }
   }
   for(unsigned i = 0; i < simmatrix.cols();i++){
      for(unsigned j = 0; j< simmatrix.rows();j++){
         Rcout << sm.at(i).at(j) << ", ";
      }
      Rcout << std::endl;
   }

   TransClust tc(sm);
   clustering res = tc.cluster();
   return List::create(
      Named("id2object") = res.id2object,
      Named("cost") = res.cost,
      Named("clusters") = res.clusters,
      Named("threshold") = res.threshold
   );
}
