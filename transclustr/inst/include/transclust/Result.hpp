#ifndef RESULT_HPP
#define RESULT_HPP
#include <vector>
#include <map>
#include "transclust/ConnectedComponent.hpp"
#include "transclust/ClusteringResult.hpp"

class Result{
	public:
		Result(std::vector<std::string>id2object);
		void add(ConnectedComponent& cc,ClusteringResult& cr);
		clustering get();
		void dump();


	private:
		std::vector<std::string> id2object;
		std::map<double,double> cost;
		std::map<double,std::vector<std::vector<unsigned>>> clusters;
};


#endif
