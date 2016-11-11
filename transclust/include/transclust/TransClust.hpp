#ifndef TRANSCLUST_HPP
#define TRANSCLUST_HPP
#include <string>
#include <queue>
#include <deque>
#include <vector>
#include <map>
#include <cmath>
#include <plog/Log.h>
#include "transclust/Common.hpp"
#include "transclust/InputParser.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/ClusteringResult.hpp"

class TransClust{

	public:
		TransClust(
				const std::string& filename,
				TCC::TransClustParams& _tcp
				);

		RES::Clustering cluster();
	private:

		inline void addResult(
				ConnectedComponent& cc,
				RES::ClusteringResult& cr)
		{
			if(cc.isTransitive())
			{
				cr.cost = 0;
				result.clusters.push_back(cc.getIndex2ObjectId());
			}else{

				for(auto cluster:cr.clusters)
				{
					result.clusters.push_back(std::deque<unsigned>());
					for(unsigned local_id:cluster)
					{
						result.clusters.back().push_back(cc.getObjectId(local_id));
					}
				}
			}
			
			result.cost += cr.cost;
		}

		TCC::TransClustParams tcp;
		InputParser ip;
		float total_cost = 0;
		std::deque<ConnectedComponent> ccs;
		std::deque<std::string> id2name; 
		std::deque<std::deque<unsigned>> clusters;
		RES::Clustering result;
		
};
#endif
