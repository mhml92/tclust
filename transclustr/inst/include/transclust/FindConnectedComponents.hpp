#ifndef FINDCONNECTEDCOMPONENTS_HPP
#define FINDCONNECTEDCOMPONENTS_HPP
#include <vector>
#include <queue>
#include "transclust/Common.hpp"
#include "transclust/ConnectedComponent.hpp"

namespace FCC{

	/****************************************************************************
	 * FIND CCs IN CC W. THRESHOLD
	 ***************************************************************************/
	void findConnectedComponents(
			TCC::TransClustParams& tcp,
			const ConnectedComponent &cc,
			std::queue<ConnectedComponent> &ccs,
			const double threshold);

	/* Breadth-first search */
	void BFS_cc(
			std::vector<std::vector<unsigned>>& membership,
			const ConnectedComponent &cc,
			const double threshold);
}
#endif
