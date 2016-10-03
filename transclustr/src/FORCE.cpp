#include <Rcpp.h>
#include <iomanip>
#include <iostream>
#include <random>
#include <limits>
#include <list>
#include <queue>
#include <algorithm>
#include <chrono>
#if defined(_OPENMP)
#include <omp.h>
#endif

#include "transclust/ConnectedComponent.hpp"
#include "transclust/FindConnectedComponents.hpp"
#include "transclust/FORCE.hpp"
#include "transclust/ClusteringResult.hpp"

namespace FORCE
{
	double dist(std::vector<std::vector<double>>& pos,unsigned i, unsigned j)
	{
		double res = 0;
		for(unsigned d = 0; d < pos[0].size(); d++){
			double side = pos[i][d] - pos[j][d];
			res += side*side;
		}
		return std::sqrt(res);
	}

	void layout(
			const ConnectedComponent& cc,
			std::vector<std::vector<double>>& pos,
			double p,
			double f_att,
			double f_rep,
			unsigned R,
			double start_t,
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
			double radStep = (2*M_PI)/pos.size();
			double radVal = 0;
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
			std::uniform_real_distribution<double> distribution(-1.0,1.0);
			#pragma omp parallel for
			for(unsigned i = 0; i < pos.size(); i++)
			{
				double r = 0;
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
		/*************************************************************************
		 * MAIN LOOP
		 ************************************************************************/
		std::vector<std::vector<double>> delta;
		delta.resize(pos.size(), std::vector<double>(dim,0.0));

		f_att /= static_cast<double>(cc.size());
		f_rep /= static_cast<double>(cc.size());

		for(unsigned r = 0; r < R; r++)
		{
			// zero delta vectors
			for(auto& v:delta){std::fill(v.begin(),v.end(),0.0);}


			double temp = (start_t*static_cast<double>(cc.size()))*std::pow((1.0/(r+1.0)),2);
			/**********************************************************************
			 * CALCULATE DELTA VECTOR
			 *********************************************************************/
				for(unsigned i = 0; i < pos.size();i++)
				{
					for(unsigned j = i+1; j < pos.size(); j++)
					{
						double _distance = dist(pos,i,j);
						if(_distance > 0.0)
						{
							double log_d = std::log(_distance+1);

							double force = 0.0;

							// normalized edge weight
							double edge_weight = cc.at(i,j);
							if(edge_weight > 0)
							{
								force = (edge_weight * f_att * log_d)/_distance;
							}else{
								force = ((edge_weight * f_rep)/log_d)/_distance;
							}
							for(unsigned d = 0; d < dim; d++)
							{
								double displacement = (force * (pos[j][d]-pos[i][d]));
								delta[i][d] += displacement;
								delta[j][d] -= displacement;
							}
						}
					}
				}

			/**********************************************************************
			 * APPLY COOLING FUNCTION AND UPDATE POSITIONS
			 *********************************************************************/
				for(unsigned i = 0 ; i < pos.size(); i++)
				{
					double len = 0.0;
					for(unsigned d = 0; d < dim;d++){
						len += delta[i][d]*delta[i][d];
					}
					len = std::sqrt(len);
					//std::cout << len << std::endl;
					for(unsigned d = 0; d < dim;d++)
					{
						double pd = delta[i][d];
						if(len > temp)
						{
							//delta[i][d] = (delta[i][d]/len)*temp;
							pd = (pd/len)*temp;
						}
						pos[i][d] += pd;
					}
				}
		}
	}

	void partition(
			const ConnectedComponent& cc,
			std::vector<std::vector<double>>& pos,
			ClusteringResult& cr,
			double d_init,
			double d_maximal,
			double s_init,
			double f_s)
	{
		double d = d_init;
		double s = s_init;
		std::vector<double> D;
		while(d <= d_maximal){
			D.push_back(d);
			d += s;
			s += s*f_s;
		}
		cr.cost = std::numeric_limits<double>::max();

		std::vector<std::vector<unsigned>> clustering;

		std::vector<unsigned> dummy(cc.size());
		std::iota(dummy.begin(),dummy.end(),0);
		clustering.push_back(dummy);

		for(std::vector<double>::reverse_iterator it = D.rbegin(); it != D.rend(); ++it) {
			clustering = geometricLinking(pos,*it,clustering);

			std::vector<unsigned> membership(cc.size(),std::numeric_limits<unsigned>::max());
			unsigned clusterId = 0;
			for(auto& cluster:clustering){
				for(unsigned i = 0; i < cluster.size(); i++){
					membership.at(cluster.at(i)) = clusterId;
				}
				clusterId++;
			}

			double cost = 0;
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
			std::vector<std::vector<double>>& pos,
			const double maxDist,
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
						if (dist(pos,obj.at(i),obj.at(j)) <= maxDist)
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
