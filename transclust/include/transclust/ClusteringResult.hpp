#ifndef CLUSTERINGRESULT_HPP
#define CLUSTERINGRESULT_HPP
#include <vector>
#include <deque>
#include <deque>
#include <string>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/deque.hpp>
#include "transclust/Common.hpp"
#include "transclust/ConnectedComponent.hpp"

namespace RES
{
	// Struct holding a partial clustering
	struct ClusteringResult {
		double cost;
		std::deque<std::deque<unsigned>> clusters;

		template <typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & cost;
			ar & clusters;
		}

	};

	inline double calculateCost_membership(
			ConnectedComponent& cc, 
			std::deque<std::deque<unsigned>>& clustering)
	{
		std::vector<unsigned> membership(cc.size(),std::numeric_limits<unsigned>::max());
		unsigned clusterId = 0;
		for(auto& cluster:clustering)
		{
			for(unsigned i = 0; i < cluster.size(); i++)
			{
				membership.at(cluster.at(i)) = clusterId;
			}
			clusterId++;
		}

		double cost = 0;
		for(unsigned i = 0; i< membership.size(); i++)
		{
			// read all neseccary cost values for object i in to a buffer
			// to minimoze potential I/Os
			std::vector<float> cost_buffer(membership.size()-(i+1),0);
			cc.getBufferedCost(cost_buffer,i,i+1,false);
			
			unsigned j_pos = 0;

			for(unsigned j = i+1; j < membership.size(); j++)
			{
				float alt_cost = cost_buffer.at(j_pos);
				j_pos++;
				float _cost = cc.getCost(i,j,false);
				if(_cost - alt_cost != 0){
					std::cout << "diff in fetched cost:\n"
						<< "direct cost:   " << _cost << "\n"
						<< "buffered cost: " << alt_cost << std::endl;
				}
				if((membership.at(i) != membership.at(j)) && _cost > 0.0)
				{
					cost += _cost;
				}else if((membership.at(i) == membership.at(j)) && _cost < 0.0)
				{
					cost += std::fabs(_cost);
				}
			}
		}
		return cost;
	}

	inline double calculateCost(
			ConnectedComponent& cc, 
			std::deque<std::deque<unsigned>>& clustering)
	{
		double cost = 0;

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
		// calculate external cost of each connected component
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
	}
}
#endif
