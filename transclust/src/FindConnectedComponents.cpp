#include <cmath> 
#include <iomanip>
#include <list>
#include <limits>
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

		//std::cout << cc.getThreshold() << "\t\t\t" << membership.size() <<"\t\t\t"<< cc.size() << std::endl;
		
		//std::cout << "---------------------------------------------------------------------" << std::endl;
		//std::cout << "CC size: " << cc.size() << std::endl;
		//std::cout << "CC threshold: " << cc.getThreshold() << std::endl;
		//std::cout << "num clusters: " << membership.size() << std::endl;
		//int count = 1;
		for(auto &ccv:membership)
		{
			//std::cout << "cluster " << count << " size: " << ccv.size() << std::endl;
			//count++;
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
					double cost = (std::rint((cc.getMatrix()(i,j)-threshold)*100000)/100000);
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
