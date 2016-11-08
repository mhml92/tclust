#ifndef DYNAMIC_UNION_FIND_HPP
#define DYNAMIC_UNION_FIND_HPP
#include <deque>

class DynamicUnionFind
{
	public:
		DynamicUnionFind();
		inline std::deque<long>& getMembershipVector(){return membership;}
		inline unsigned size() {return membership.size();}
		inline long at(unsigned i){return membership.at(i);}
		inline void reset(){membership.clear();}

		void funion(unsigned a,unsigned b);

		unsigned find(unsigned x);
	private:
		std::deque<long> membership;
};
#endif
