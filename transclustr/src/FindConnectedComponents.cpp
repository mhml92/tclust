#include <cmath>
#include <iomanip>
#include <list>
#include <limits>
#include "transclust/FindConnectedComponents.hpp"
#include "transclust/ConnectedComponent.hpp"
#ifndef NDEBUG
#	include "transclust/DEBUG.hpp"
#	define DEBUG_FCC(membership, cc, threshold) DEBUG::findConnectedComponents(membership,cc,threshold)
#else
#	define DEBUG_FCC(membership, cc, threshold) {}
#endif

namespace FCC{
	/****************************************************************************
	 * FIND CCs IN CC W. THRESHOLD
	 ***************************************************************************/
	void findConnectedComponents(
			TCC::TransClustParams& tcp,
			const ConnectedComponent &cc,
			std::queue<ConnectedComponent> &ccs,
			const double threshold)
	{
		std::vector<std::vector<unsigned>>membership;

	   BFS_cc(membership,cc,threshold);
		DEBUG_FCC(membership,cc,threshold);

		for(auto &ccv:membership)
		{
			ccs.push(ConnectedComponent(cc,ccv,threshold,tcp));
		}

	}

	/****************************************************************************
	 * DETERMINE MEMBERSHIP IN CC
	 ***************************************************************************/
	void BFS_cc(
			std::vector<std::vector<unsigned>>& membership,
			const ConnectedComponent &cc,
			const double threshold)
	{
		std::list<unsigned> nodes;
		// fill list of nodes
		for (unsigned i=1; i< cc.size();i++)
		{
			nodes.push_back(i);
		}

		membership.push_back(std::vector<unsigned>());

		std::queue<unsigned> Q;
		unsigned componentId = 0;
		Q.push(0);
		membership.at(componentId).push_back(0);
		while(!nodes.empty()){

			unsigned i = Q.front();
			for (auto it = nodes.begin(); it != nodes.end();)
			{
				unsigned j = *it;
				if(j != i)
				{
					if (cc.getCost(i,j,threshold) > 0)
					{
						Q.push(j);
						membership.at(componentId).push_back(j);
						it = nodes.erase(it);
					}else{
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
					membership.push_back(std::vector<unsigned>());
					Q.push(nodes.front());
					membership.at(componentId).push_back(nodes.front());
					nodes.pop_front();
				}
			}
		}
	}
}
