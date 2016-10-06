#ifndef FORCE_HPP
#define FORCE_HPP
#include <vector>
#include <iostream>
#include "transclust/Common.hpp"
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
}
#endif
