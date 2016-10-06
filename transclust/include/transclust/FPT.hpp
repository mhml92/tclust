#ifndef FPT_HPP
#define FPT_HPP
#include <iomanip>
#include <vector>
#include <list>
#include <iostream>
#include <string>
#include <limits>
#include <chrono>
#include "transclust/Common.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/ClusteringResult.hpp"


class FPT{

 	public:
		struct Node {
			std::vector<std::vector<unsigned>> nodeParents;
			std::vector<std::vector<double>> edgeCost;
			unsigned size;
			double cost;
		};

		FPT(
			ConnectedComponent& cc,
			double time_limit,
			double stepSize,
			double mockInf
		);

		void cluster(ClusteringResult &cr);

	private:

		ConnectedComponent &cc;
		std::chrono::time_point<std::chrono::system_clock> start; 
		double time_limit;
		double stepSize;
		double maxK;
		double inf;
		int LEVEL = 0;

		bool solution_found;
		double solution_cost;
		std::vector<std::vector<unsigned>> solution_nodeParents;
		std::vector<std::vector<double>> solution_edgeCost;


		inline double getDeltaTime()
		{
			std::chrono::duration<double> diff = (std::chrono::system_clock::now()-start);
			return diff.count();
		}
		void reduce(Node& fptn);
		double costSetForbidden(
				Node& fptn, 
				unsigned node_i,
				unsigned node_j);

		double costSetPermanent(
				Node& fptn, 
				unsigned node_i,
				unsigned node_j);
		void find_solution(Node& fptn0);
		void mergeNodes(
			Node& fptn, 
			unsigned i,
			unsigned j, 
			double costForMerging);
		
		void buildSolution(ClusteringResult &cr);
		void clone_node(Node& fptn0,Node& fptn1);
};
#endif
