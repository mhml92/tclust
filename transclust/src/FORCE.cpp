#include <cmath> 
#include <iomanip> 
#include <iostream>
#include <random>
#include <limits>
#include <queue>
#include "ConnectedComponent.hpp"
#include "FindConnectedComponents.hpp"
#include "FORCE.hpp"
#include "ClusteringResult.hpp"

namespace FORCE{

	float dist(std::vector<std::vector<float>>& pos,unsigned i, unsigned j)
	{
		float res = 0.f;
		for(unsigned d = 0; d < pos[0].size(); d++){
			float side = pos[i][d] - pos[j][d]; 
			res += std::pow(side,2.f);
		}
		return std::sqrt(res);
	}

	void layout(
			const ConnectedComponent& cc,
			std::vector<std::vector<float>>& pos,
			float p,
			float f_att,
			float f_rep,
			unsigned R,
			float start_t,
			unsigned dim)
	{ 
		//std::cout << "id: " << cc.getId() << " threshold: " << cc.getThreshold() << " num nodes: " << cc.size() << std::endl;
		/*************************************************************************
		 * INITIAL LAYOUT
		 ************************************************************************/
		if(dim == 2)
		{
			//uniform 2d layout
			float radStep = (2*M_PI)/pos.size();
			float radVal = 0;
			for(unsigned i = 0; i < pos.size(); i++)
			{
				pos[i][0] = cos(radVal)*p;
				pos[i][1] = sin(radVal)*p;
				radVal += radStep;
			}
		}else{
			// uniform hsphere layout
			std::default_random_engine generator;
			std::uniform_real_distribution<float> distribution(-1.0,1.0);
			for(unsigned i = 0; i < pos.size(); i++)
			{
				float r = 0.f;
				for(unsigned d = 0; d < dim; d++)
				{
					pos[i][d] = distribution(generator);
					r += pow(pos[i][d],2.f);
				}
				r = std::sqrt(r);
				for(unsigned d = 0; d < dim; d++)
				{
					pos[i][d] = (pos[i][d]/r)*p;
				}
			}
		}
		
		/*************************************************************************
		 * MAIN LOOP
		 ************************************************************************/
		f_att /= cc.size();
		f_rep /= cc.size();
		std::vector<std::vector<float>> delta;
		for(unsigned r = 0; r < R; r++)
		{
			//DEBUG_position(cc,pos,r);
			delta.clear();
			delta.resize(pos.size(), std::vector<float>(dim,0));
			/**********************************************************************
			 * CALCULATE DELTA VECTOR
			 *********************************************************************/
			for(unsigned i = 0; i < pos.size();i++)
			{
				for(unsigned j = i+1; j < pos.size(); j++)
				{
					float dist_ij = dist(pos,i,j);
					if(dist_ij > 0)
					{
						float log_d = std::log(dist_ij+1);
						//float log_d = std::log(dist_ij+exp(1));
						//float log_d = dist_ij;

						float f_ij = 0;

						float edge_weight = cc.at(i,j);
						if(edge_weight > 0)
						{
							f_ij = (edge_weight*f_att*log_d)/dist_ij;
						}else{
							f_ij = ((edge_weight*f_rep)/(log_d))/dist_ij;
						}

						for(unsigned d = 0; d < dim; d++)
						{
							float f_vec = f_ij * (pos[j][d]-pos[i][d]);
							delta[i][d] += f_vec;
							delta[j][d] -= f_vec;
						}
					}
				}
			}
			for(unsigned i = 0; i < pos.size();i++)
			{
				// float length = 0;
				for(unsigned d = 0; d < dim;d++)
				{
					pos[i][d] += delta[i][d];
				}
				//   std::cout << std::setw(10) << i << ": "<< sqrt(length) << std::endl;
			}
			//DEBUG_position(cc,pos,r);
			/**********************************************************************
			 * APPLY COOLING FUNCTION
			 *********************************************************************/
			/*
			float m = (start_t*(float)cc.size())/(pow(r+1,2));
			for(unsigned i = 0; i < pos.size(); i++){

				float len = 0;
				for(unsigned j = 0; j < dim;j++){
					len += pow(delta[i][j],2);
				}
				len = sqrt(len);
				float scalar = std::min(m,len);
				if(scalar != 0){
					for(unsigned j = 0; j < dim;j++){
						delta[i][j] = (delta[i][j]/len)*scalar;
					}
				}else{
					for(unsigned j = 0; j < dim;j++){
						delta[i][j] = 0;
					}
				}
			}
			*/

			//DEBUG_position(cc,pos,r);
			//DEBUG_delta(cc,pos,delta,r);
		} 
	}

