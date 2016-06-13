#ifndef RESULT_HPP
#define RESULT_HPP
#include <vector>
#include <map>
#include "ConnectedComponent.hpp"
#include "ClusteringResult.hpp"

class Result{
	public:
		Result(std::vector<std::string>id2object);
		void add(ConnectedComponent& cc,ClusteringResult& cr);
		void dump();
	private:
		std::vector<std::string> id2object;
		std::map<float,float> cost;
		std::map<float,std::vector<std::vector<unsigned>>> clusters;
};


#endif
