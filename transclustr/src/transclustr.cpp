#include <Rcpp.h>
#include "TransClust.hpp"
using namespace Rcpp;

// Enable C++11 via this plugin (Rcpp 0.10.3 or later)
// [[Rcpp::plugins(cpp11)]]

// [[Rcpp::export]]
List cluster(std::string filename) {
   TransClust tc(filename);
   clustering res = tc.cluster();
   return List::create(
      Named("id2object") = res.id2object,
      Named("cost") = res.cost,
      Named("clusters") = res.clusters,
      Named("threshold") = res.threshold
   );
}


/*** R
str(cluster("/home/mikkel/Dropbox/Datalogi/Speciale/code/transclust/data/karateclub/Zachary_karate_club_similarities.sim"))
*/
