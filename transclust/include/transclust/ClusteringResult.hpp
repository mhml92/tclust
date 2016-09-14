#ifndef CLUSTERINGRESULT_HPP
#define CLUSTERINGRESULT_HPP
#include <vector>
#include <string>

// Struct holding a partial clustering
struct ClusteringResult {
	double cost;
	std::vector<unsigned> membership; 
};

typedef struct clustering {
	std::vector<std::string> id2object;
	std::vector<double> threshold;
	std::vector<double> cost;
	std::vector<std::vector<std::vector<unsigned>>> clusters;
}clustering;

#endif
