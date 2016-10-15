#ifndef FINDCONNECTEDCOMPONENTS_HPP
#define FINDCONNECTEDCOMPONENTS_HPP
#include <vector>
#include <queue> 
#include "transclust/Common.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/DynamicUnionFind.hpp"

namespace FCC{

	/****************************************************************************
	 * FIND CCs IN CC W. THRESHOLD
	 ***************************************************************************/
	void findConnectedComponents(
			TCC::TransClustParams& tcp,
			ConnectedComponent &cc, 
			std::queue<ConnectedComponent> &ccs,
			const float threshold);

	/* Breadth-first search */
	void BFS_cc(
			std::vector<std::vector<unsigned>>& membership,
			ConnectedComponent &cc, 
			const float threshold);

	/* Dynamic Union Find  */
	void DUF_cc(
			std::vector<std::vector<unsigned>>& membership,
			ConnectedComponent &cc, 
			const float threshold);

}
#endif
