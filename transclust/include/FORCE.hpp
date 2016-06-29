#ifndef FORCE_HPP
#define FORCE_HPP
#include <vector>
#include <math.h>
#include <iostream>
#include "ConnectedComponent.hpp"
#include "ClusteringResult.hpp"

namespace FORCE{


	float dist(std::vector<std::vector<float>>& pos,unsigned i, unsigned j);

	void layout(
			const ConnectedComponent& cc,
			std::vector<std::vector<float>>& pos,
			float p,
			float f_att,
			float f_rep,
			unsigned R,
			float start_t,
			unsigned dim);

	void partition(
			const ConnectedComponent& cc,
			std::vector<std::vector<float>>& pos,
			ClusteringResult& cs,
			float d_init,
			float d_maximal,
			float s_init,
			float f_s);


	std::vector<std::vector<unsigned>> geometricLinking(
			std::vector<std::vector<float>>& pos,
			const float maxDist,
			const std::vector<std::vector<unsigned>>& objects);

	inline void DEBUG_delta(
			const ConnectedComponent& cc,
			std::vector<std::vector<float>>& pos,
			std::vector<std::vector<float>>& delta,
			unsigned r)
	{
		for(unsigned i = 0; i < pos.size(); i++){
			float len = 0;
			for(unsigned j = 0; j < pos[0].size();j++){
				len += pow(delta[i][j],2);
			}
			len = sqrt(len);
			std::cout << cc.getThreshold() << "\t" << r << "\t" << len << "\t" << std::endl;
		}
	}

	inline void DEBUG_position(
			const ConnectedComponent& cc,
			std::vector<std::vector<float>>& pos,
			float r)
	{
		for(unsigned i = 0; i < pos.size();i++){
			std::cout << cc.getThreshold() << "\t" << r << "\t";
			for(unsigned d = 0;d < pos[0].size();d++){
				std::cout << pos[i][d] << "\t";
			}
			std::cout << std::endl;
		}
	}

	inline void DEBUG_linking(
			std::vector<unsigned> membership,
			std::vector<std::vector<float>> pos,
			float threshold,
			unsigned id)
	{

		//std::cout << "cluster\tx\ty\tdist" << std::endl;
		for(unsigned i = 0; i < membership.size(); i++){
			std::cout << membership.at(i) << "\t" << pos.at(i)[0] << "\t" << pos.at(i)[1] << "\t" << threshold << "\t" << id << std::endl;
		}
	}

}
#endif
