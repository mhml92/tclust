#include <cmath>
#include <iomanip>
#include <list>
#include <limits>
#include <Rcpp.h>
#include "transclust/FindConnectedComponents.hpp"
#include "transclust/ConnectedComponent.hpp"

namespace FCC{
	/*******************************************************************************
	 * FIND CCs IN CC W. THRESHOLD
	 ******************************************************************************/
	void findConnectedComponents(
			const ConnectedComponent &cc,
			std::queue<ConnectedComponent> &ccs,
			const double threshold)
	{
	   std::vector<std::vector<unsigned>> membership = findMembershipVector(cc,threshold);

	   /*
	   for(unsigned i = 0; i < membership.size(); i++){
	      // test that in each member there is at least one other element with a
	      // positive similarity
	      for(auto& i_m:membership.at(i))
	      {
	         bool has_neighbor = false;
	         for(auto& j_m:membership.at(i))
	         {
	            if(i_m != j_m){
	               if(cc.getMatrix()(i_m,j_m)-threshold > 0){
	                  has_neighbor = true;
	               }
	            }
	         }
	         if(!has_neighbor && membership.at(i).size() > 1){
	            Rcpp::Rcout << __FILE__<< " " <<__LINE__ << " noooo" << std::endl;
	            exit(1);
	         }
	      }


	      // test that for each member in the i'th ccv there is no member in
	      // the j'th ccv which has similarity > threshold
	      for(unsigned j = i+1; j < membership.size(); j++){
            for(auto& i_ccv:membership.at(i))
            {
               for(auto& j_ccv:membership.at(j))
               {
                  if((cc.getMatrix()(i_ccv,j_ccv) - threshold) > 0){
                     Rcpp::Rcout << __FILE__<< " " <<__LINE__ << " noooo" << std::endl;
	                  exit(1);

                  }

               }

            }
	      }
	   }
	   */

	   for(auto &ccv:membership)
	   {
	      ccs.push(ConnectedComponent(cc,ccv,threshold));
		}
	}

	/*******************************************************************************
	 * DETERMINE MEMBERSHIP IN CC
	 ******************************************************************************/
	std::vector<std::vector<unsigned>> findMembershipVector(
			const ConnectedComponent &cc,
			const double threshold)
	{
		std::list<unsigned> nodes;
		// fill list of nodes
		for (unsigned i=1; i< cc.size();i++)
		{
			nodes.push_back(i);
		}

		// result vector
		std::vector<std::vector<unsigned>> result;
		result.push_back(std::vector<unsigned>());

		std::queue<unsigned> Q;
		unsigned componentId = 0;
		Q.push(0);
		result.at(componentId).push_back(0);
		while(!nodes.empty()){

			unsigned i = Q.front();
			for (auto it = nodes.begin(); it != nodes.end();)
			{
				unsigned j = *it;
				if(j != i)
				{

					double cost = (std::rint( (cc.getMatrix()(i,j)-threshold) *100000)/100000.0);
					if (cost > 0)
					{
						Q.push(j);
						result.at(componentId).push_back(j);
						it = nodes.erase(it);
					}else{
						//std::cout
						//	<<std::setprecision(std::numeric_limits<double>::digits10 + 1)
						//	<<  cost
						//	<< std::endl;
						++it;
					}
				}else{
					++it;
				}
			}

			Q.pop();

			if(Q.empty())
			{
				if(!nodes.empty())
				{
					componentId++;
					result.push_back(std::vector<unsigned>());
					Q.push(nodes.front());
					result.at(componentId).push_back(nodes.front());
					nodes.pop_front();
				}
			}
		}
		return result;
	}
}
