#include "ClusteringResult.hpp"

ClusteringResult::ClusteringResult(
      std::vector<unsigned>& best_clustering, 
      float best_cost)
   :
      clustering(best_clustering),
      cost(best_cost)
{ }


