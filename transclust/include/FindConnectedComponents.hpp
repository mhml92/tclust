#ifndef FINDCONNECTEDCOMPONENTS_HPP
#define FINDCONNECTEDCOMPONENTS_HPP
#include <vector>
#include <queue> 
#include "ConnectedComponent.hpp"

namespace FCC{

	//float dist(std::vector<std::vector<float>>& pos,unsigned i, unsigned j);

	/****************************************************************************
	 * FIND CCs IN CC W. THRESHOLD
	 ***************************************************************************/
	void findConnectedComponents(
			const ConnectedComponent &cc, 
			std::queue<ConnectedComponent> &ccs,
			const float threshold);

	/****************************************************************************
	 * DETERMINE MEMBERSHIP BASED ON CC
	 ***************************************************************************/
	std::vector<std::vector<unsigned>> findMembershipVector(
			const ConnectedComponent &cc, 
			const float threshold);

	/****************************************************************************
	 * FIND CCs IN POS W. THRESHOLD
	 ***************************************************************************/
	void findConnectedComponents(
			const ConnectedComponent &cc, 
			const std::vector<std::vector<float>>& pos,
			std::vector<ConnectedComponent> &ccs,
			const float threshold);

	/****************************************************************************
	 * DETERMINE MEMBERSHIP BASED ON POS 
	 ***************************************************************************/
	std::vector<std::vector<unsigned>> findMembershipVector(
			const ConnectedComponent& cc,
			const std::vector<std::vector<float>>& pos,
			std::vector<ConnectedComponent>& ccs,
			const float threshold);

}
#endif
