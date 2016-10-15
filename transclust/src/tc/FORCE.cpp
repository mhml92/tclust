#include <iomanip>
#include <iostream>
#include <random>
#include <limits>
#include <list>
#include <queue>
#include <algorithm>
#include <chrono>
#ifdef _OPENMP
#	include <omp.h>
#endif
#ifndef NDEBUG
#	include "transclust/DEBUG.hpp"
#	define DEBUG_GM(membership, cc, threshold) DEBUG::geometricLinking(membership,cc,threshold)
#else
#	define DEBUG_GM(membership, cc, threshold) {}
#endif
#include "transclust/Common.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/FindConnectedComponents.hpp"
#include "transclust/FORCE.hpp"
#include "transclust/ClusteringResult.hpp"
namespace FORCE
{

	void layout(
			ConnectedComponent& cc,
			std::vector<std::vector<float>>& pos,
			float p,
			float f_att,
			float f_rep,
			unsigned R,
			float start_t,
			unsigned dim,
			unsigned seed
)
	{
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
			//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			std::mt19937 generator(seed);
			std::uniform_real_distribution<float> distribution(-1.0,1.0);

			for(unsigned i = 0; i < pos.size(); i++)
			{
				float r = 0;
				for(unsigned d = 0; d < dim; d++)
				{
					pos[i][d] = distribution(generator);
					r += pos[i][d] * pos[i][d];
				}
				r = std::sqrt(r);

				for(unsigned d = 0; d < dim; d++)
				{
					pos[i][d] = (pos[i][d]/r)*p;
				}
			}
		}
		//DEBUG_force(pos,-1);
		/*************************************************************************
		 * MAIN LOOP
		 ************************************************************************/
		std::vector<std::vector<float>> delta;
		delta.resize(pos.size(), std::vector<float>(dim,0.0f));

		f_att /= static_cast<float>(cc.size());
		f_rep /= static_cast<float>(cc.size());

		for(unsigned r = 0; r < R; r++)
		{
			// zero delta vectors
			for(auto& v:delta){std::fill(v.begin(),v.end(),0.0f);}


			float temp = (start_t*static_cast<float>(cc.size()))*std::pow((1.0f/(r+1.0f)),2);
			/**********************************************************************
			 * CALCULATE DELTA VECTOR
			 *********************************************************************/
			//float negative_force = 0;
			//float positive_force = 0;
			#pragma omp parallel
			{
				// create local copy
				std::vector< std::vector<float> > _delta = delta;

				#pragma omp for schedule(dynamic)
				for(unsigned i = 0; i < pos.size();i++)
				{
					for(unsigned j = i+1; j < pos.size(); j++)
					{
						float _distance = TCC::dist(pos,i,j);
						if(_distance > 0.0f)
						{
							float log_d = std::log(_distance+1);

							float force = 0.0f;

							// normalized edge weight
							float edge_weight = cc.at(i,j);
							if(edge_weight > 0)
							{
								force = (edge_weight * f_att * log_d)/_distance;
								//#pragma omp atomic
								//positive_force += force;
							}else{
								force = (edge_weight * f_rep)/log_d/_distance;
								//#pragma omp atomic
								//negative_force += force;
							}
							for(unsigned d = 0; d < dim; d++)
							{
								float displacement = (force * (pos[j][d]-pos[i][d]));
								_delta[i][d] += displacement;
								_delta[j][d] -= displacement;
							}
						}
					}
				}

				#pragma omp critical// for schedule(static)
				for(unsigned i = 0; i < delta.size(); i++)
				{
					for(unsigned j = 0; j < dim; j++)
					{
						delta.at(i).at(j) = delta.at(i).at(j) + _delta.at(i).at(j);
					}
				}
			}
			//std::cout << cc.getId() << ","  << r << "," << positive_force << "," << std::abs(negative_force) << std::endl;
			/*******************************************************************
			 * APPLY COOLING FUNCTION AND UPDATE POSITIONS
			 ******************************************************************/
			//#pragma omp for schedule(static) 
			for(unsigned i = 0; i < pos.size(); i++)
			{
				float len = 0.0;
				for(unsigned d = 0; d < dim;d++){
					len += delta[i][d]*delta[i][d];
				}
				len = std::sqrt(len);

				for(unsigned d = 0; d < dim;d++)
				{
					float pd = delta[i][d];
					if(len > temp)
					{
						pd = (pd/len)*temp;
					}
					//#pragma omp atomic
					pos[i][d] = pos[i][d] + pd;
				}
			}
			//DEBUG_force(pos,r);
		}
	}

	void partition(
			ConnectedComponent& cc,
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

			DEBUG_GM(clustering,pos,*it);

			std::vector<unsigned> membership(cc.size(),std::numeric_limits<unsigned>::max());
			unsigned clusterId = 0;
			for(auto& cluster:clustering){
				for(unsigned i = 0; i < cluster.size(); i++){
					membership.at(cluster.at(i)) = clusterId;
				}
				clusterId++;
			}

			float cost = 0;
			for(unsigned i = 0; i< membership.size(); i++)
			{
				for(unsigned j = i+1; j < membership.size(); j++)
				{
					if((membership.at(i) != membership.at(j))
							&& cc.at(i,j,false) > 0.0)
					{
						cost += cc.at(i,j,false);
					}else if((membership.at(i) == membership.at(j))
							&& cc.at(i,j,false) < 0.0)
					{
						cost -= cc.at(i,j,false);
					}
				}
			}
			if(cost < cr.cost)
			{
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

		for(auto& obj:objects)
		{
			std::list<unsigned> nodes;
			// fill list of nodes
			for (unsigned i=1; i < obj.size();i++)
			{
				nodes.push_back(i);
			}

			result.push_back(std::vector<unsigned>());
			std::queue<unsigned> Q;
			Q.push(0);
			unsigned componentId = result.size()-1;
			result.at(componentId).push_back(obj.at(0));
			while(!nodes.empty()){

				unsigned i = Q.front();
				for (auto it = nodes.begin(); it != nodes.end();)
				{
					unsigned j = *it;
					if(j != i)
					{
						if (TCC::dist(pos,obj.at(i),obj.at(j)) <= maxDist)
						{
							Q.push(j);
							result.at(componentId).push_back(obj.at(j));
							it = nodes.erase(it);
						}else{
							++it;
						}
					}else{
						++it;
					}
				}

				Q.pop();

				if(Q.empty())
				{
					if(!nodes.empty())
					{
						result.push_back(std::vector<unsigned>());
						componentId++;
						Q.push(nodes.front());
						result.at(componentId).push_back(obj.at(nodes.front()));
						nodes.pop_front();
					}
				}
			}
		}
		return result;
	}
}
