#include <limits>
#include <map>
#include <unordered_map>
#include <iomanip>
#include <utility>
#include <boost/unordered_map.hpp>
#include "transclust/Result.hpp"

//Result::Result(std::deque<std::string>& id2name)
//{ }
//
//void Result::add(ConnectedComponent& cc, ClusteringResult& cr)
//{
//
//	// update cost
//	total_cost += cr.cost;
//
//	/////////////////////////////////////////////////////////////////////////////
//	// update membership
//	/////////////////////////////////////////////////////////////////////////////
//
//	// mapping the internal object ids of the connected component to the global
//	// objects of the dataset
//	boost::unordered_map<unsigned,unsigned> local2global_map;
//
//	// add each obejct to the global membership 
//	for(unsigned i = 0; i < cr.membership.size(); i++)
//	{
//		// get the local cluster id of the connected component
//		unsigned lcid = membership.at(i);
//
//		// add local cluster id to map if first apperance
//		if(local2global_map.find(lcid) == local2global_map.end())
//		{
//			local2global_map.insert(std::make_pair(lcid,global_cluster_id));
//			global_cluster_id++;
//		}
//
//		// get global cluster id
//		unsigned gcid = local2global_map[lcid];
//
//		membership.at(cc.getObjectId(i)) = gcid;
//	}
//
//	/*
//	// update cost
//	if(cost.find(cc.getThreshold()) == cost.end())
//	{
//		cost.insert(std::make_pair(cc.getThreshold(),0));
//	}
//	cost.at(cc.getThreshold()) += cr.cost;
//
//	// update clusters
//	std::vector<std::vector<unsigned>> clstrs;
//
//	std::unordered_map<unsigned,unsigned> clstr_index;
//
//	for(unsigned i = 0; i < cr.membership.size(); i++)
//	{
//		unsigned clster_num = cr.membership.at(i);
//
//		// if cluster number is not present in map
//		if(clstr_index.find(clster_num) == clstr_index.end())
//		{
//			clstr_index.insert(std::make_pair(clster_num,clstrs.size()));
//			clstrs.push_back(std::vector<unsigned>());
//		}
//		unsigned ci = clstr_index.at(clster_num);
//		unsigned objId = cc.getObjectId(i);
//		clstrs.at(ci).push_back(objId);
//	}
//
//	// check if threshold exists in map
//	if(clusters.find(cc.getThreshold()) == clusters.end())
//	{
//		clusters.insert(std::make_pair(cc.getThreshold(),std::vector<std::vector<unsigned>>()));
//	}
//
//	for(auto& clstr:clstrs)
//	{
//		clusters.at(cc.getThreshold()).push_back(clstr);
//	}
//	*/
//}
//
//clustering Result::get(){
//	/*
//	clustering res;
//
//	res.id2object = id2object;
//
//	for(auto& c:cost)
//	{
//		res.threshold.push_back( c.first );
//		res.cost.push_back( c.second );
//
//		std::vector< unsigned > _clusters;
//		_clusters.resize(id2object.size(),std::numeric_limits<unsigned>::max());
//
//		unsigned cid = 0;
//		for(auto& clstr:clusters.at(c.first))
//		{
//			//std::vector<unsigned> cluster;
//			for(unsigned oid:clstr)
//			{
//				_clusters.at(oid) = cid;
//			}
//		   cid++;
//			//_clusters.push_back(cluster);
//		}
//		res.clusters.push_back(_clusters);
//	}
//	return res;
//	*/
//}
//
//void Result::dump()
//{
//
//	/*
//	if(cost.size() > 0 ){
//		for(auto& c:cost)
//		{
//			float threshold = c.first;
//			float cost = c.second;
//			std::cout << threshold << "\t";
//			std::cout << cost << "\t";
//			std::string output = "";
//
//			unsigned count_objects = 0;
//			for(auto& clstr:clusters.at(threshold))
//			{
//				for(auto& oid:clstr)
//				{
//					output += id2object[oid] + ",";
//					count_objects++;
//				}
//				output.pop_back();
//				output += ";";
//			}
//
//			std::cout << output << std::endl;
//		}
//	}
//	*/
//}
