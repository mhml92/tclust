#include <cmath>
#include <iomanip>
#include <list>
#include <limits>
#include "transclust/FindConnectedComponents.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/DynamicUnionFind.hpp"
#ifndef NDEBUG
#	include "transclust/DEBUG.hpp"
#	define DEBUG_FCC(membership, cc, threshold) DEBUG::findConnectedComponents(membership,cc,threshold)
#else
#	define DEBUG_FCC(membership, cc, threshold) {}
#endif

namespace FCC{
	/*******************************************************************************
	 * FIND CCs IN CC W. THRESHOLD
	 ******************************************************************************/
	void findConnectedComponents(
			const ConnectedComponent &cc,
			std::queue<ConnectedComponent> &ccs,
			const double threshold)
	{


		///* testing DUF */
		//if(cc.size() > 1){
		//	std::cout << "CC SIZE " << cc.size() << std::endl;
		//	std::vector<int> _membership;
		//	for(int i = 0; i < cc.size(); i++)
		//	{
		//		for(int j = i+1; j < cc.size(); j++)
		//		{
		//			if(cc.at(i,j) > 0){
		//				DUF::funion(_membership,i,j);
		//			}

		//		}
		//	}
		//	for(unsigned i = 0; i < cc.size(); i++){
		//		std::cout << _membership.at(i) << ", ";
		//	}
		//	std::cout << std::endl;
		//}
		///******************************/



		std::vector<std::vector<unsigned>> membership = findMembershipVector(cc,threshold);

		DEBUG_FCC(membership,cc,threshold);

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
					//double cost = (std::rint((cc.getMatrix()(i,j)-threshold)*100000)/100000);
					double cost = TCC::round(cc.getMatrix()(i,j)-threshold);
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
