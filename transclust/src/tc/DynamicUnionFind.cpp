#include <limits>
#include <vector>
#include <iostream>
#include "transclust/DynamicUnionFind.hpp"

namespace DUF
{

	void funion(
			std::vector<long>& membership, 
			unsigned a, 
			unsigned b) 
	{
		if(a==b){
			return;
		}

		// get the roots of objects a and b
		long root_a = find(membership,a);
		long root_b = find(membership,b);

		// if they already have the same root - do nothing
		if (root_a == root_b) {
			membership.at(root_a) -= 1;
			return;
		}

		// If the root has not been used so far, we need to adjust the -1 one to
		// zero
		if(membership.at(root_a) == std::numeric_limits<long>::lowest()){
			membership.at(root_a) = 0;
		}
		if(membership.at(root_b) == std::numeric_limits<long>::lowest()){
			membership.at(root_b) =  0;
		}

		if(membership.at(root_b) < membership.at(root_a)) {
			// Combine the number of similarities of both branch and one
			// additional.
			membership.at(root_b) += (membership.at(root_a) -1);
			// Then, point the root of set a at set b.
			membership.at(root_a) =  root_b;
		} else {
			membership.at(root_a) += (membership.at(root_b) -1);
			membership.at(root_b) = root_a;
		}
	}

	/**
	 * find() finds the (int) name of the set containing a given element.
	 * Performs path compression along the way. When the element does not exist
	 * yet, it automatically adds all missing elements to the set.
	 * 
	 * @param x
	 * @return
	 */
	long find(
			std::vector<long>& membership,
			unsigned x) 
	{
		// Okay, the first time that this element is querried. Create all
		// 		// elements up to this one
		
		//if (x >= membership.size()) {
		//	for (unsigned i = membership.size(); i <= x; i++) {
		//		membership.push_back(std::numeric_limits<long>::lowest());
		//	}
		//	// As the element was freshly added, it cannot be already in a set.
		//	return x;
		//}
		if (membership.at(x) == std::numeric_limits<long>::lowest()) {
			return x;
		}


		if(membership.at(x) < 0) {
			return x; // x is the root of the tree; return it
		} else {
			// Find out who the root is; compress path by making the root
			// x's parent.
			membership.at(x) = find(membership,membership.at(x));
			return membership.at(x); // Return the root
		}
	}
}
