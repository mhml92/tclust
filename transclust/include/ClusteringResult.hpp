#ifndef CLUSTERINGRESULT_HPP
#define CLUSTERINGRESULT_HPP
#include <vector>

// Struct holding a partial clustering
typedef struct ClusteringResult {
   float cost;
   std::vector<unsigned> membership; 
} clustering;
#endif
