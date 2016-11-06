#ifndef TRANSCLUST_HPP
#define TRANSCLUST_HPP
#include <string>
#include <queue>
#include <deque>
#include <vector>
#include <map>
#include <cmath>
#include <plog/Log.h>
#include "transclust/Common.hpp"
#include "transclust/InputParser.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/ClusteringResult.hpp"
#include "transclust/Result.hpp"

class TransClust{

	public:
		TransClust(
				const std::string& filename,
				TCC::TransClustParams& _tcp
				);

		clustering cluster();
	private:
		TCC::TransClustParams tcp;
		InputParser ip;
		std::deque<ConnectedComponent> ccs;
		std::deque<std::string> id2name; 
};
#endif
