#ifndef FORCE_HPP
#define FORCE_HPP
#include <vector>
#include <iostream>
#include "transclust/Common.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/ClusteringResult.hpp"

namespace FORCE{
	float dist(std::vector<std::vector<float>>& pos,unsigned i, unsigned j);

	void layout(
			ConnectedComponent& cc,
			std::vector<std::vector<float>>& pos,
			float p,
			float f_att,
			float f_rep,
			const unsigned R,
			float start_t,
			const unsigned dim,
			unsigned seed);

	void layout_parallel(
			ConnectedComponent& cc,
			std::vector<std::vector<float>>& pos,
			float p,
			float f_att,
			float f_rep,
			const unsigned R,
			float start_t,
			const unsigned dim,
			unsigned seed);

	float calculate_force(
			float edge_weight,
			float f_att,
			float f_rep,
			float distance,
			unsigned size
			);

	void partition(
			ConnectedComponent& cc,
			std::vector<std::vector<float>>& pos,
			RES::ClusteringResult& cs,
			float d_init,
			float d_maximal,
			float s_init,
			float f_s);

	std::deque<std::deque<unsigned>> geometricLinking(
			std::vector<std::vector<float>>& pos,
			float maxDist,
			std::deque<std::deque<unsigned>>& objects);

	void initial_layout(
			std::vector<std::vector<float>>& pos,
			unsigned dim,
			float p,
			unsigned seed);

	void inline DEBUG_force(std::vector<std::vector<float>>& pos,int iter){

		if(pos.size() == 91){
			for(unsigned i = 0; i < pos.size();i++){
				std::cout << iter+1 << ",";
				for(unsigned j = 0; j < pos[0].size();j++){
					std::cout  << pos.at(i).at(j) << ",";	
				}
				std::cout << std::endl;
			}
		}
	}
}
#endif