	void partition(
			const ConnectedComponent& cc,
			std::vector<std::vector<float>>& pos,
			ClusteringResult& cr,
			float d_init,
			float d_maximal,
			float s_init,
			float f_s)
	{ 


		float d = d_init;
		float s = s_init;
		std::vector<float> D;
		while(d <= d_maximal){
			D.push_back(d);
			LOG_DEBUG << "THE D: " << d;
			d += s;
			s += s*f_s;
		}
		cr.cost = std::numeric_limits<float>::max();

		std::vector<std::vector<unsigned>> clustering;

		std::vector<unsigned> dummy(cc.size());
		std::iota(dummy.begin(),dummy.end(),0);
		clustering.push_back(dummy);

		for(std::vector<float>::reverse_iterator it = D.rbegin(); it != D.rend(); ++it) {
			clustering = geometricLinking(pos,*it,clustering);
			//LOG_DEBUG << "CLUSTERING";
			//for(auto& cluster:clustering){
			//	for(unsigned i = 0; i < cluster.size(); i++){
			//		std::cout << cluster[i] << ", ";
			//	}
			//	std::cout << std::endl;
			//}
			

			std::vector<unsigned> membership(cc.size(),std::numeric_limits<unsigned>::max());
			unsigned clusterId = 0;
			for(auto& cluster:clustering){
				for(unsigned i = 0; i < cluster.size(); i++){
					membership.at(cluster.at(i)) = clusterId;
				}
				clusterId++;
			}
			//for(unsigned i = 0; i < membership.size(); i++){
			//	std::cout << membership[i] << ", ";
			//}
			//std::cout << std::endl;

			float cost = 0;
			for(unsigned i = 0; i< membership.size(); i++)
			{
				for(unsigned j = i+1; j < membership.size(); j++)
				{
					if(membership.at(i) != membership.at(j))
					{
						if(cc.at(i,j,false) > 0)
						{
							cost += cc.at(i,j,false);
						}
					}else if(cc.at(i,j,false) < 0){
						cost -= cc.at(i,j,false);
					}
				}
			}
			LOG_DEBUG << "cluster cost: " <<cost << " at: " << *it;
			if(cost < cr.cost)
			{
				LOG_DEBUG << "new cluster cost: " <<cost << " at: " << *it;
				cr.cost = cost;
				cr.membership = membership;
			}

		}
	}

	/*******************************************************************************
	 * DETERMINE MEMBERSHIP IN POS 
	 ******************************************************************************/
	std::vector<std::vector<unsigned>> geometricLinking(
			std::vector<std::vector<float>>& pos,
			const float maxDist,
			const std::vector<std::vector<unsigned>>& objects)
	{
		std::vector<std::vector<unsigned>> result;
		unsigned componentId = 0;

		for(auto& obj:objects)
		{
			std::vector<unsigned> membership(obj.size(),std::numeric_limits<unsigned>::max());

			result.push_back(std::vector<unsigned>());
			std::queue<unsigned> Q;
			Q.push(0);
			membership.at(0) = componentId;
			result.at(componentId).push_back(obj.at(0));
			while(true)
			{
				unsigned i = Q.front();
				Q.pop();

				for(unsigned j = 0; j < obj.size();j++)
				{
					if(membership.at(j) == std::numeric_limits<unsigned>::max())
					{
						LOG_DEBUG << "dist i,j: " << i <<"( "<< obj.at(i)<<" ), " << j<<"( "<< obj.at(j)<<" ), " << " = "  <<dist(pos,obj.at(i),obj.at(j));
						if( dist(pos,obj.at(i),obj.at(j)) <= maxDist)
						{
							Q.push(j);
							membership.at(j) = componentId;
							result.at(componentId).push_back(obj.at(j));
						}
					}
				}

				if(Q.empty())
				{
					componentId++;
					// could be done smarter by by saving 'last known nonmember'
					for(unsigned s = 0; s < membership.size();s++){
						if(membership.at(s) == std::numeric_limits<unsigned>::max())
						{
							Q.push(s);
							result.push_back(std::vector<unsigned>());
							membership.at(s) = componentId;
							result.at(componentId).push_back(obj.at(s));
							break;
						}
					}
					if(Q.empty())
					{
						break;
					}
				}
			}
		}
		return result;
	}
}
