#ifndef DEBUG_HPP
#define DEBUG_HPP
#include <vector>
#include <set>
#include <iomanip>
#include "FPT.hpp"
#include "ConnectedComponent.hpp"
#include "ClusteringResult.hpp"
#include <plog/Log.h>


namespace DEBUG{
	
	inline unsigned num_clusters(std::vector<unsigned> &v){
		std::set<unsigned> s(v.begin(),v.end());	
		return s.size();
	}

	inline double calculate_cluster_cost(
			ConnectedComponent &cc,
			ClusteringResult &cr
			)
	{
		double cost = 0;
		for( unsigned i = 0; i < cc.size(); i++ )
		{
			for ( unsigned j = i+1; j < cc.size(); j++ )
			{
				if( (cc.at(i,j,false) > 0) && (cr.membership.at(i) != cr.membership.at(j)) )
				{
					//LOG_DEBUG << "adding " << cc.at(i,j,false) << " to cost";
					cost += cc.at(i,j,false);
				}
				else if( (cc.at(i,j,false) < 0) && (cr.membership.at(i) == cr.membership.at(j)) )
				{
					// (cost - -sim)
					//LOG_DEBUG << "adding (-) " << cc.at(i,j,false) << " to cost";
					cost += -cc.at(i,j,false);
				}
			}
		}
		for(unsigned i = 0; i < cr.membership.size();i++){
			std::cout << std::setw(10) << cr.membership[i];
		}
		std::cout << std::endl;
		return cost;
	}

	inline void is_edgecost_symmetric(FPT::Node &fptn)
	{
		LOG_VERBOSE << "DEBUG is is_edgecost_symmetric";
		for( unsigned i = 0; i < fptn.size; i++)
		{
			for( unsigned j = i+1; j < fptn.size; j++)
			{
				LOG_VERBOSE << fptn.edgeCost.at(i).at(j) << ", " << fptn.edgeCost.at(j).at(i);
				if(fptn.edgeCost.at(i).at(j) != fptn.edgeCost.at(j).at(i))
				{
					LOG_FATAL << "edge cost not symmetric";
				}

			}
		
		}

		for( unsigned i = 0; i < fptn.size; i++)
		{
			if(fptn.edgeCost.at(i).at(i) != 0){
				LOG_DEBUG << "diagonal not 0 at  " << i << ", " << i << " -> " << fptn.edgeCost.at(i).at(i); 
			}
		}
		
	}

	inline void dump_fptn(FPT::Node& fptn){
		for(unsigned i = 0; i < fptn.size; i++)
		{
			for(unsigned j = 0; j < fptn.size; j++)
			{
				std::cout << std::setw(9) << fptn.edgeCost.at(i).at(j);
			}
			std::cout << std::endl;
		}
	}
}

#endif
