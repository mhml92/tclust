#ifndef DYNAMIC_UNION_FIND_HPP
#define DYNAMIC_UNION_FIND_HPP
#include <vector>

namespace DUF
{

	void funion(
			std::vector<long>& membership,
			unsigned a,
			unsigned b
			);

	long find(
			std::vector<long>& membership,
			unsigned x);
}
#endif
