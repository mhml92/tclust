//#include <cmath>
//#include <iomanip>
//#include <list>
//#include <limits>
//#include <plog/Log.h>
//#ifdef _OPENMP
//#	include <omp.h>
//#endif
//#include <unordered_map>
//#include <boost/lockfree/queue.hpp>
//#include "transclust/FindConnectedComponents.hpp"
//#include "transclust/ConnectedComponent.hpp"
//#include "transclust/DynamicUnionFind.hpp"
//#ifndef NDEBUG
//#	include "transclust/DEBUG.hpp"
//#	define DEBUG_FCC(membership, cc, threshold) DEBUG::findConnectedComponents(membership,cc,threshold)
//#else
//#	define DEBUG_FCC(membership, cc, threshold) {}
//#endif
//
//namespace FCC{
//	/****************************************************************************
//	 * FIND CCs IN CC W. THRESHOLD
//	 ***************************************************************************/
//	void findConnectedComponents(
//			TCC::TransClustParams& tcp,
//			ConnectedComponent &cc,
//			std::queue<ConnectedComponent> &ccs,
//			const float threshold)
//	{
//		std::vector<std::vector<unsigned>>membership; 
//
//		LOGD << "Finding connected components for id: " << cc.getId();
//
//		//BFS_cc(membership,cc,threshold);
//		DUF_cc(membership,cc,threshold);
//
//		LOGD << "done. found " << membership.size() << " connected components";
//
//		//if(tcp.external){
//		//	DUF_cc(membership,cc,threshold);
//		//}else{
//		//	BFS_cc(membership,cc,threshold);
//		//}
//		
//		DEBUG_FCC(membership,cc,threshold);
//
//		LOGD << "Creating datastructures for connected components";
//		for(auto &ccv:membership)
//		{
//			ccs.push(ConnectedComponent(cc,ccv,threshold,tcp));
//		}
//		LOGD << "done";
//	}
//
//	/****************************************************************************
//	 * DETERMINE MEMBERSHIP IN CC
//	 ***************************************************************************/
//
//	void DUF_cc(
//			std::vector<std::vector<unsigned>>& membership,
//			ConnectedComponent &cc,
//			const float threshold)
//	{
//		std::vector<long> duf_result(cc.size(),std::numeric_limits<long>::lowest());
//		#pragma omp parallel
//		{
//			std::vector<std::pair<unsigned,unsigned>> edges;
//			#pragma omp for schedule(dynamic)
//			for(unsigned i = 0; i < cc.size(); i++)
//			{
//				for(unsigned j = i+1; j < cc.size(); j++)
//				{
//					if(cc.getCost(i,j,threshold) > 0){
//						edges.push_back(std::make_pair(i,j));
//					}
//				}
//			}
//			#pragma omp critical
//			for(auto& e:edges)
//			{
//				DUF::funion(duf_result,e.first,e.second);
//			}
//		}
//		// makeing sure that all elements have been created in duf_result
//		//DUF::find(duf_result,cc.size()-1);
//
//		/* mapping root => vector in membership  */
//		std::unordered_map<unsigned,unsigned>cc_map;
//		for(unsigned i = 0; i < duf_result.size(); i++){
//			unsigned root = 0;
//			if(duf_result.at(i) < 0)
//			{
//				root = i;
//			}else{
//				root = DUF::find(duf_result,i);
//			}
//
//			if(cc_map.find(root) == cc_map.end())
//			{
//				membership.push_back(std::vector<unsigned>());
//				cc_map[root] = membership.size()-1;	
//			}
//			membership.at(cc_map[root]).push_back(i);
//		}
//	}
//
//
//	void BFS_cc(
//			std::vector<std::vector<unsigned>>& membership,
//			ConnectedComponent &cc,
//			const float threshold)
//	{
//		std::list<unsigned> nodes;
//		// fill list of nodes
//		for (unsigned i=1; i< cc.size();i++)
//		{
//			nodes.push_back(i);
//		}
//
//		membership.push_back(std::vector<unsigned>());
//
//		std::queue<unsigned> Q;
//		unsigned componentId = 0;
//		Q.push(0);
//		membership.at(componentId).push_back(0);
//		while(!nodes.empty()){
//
//			unsigned i = Q.front();
//			for (auto it = nodes.begin(); it != nodes.end();)
//			{
//				unsigned j = *it;
//				if(j != i)
//				{
//					if (cc.getCost(i,j,threshold) > 0)
//					{
//						Q.push(j);
//						membership.at(componentId).push_back(j);
//						it = nodes.erase(it);
//					}else{
//						++it;
//					}
//				}else{
//					++it;
//				}
//			}
//
//			Q.pop();
//
//			if(Q.empty())
//			{
//				if(!nodes.empty())
//				{
//					componentId++;
//					membership.push_back(std::vector<unsigned>());
//					Q.push(nodes.front());
//					membership.at(componentId).push_back(nodes.front());
//					nodes.pop_front();
//				}
//			}
//		}
//	}
//}
