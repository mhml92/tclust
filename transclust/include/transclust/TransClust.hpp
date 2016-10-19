#ifndef TRANSCLUST_HPP
#define TRANSCLUST_HPP
#include <string>
#include <queue>
#include <vector>
#include <map>
#include <cmath>
#include "transclust/Common.hpp"
#include "transclust/TriangularMatrix.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/ClusteringResult.hpp"
#include "transclust/Result.hpp"

class TransClust{

	public:
		TransClust(
				const std::string& filename,
				TCC::TransClustParams& _tcp
				);

		TransClust(
				std::vector<float>& sim_matrix_1d,
				unsigned num_o,
				TCC::TransClustParams& _tcp
				);

		clustering cluster();
	private:
		TCC::TransClustParams tcp;

		std::queue<ConnectedComponent> ccs;
		std::vector<std::string> id2object;
};
#endif
