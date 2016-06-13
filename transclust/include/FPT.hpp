#ifndef FPT_HPP
#define FPT_HPP
#include <iomanip>
#include <vector>
#include <cmath>
#include <list>
#include <iostream>
#include <string>
#include "ConnectedComponent.hpp"
#include "ClusteringResult.hpp"
#include <limits>
#include <plog/Log.h>

namespace FPT{

	struct Node {
		std::vector<std::vector<unsigned>> nodeParents;
		std::vector<std::vector<float>> edgeCost;
		//std::vector<std::vector<bool>> forbidden;
		unsigned size;
		float cost;
	};

	/****************************************************************************
	 * DEBUG
	 ***************************************************************************/

	inline void dumpCost(Node& fptn){
		std::cout << "cost: " << fptn.cost << std::endl;
	}
	inline void dumpCR(ClusteringResult& cr)
	{
		std::cout << "cost: " << cr.cost << std::endl;
		for(unsigned i = 0; i < cr.membership.size(); i++)
		{
			std::cout << std::setw(5) << std::fixed << std::setprecision(1) << cr.membership.at(i);
		}
		std::cout << std::endl;
	}
	inline void dumpCC(ConnectedComponent& cc)
	{

		for(unsigned i = 0; i < cc.size();i++){
			for(unsigned j = 0; j < cc.size();j++){
				if(i == j){
				std::cout << std::setw(5) << std::fixed << std::setprecision(1)  << "-.-";
				
				}else{
				std::cout << std::setw(5) << std::fixed << std::setprecision(1)  <<  cc.at(i,j);
				}
			}
			std::cout << "\n" << std::endl;
		}
		std::cout << "\n" << std::endl;
	}

	inline void dumpFPTN(Node& fptn)
	{
		/*
		std::cout << "FTPN-----------------------------------------" << std::endl;

		std::cout << "cost: " << fptn.cost << std::endl;

		std::cout << "nodeParents" << std::endl;
		for(unsigned i = 0; i < fptn.nodeParents.size();i++)
		{
			for(unsigned j = 0; j < fptn.nodeParents.at(i).size(); j++)
			{
				std::cout << std::setw(5) << fptn.nodeParents.at(i).at(j);
			}
			std::cout << std::endl;
		}
		std::cout << "\ncost matrix" << std::endl;

		for(unsigned i = 0; i < fptn.edgeCost.size();i++){
			for(unsigned j = 0; j < fptn.edgeCost.at(i).size();j++){
				std::string val = "";
				if(fptn.edgeCost.at(i).at(j) == std::numeric_limits<float>::max())
				{
					val = "+Inf";
					std::cout << std::setw(5)  <<  val;
				}
				else if(fptn.edgeCost.at(i).at(j) == std::numeric_limits<float>::min())
				{
					val = "-Inf";
					std::cout << std::setw(5)  <<  val;
				}
				else
				{
					float v = fptn.edgeCost.at(i).at(j);
					std::cout << std::setw(5) << std::setprecision(2)  << v;
							
				}
			}
			std::cout << "\n" << std::endl;
		}
		std::cout << "---------------------------------/FPTN\n" << std::endl;
	*/
	}
	/****************************************************************************
	 ***************************************************************************/

	void cluster(
			ConnectedComponent& cc,
			ClusteringResult& cr,
			float time_limit,
			float maxK,
			float stepSize
			);

	void find_solution(
			ConnectedComponent& cc,
			ClusteringResult& cr,
			Node& fptn0,
			float K);

	void reduce(
			ConnectedComponent& cc,
			ClusteringResult& cr,
			Node& fptn,
			float K);

	float costSetForbidden(
			Node& fptn, 
			unsigned node_i,
			unsigned node_j);

	float costSetPermanent(
			Node& fptn, 
			unsigned node_i,
			unsigned node_j);

	void mergeNodes(
			Node& fptn, 
			unsigned i,
			unsigned j, 
			float costForMerging);

	void setForbiddenAndFindSolution(
			ConnectedComponent& cc,
			ClusteringResult& cr,
			Node& fptn,
			float K,
			unsigned node_i, 
			unsigned node_j, 
			float costsForSetForbidden);

	void buildSolution(
			ConnectedComponent& cc,
			ClusteringResult& cr, 
			Node& fptn);

	void clone_node(Node& fptn0,Node& fptn1);

}
#endif
