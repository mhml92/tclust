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
#	define DEBUG_COST(cc,clustering,cost) DEBUG::test_cost(cc,clustering,cost);
#else
#	define DEBUG_GM(membership, cc, threshold) {}
#	define DEBUG_COST(cc,clustering,cost) {} 
#endif
#include "transclust/FORCE.hpp"
#include "transclust/Common.hpp"
#include "transclust/ConnectedComponent.hpp"
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
				float len = 0;
				for(unsigned d = 0; d < dim; d++)
				{
					pos[i][d] = distribution(generator);
					len += pos[i][d] * pos[i][d];
				}
				len = std::sqrt(len);

				for(unsigned d = 0; d < dim; d++)
				{
					pos[i][d] = (pos[i][d]/len)*p;
				}
			}
		}
		//DEBUG_force(pos,-1);
		/*************************************************************************
		 * MAIN LOOP
		 ************************************************************************/
		std::vector<std::vector<float>> delta;
		delta.resize(pos.size(), std::vector<float>(dim,0.0f));

		//f_att /= static_cast<float>(cc.size());
		//f_rep /= static_cast<float>(cc.size());
		float num_nodes = cc.size();

		for(unsigned r = 0; r < R; r++)
		{
			// zero delta vectors
			for(auto& v:delta){std::fill(v.begin(),v.end(),0.0f);}


			float temp = (start_t*(float)cc.size())*std::pow((1.0f/(r+1)),2);
			/**********************************************************************
			 * CALCULATE DELTA VECTOR
			 *********************************************************************/
			for(unsigned i = 0; i < pos.size();i++)
			{

				// read all neseccary cost values for object i in to a buffer
				// to minimoze potential I/Os
				//std::vector<float> cost_buffer(pos.size()-(i+1),0);
				//unsigned j_pos = 0;
				////#pragma omp critical
				//{
				//	for(unsigned j = i+1;j < pos.size();j++)
				//	{
				//		cost_buffer.at(j_pos) = cc.getCost(i,j);
				//		j_pos++;
				//	}

				//}
				//j_pos = 0;

				for(unsigned j = i+1; j < pos.size(); j++)
				{
					float _distance = TCC::dist(pos,i,j);
					if(_distance > 0.0f)
					{
						
						float log_d = std::log(_distance+1);

						float force = 0.0f;

						//float edge_weight = cost_buffer.at(j_pos);//cc.getCost(i,j);
						float edge_weight = cc.getCost(i,j);
						//						j_pos++;
						if(edge_weight > 0)
						{
							force = ((edge_weight * f_att * log_d))/num_nodes;//_distance;
						}else{
							if(_distance > 4){
								//std::cout << "dist > 5\n"
								//	<< "edge_weight <- " << edge_weight << "\n"
								//	<< "f_rep       <- " << f_rep << "\n"
								//	<< "log_d       <- " << log_d << "\n"
								//	<< "num_nodes   <- " << num_nodes << "\n"
								//	<< "#force:        " << ((edge_weight * f_rep))/(log_d*num_nodes) 
								//	<< std::endl;
								continue;
							
							}
							force = ((edge_weight * f_rep))/(log_d*num_nodes);
						}
						for(unsigned d = 0; d < dim; d++)
						{
							float displacement = (force * (pos[j][d]-pos[i][d])/_distance);
							delta[i][d] += displacement;
							delta[j][d] -= displacement;
						}
					}
				}
			}

			/*******************************************************************
			 * APPLY COOLING FUNCTION AND UPDATE POSITIONS
			 ******************************************************************/
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
					pos[i][d] = pos[i][d] + pd;
				}
			}
			//DEBUG_force(pos,r);
		}
	}

	void partition(
			ConnectedComponent& cc,
			std::vector<std::vector<float>>& pos,
			RES::ClusteringResult& cr,
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
		cr.cost = std::numeric_limits<double>::max();

		std::deque<std::deque<unsigned>> clustering;

		std::deque<unsigned> dummy(cc.size());
		std::iota(dummy.begin(),dummy.end(),0);
		clustering.push_back(dummy);

		for(std::vector<float>::reverse_iterator it = D.rbegin(); it != D.rend(); ++it) 
		{
			clustering = geometricLinking(pos,*it,clustering);

			DEBUG_GM(clustering,pos,*it);

			double cost = RES::calculateCost(cc,clustering);

			DEBUG_COST(cc,clustering,cost);

			if(cost < cr.cost)
			{
				cr.cost = cost;
				cr.clusters = clustering;
				if(cr.cost == 0){break;}
			}
		}
	}

	/*******************************************************************************
	 * DETERMINE MEMBERSHIP IN POS
	 ******************************************************************************/
	std::deque<std::deque<unsigned>> geometricLinking(
			std::vector<std::vector<float>>& pos,
			float maxDist,
			std::deque<std::deque<unsigned>>& objects)
	{
		std::deque<std::deque<unsigned>> result;

		for(auto& obj:objects)
		{
			std::list<unsigned> nodes;
			// fill list of nodes
			for (unsigned i=1; i < obj.size();i++)
			{
				nodes.push_back(i);
			}

			result.push_back(std::deque<unsigned>());
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
						result.push_back(std::deque<unsigned>());
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
