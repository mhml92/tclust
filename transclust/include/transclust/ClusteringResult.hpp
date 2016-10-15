#ifndef CLUSTERINGRESULT_HPP
#define CLUSTERINGRESULT_HPP
#include <vector>
#include <string>

// Struct holding a partial clustering
struct ClusteringResult {
	float cost;
	std::vector<unsigned> membership;
};

typedef struct clustering {
	std::vector<std::string> id2object;
	std::vector<float> threshold;
	std::vector<float> cost;
	std::vector< std::vector<unsigned> > clusters;
}clustering;

#endif
