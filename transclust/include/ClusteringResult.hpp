#ifndef CLUSTERINGRESULT_HPP
#define CLUSTERINGRESULT_HPP
#include <vector>

// Struct holding a partial clustering
struct ClusteringResult {
	double cost;
	std::vector<unsigned> membership; 
};
#endif
