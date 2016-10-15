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
	/****************************************************************************
	 * FIND CCs IN CC W. THRESHOLD
	 ***************************************************************************/
	void findConnectedComponents(
			TCC::TransClustParams& tcp,
			ConnectedComponent &cc,
			std::queue<ConnectedComponent> &ccs,
			const float threshold)
	{
		std::vector<std::vector<unsigned>>membership; 

		if(tcp.external){
			DUF_cc(membership,cc,threshold);
		}else{
			BFS_cc(membership,cc,threshold);
		}

		DEBUG_FCC(membership,cc,threshold);

		for(auto &ccv:membership)
		{
			ccs.push(ConnectedComponent(cc,ccv,threshold,tcp));
		}
	}

	/****************************************************************************
	 * DETERMINE MEMBERSHIP IN CC
	 ***************************************************************************/

	void DUF_cc(
			std::vector<std::vector<unsigned>>& membership,
			ConnectedComponent &cc,
			const float threshold)
	{
		std::vector<int> duf_result;
		for(int i = 0; i < cc.size(); i++)
		{
			for(int j = i+1; j < cc.size(); j++)
			{
				if(cc.getCost(i,j,threshold) > 0){
					DUF::funion(duf_result,i,j);
				}
			}
		}
		/* mapping root => vector in membership  */
		std::map<unsigned,unsigned>cc_map;
		for(unsigned i = 0; i < duf_result.size(); i++){
			unsigned root = 0;
			if(duf_result.at(i) < 0)
			{
				root = i;
			}else{
				root = DUF::find(duf_result,i);
			}

			if(cc_map.find(root) == cc_map.end())
			{
				membership.push_back(std::vector<unsigned>());
				cc_map[root] = membership.size()-1;	
			}
			membership.at(cc_map[root]).push_back(i);
		}
	}


	void BFS_cc(
			std::vector<std::vector<unsigned>>& membership,
			ConnectedComponent &cc,
			const float threshold)
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
