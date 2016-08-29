#ifndef NEW_FPT_HPP
#define NEW_FPT_HPP
#include <vector>
#include <chrono>

#include "ConnectedComponent.hpp"
#include "ClusteringResult.hpp"

class New_FPT{
	public:
		struct Node {
			std::vector<std::vector<bool>> hasEdge;
			std::vector<std::vector<unsigned>> nodeParents;
			std::vector<std::vector<double>> edgeCost;
			unsigned size;
			double cost;
		};

		New_FPT(
			ConnectedComponent& cc,
			ClusteringResult& cr,
			double time_limit,
			double stepSize
		);

		void cluster(ClusteringResult& cr);
	private:
		inline double getDeltaTime()
		{
			std::chrono::duration<double> diff = (std::chrono::system_clock::now()-start);
			return diff.count();
		}

		std::chrono::time_point<std::chrono::system_clock> start; 
		bool solution_found = false;
		Node solution;

		ConnectedComponent &cc;
		double time_limit;
		double stepSize;
		double maxK;
		double icf(Node& fptn,unsigned u,unsigned v);
		double icp(Node& fptn,unsigned u,unsigned v);
		void findConflictTriple(Node& fptn,std::vector<unsigned>& ct);
		//void reduce(Node& fptn);
		void find_solution(Node& fptn);
		void initializeRootNode(ConnectedComponent& cc,ClusteringResult& cr, Node& fptn);
};


#endif
