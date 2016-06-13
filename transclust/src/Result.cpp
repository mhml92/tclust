#include "Result.hpp"
#include <limits>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <iomanip>

Result::Result(std::vector<std::string> id2object)
	:
		id2object(id2object)

{ }

void Result::add(ConnectedComponent& cc, ClusteringResult& cr)
{
	// update cost
	if(cost.find(cc.getThreshold()) == cost.end())
	{
		cost.insert({cc.getThreshold(),0});
	}
	cost.at(cc.getThreshold()) += cr.cost;

	// update clusters
	std::vector<std::vector<unsigned>> clstrs;
	std::unordered_map<unsigned,unsigned>clstr_index;
	for(unsigned i = 0; i < cr.membership.size(); i++)
	{
		unsigned clster_num = cr.membership.at(i);
		// if cluster number is not present in map
		if(clstr_index.find(clster_num) == clstr_index.end())
		{
			clstr_index.insert({clster_num,clstrs.size()});
			clstrs.push_back(std::vector<unsigned>());
		}
		unsigned ci = clstr_index.at(clster_num);
		unsigned objId = cc.getObjectId(i);
		clstrs.at(ci).push_back(objId);
	}

	// check if threshold exists in map
	if(clusters.find(cc.getThreshold()) == clusters.end())
	{
		clusters.insert({cc.getThreshold(),std::vector<std::vector<unsigned>>()}); 
	}

	for(auto& clstr:clstrs)
	{
		clusters.at(cc.getThreshold()).push_back(clstr);
	}
}

void Result::dump()
{
	for(auto& c:cost)
	{
		float threshold = c.first;
		float cost = c.second;
		std::cout << threshold << "\t";
		std::cout << cost << "\t";
		std::string output = "";
		for(auto& clstr:clusters.at(threshold))
		{
			for(auto& oid:clstr)
			{
				output += id2object[oid] + ",";
			}
			output.pop_back();
			output += ";";
		}
		std::cout << output << std::endl;
	}
}
