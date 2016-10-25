#ifndef DEBUG_HPP
#define DEBUG_HPP
#include <vector>
#include <queue>
#include "transclust/ConnectedComponent.hpp"

namespace DEBUG
{
	inline void geometricLinking(
			std::vector<std::vector<unsigned>>& clustering, 
			std::vector<std::vector<float>>& pos, 
			float distance)
	{
		//std::cout << "Debug geometricLinking" << std::endl;

		for(unsigned i = 0; i < clustering.size(); i++){
			// test that each connected component IS connected
			std::vector<bool> assigned(clustering.at(i).size(),false);
			std::queue<unsigned> Q;
			Q.push(0);
			assigned.at(0) = true;
			while(!Q.empty()){
				for(unsigned ei = 0; ei < clustering.at(i).size(); ei++){
					if( !assigned.at(ei) ){
						if( TCC::dist(pos, clustering.at(i).at(Q.front()), clustering.at(i).at(ei) ) <= distance){
							assigned.at(ei) = true;
							Q.push(ei);
						}
					}
				}
				Q.pop();
			}
			for(unsigned ei = 0; ei < assigned.size(); ei++){
				if(!assigned.at(ei)){
					std::cout << "[ERROR]" << __FILE__ << " at line: " << __LINE__ << std::endl;
				}
			}

			// test that for each member in the i'th ccv there is no member in
			// the j'th ccv which has similarity > threshold
			for(unsigned j = i+1; j < clustering.size(); j++){
				for(auto& i_ccv:clustering.at(i))
				{
					for(auto& j_ccv:clustering.at(j))
					{
						if(TCC::dist(pos,i_ccv,j_ccv)  <= distance){
							std::cout << "[ERROR]" << __FILE__ << " at line: " << __LINE__ << std::endl;
						}
					}

				}
			}
		}
	}

	inline void findConnectedComponents(
			std::vector<std::vector<unsigned>>& membership, 
			ConnectedComponent& cc, 
			float threshold)
	{
		//std::cout << "Debug findConnectedComponents" << std::endl;

		for(unsigned i = 0; i < membership.size(); i++){
			// test that each connected component IS connected
			std::vector<bool> assigned(membership.at(i).size(),false);
			std::queue<unsigned> Q;
			Q.push(0);
			assigned.at(0) = true;
			while(!Q.empty()){
				for(unsigned ei = 0; ei < membership.at(i).size(); ei++){
					if( !assigned.at(ei) ){
						if( cc.at( membership.at(i).at(Q.front()), membership.at(i).at(ei) ) > 0){
							assigned.at(ei) = true;
							Q.push(ei);
						}
					}
				}
				Q.pop();
			}
			for(unsigned ei = 0; ei < assigned.size(); ei++){
				if(!assigned.at(ei)){
					std::cout << "[ERROR]" << __FILE__ << " at line: " << __LINE__ << std::endl;
				}
			}

			// test that for each member in the i'th ccv there is no member in
			// the j'th ccv which has similarity > threshold
			for(unsigned j = i+1; j < membership.size(); j++){
				for(auto& i_ccv:membership.at(i))
				{
					for(auto& j_ccv:membership.at(j))
					{
						if((cc.getMatrix().get(i_ccv,j_ccv) - threshold) > 0){
							std::cout << "[ERROR]" << __FILE__ << " at line: " << __LINE__ << std::endl;
						}
					}

				}
			}
		}
	}

	inline void round(float num){
		if(std::isnan(num)){
			std::cout << "Number is NAN" << std::endl;
		}
	}


}
#endif
