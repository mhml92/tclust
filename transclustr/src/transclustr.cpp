#include <Rcpp.h>
#include "TransClust.hpp"

// Enable C++11 via this plugin (Rcpp 0.10.3 or later)
// [[Rcpp::plugins("cpp11")]]
using namespace Rcpp;

// [[Rcpp::export]]
void transcluster(std::string x) {
   TransClust tc(x);
   auto r = tc.cluster();
}
/*** R
transcluster("/home/mikkel/Dropbox/Datalogi/Speciale/code/transclust/data/karateclub/Zachary_karate_club_similarities.sim")
*/
