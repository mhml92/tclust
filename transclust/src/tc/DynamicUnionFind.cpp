#include <limits>
#include <vector>
#include <iostream>
#include "transclust/DynamicUnionFind.hpp"

namespace DUF
{

	void funion(
			std::vector<int>& membership, 
			int a, 
			int b) 
	{
		if(a==b){
			return;
		}

		// get the roots of objects a and b
		int root_a = find(membership,a);
		int root_b = find(membership,b);

		// if they already have the same root - do nothing
		if (root_a == root_b) {
			return;
		}

		if(membership.at(root_b) < membership.at(root_a)) {
			// Combine the number of similarities of both branch and one
			// additional.
			membership.at(root_b) += membership.at(root_a);
			// Then, point the root of set a at set b.
			membership.at(root_a) =  root_b;
		} else {
			membership.at(root_a) += membership.at(root_b);
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
	int find(
			std::vector<int>& membership,
			int x) 
	{
		// Okay, the first time that this element is querried. Create all
		// 		// elements up to this one
		if (x >= membership.size()) {
			for (unsigned i = membership.size(); i <= x; i++) {
				membership.push_back(-1);
			}
			// As the element was freshly added, it cannot be already in a set.
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

	/**
	 * Returns the number of similarities of the connected component of x.
	 * 
	 * @param x
	 * @return
	 */
	int numberOfSimilarities(std::vector<int>& membership,int x) {
		int ret = membership.at(find(membership,x));
		if (ret == std::numeric_limits<int>::lowest()){
			ret = 0;
		}
		return -ret;
	}
}
