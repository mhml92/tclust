#ifndef FORCE_HPP
#define FORCE_HPP
#include <vector>
#include <iostream>
#include "transclust/ConnectedComponent.hpp"
#include "transclust/ClusteringResult.hpp"

namespace FORCE{


	double dist(std::vector<std::vector<double>>& pos,unsigned i, unsigned j);

	void layout(
			const ConnectedComponent& cc,
			std::vector<std::vector<double>>& pos,
			double p,
			double f_att,
			double f_rep,
			const unsigned R,
			double start_t,
			const unsigned dim,
			unsigned seed = 42);


	/*
	void binarySearchPartition(
			const ConnectedComponent& cc,
			std::vector<std::vector<double>>& pos,
			ClusteringResult& cr);
			*/


	void partition(
			const ConnectedComponent& cc,
			std::vector<std::vector<double>>& pos,
			ClusteringResult& cs,
			double d_init,
			double d_maximal,
			double s_init,
			double f_s);


	std::vector<std::vector<unsigned>> geometricLinking(
			std::vector<std::vector<double>>& pos,
			const double maxDist,
			const std::vector<std::vector<unsigned>>& objects);

	inline void DEBUG_delta(
			const ConnectedComponent& cc,
			std::vector<std::vector<double>>& pos,
			std::vector<std::vector<double>>& delta,
			unsigned r)
	{
		for(unsigned i = 0; i < pos.size(); i++){
			double len = 0;
			for(unsigned j = 0; j < pos[0].size();j++){
				len += pow(delta[i][j],2);
			}
			len = sqrt(len);
			std::cout << cc.getThreshold() << "\t" << r << "\t" << len << "\t" << std::endl;
		}
	}

	inline void DEBUG_position(
			const ConnectedComponent& cc,
			std::vector<std::vector<double>>& pos,
			double r)
	{
		//std::cout << "threshold\tr\tx\ty" << std::endl;
		for(unsigned i = 0; i < pos.size();i++){
			std::cout << cc.getId() << "\t" << cc.getThreshold() << "\t" << r << "\t";
			for(unsigned d = 0;d < pos[0].size();d++){
				std::cout << pos[i][d] << "\t";
			}
			std::cout << std::endl;
		}
	}

	inline void DEBUG_linking(
			std::vector<unsigned> membership,
			std::vector<std::vector<double>> pos,
			double threshold,
			unsigned id)
	{

		//std::cout << "cluster\tx\ty\tdist" << std::endl;
		for(unsigned i = 0; i < membership.size(); i++){
			std::cout << membership.at(i) << "\t" << pos.at(i)[0] << "\t" << pos.at(i)[1] << "\t" << threshold << "\t" << id << std::endl;
		}
	}

}
#endif
