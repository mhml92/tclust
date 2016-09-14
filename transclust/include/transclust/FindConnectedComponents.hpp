#ifndef FINDCONNECTEDCOMPONENTS_HPP
#define FINDCONNECTEDCOMPONENTS_HPP
#include <vector>
#include <queue> 
#include "transclust/ConnectedComponent.hpp"

namespace FCC{

	//double dist(std::vector<std::vector<double>>& pos,unsigned i, unsigned j);

	/****************************************************************************
	 * FIND CCs IN CC W. THRESHOLD
	 ***************************************************************************/
	void findConnectedComponents(
			const ConnectedComponent &cc, 
			std::queue<ConnectedComponent> &ccs,
			const double threshold);

	/****************************************************************************
	 * DETERMINE MEMBERSHIP BASED ON CC
	 ***************************************************************************/
	std::vector<std::vector<unsigned>> findMembershipVector(
			const ConnectedComponent &cc, 
			const double threshold);

	/****************************************************************************
	 * FIND CCs IN POS W. THRESHOLD
	 ***************************************************************************/
	void findConnectedComponents(
			const ConnectedComponent &cc, 
			const std::vector<std::vector<double>>& pos,
			std::vector<ConnectedComponent> &ccs,
			const double threshold);

	/****************************************************************************
	 * DETERMINE MEMBERSHIP BASED ON POS 
	 ***************************************************************************/
	std::vector<std::vector<unsigned>> findMembershipVector(
			const ConnectedComponent& cc,
			const std::vector<std::vector<double>>& pos,
			std::vector<ConnectedComponent>& ccs,
			const double threshold);

}
#endif
