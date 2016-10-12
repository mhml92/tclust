#ifndef DYNAMIC_UNION_FIND_HPP
#define DYNAMIC_UNION_FIND_HPP
#include <vector>

namespace DUF
{

	void funion(
			std::vector<int>& membership,
			int a,
			int b
			);

	int find(
			std::vector<int>& membership,
			int x);
}
#endif
