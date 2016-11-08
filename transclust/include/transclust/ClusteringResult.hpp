#ifndef CLUSTERINGRESULT_HPP
#define CLUSTERINGRESULT_HPP
#include <vector>
#include <deque>
#include <deque>
#include <string>
#include "transclust/ConnectedComponent.hpp"

namespace RES
{
	// Struct holding a partial clustering
	struct ClusteringResult {
		float cost;
		std::deque<std::deque<unsigned>> clusters;
	};

	struct Clustering {
		std::deque<std::string> id2name;
		float threshold;
		float cost;
		std::deque<std::deque<unsigned>> clusters;
	};

	inline float calculateCost(
			ConnectedComponent& cc, 
			std::deque<std::deque<unsigned>>& clustering)
	{
		float cost = 0;

		// calculate internal cost of each connected component
		for(auto& cluster:clustering)
		{
			for(unsigned i = 0; i < cluster.size(); i++)
			{
				for(unsigned j = i+1; j < cluster.size(); j++)
				{
					unsigned _i,_j;
					_i = cluster.at(i);
					_j = cluster.at(j);
					float edge_cost = cc.getCost(_i,_j,false);
					if(edge_cost < 0.0)
					{
						cost += std::fabs(edge_cost);
					}
				}
			}
		}
		// calculate extermaæ cost of each connected component
		for(unsigned i = 0; i < clustering.size(); i++)
		{
			for(unsigned j = i+1; j < clustering.size(); j++)
			{
				for(unsigned u = 0; u < clustering.at(i).size();u++)
				{
					for(unsigned v = 0; v < clustering.at(j).size();v++)
					{
						unsigned _u,_v;
						_u = clustering.at(i).at(u);
						_v = clustering.at(j).at(v);
						float edge_cost = cc.getCost(_u,_v,false);
						if(edge_cost > 0.0)
						{
							cost += edge_cost;
						}
					}
				}
			}
		}
		return cost;
		//auto _t2 = std::chrono::high_resolution_clock::now();
		//std::cout << std::endl;
		//std::cout << "new cost: " << std::chrono::duration_cast<std::chrono::nanoseconds>(_t2-_t1).count()  << " " <<TEST_cost <<  std::endl;
		//NEW_SCORE += std::chrono::duration_cast<std::chrono::nanoseconds>(_t2-_t1).count();

		//auto t1 = std::chrono::high_resolution_clock::now();

		//	std::vector<unsigned> membership(cc.size(),std::numeric_limits<unsigned>::max());
		//	unsigned clusterId = 0;
		//	for(auto& cluster:clustering){
		//		for(unsigned i = 0; i < cluster.size(); i++){
		//			membership.at(cluster.at(i)) = clusterId;
		//		}
		//		clusterId++;
		//	}

		//	float cost = 0;
		//	for(unsigned i = 0; i< membership.size(); i++)
		//	{
		//		for(unsigned j = i+1; j < membership.size(); j++)
		//		{
		//			float _cost = cc.getCost(i,j,false);
		//			if((membership.at(i) != membership.at(j))
		//					&& _cost > 0.0)
		//			{

		//				cost += _cost;
		//			}else if((membership.at(i) == membership.at(j))
		//					&& _cost < 0.0)
		//			{
		//				cost -= _cost;
		//			}
		//		}
		//	}

	}

}

#endif
