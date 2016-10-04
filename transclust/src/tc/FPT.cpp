#include <chrono>
#include <limits>
#include <iomanip>
#include <queue>
#include <iomanip>

#if defined(_OPENMP)
#include <omp.h>
#endif

#include "transclust/FPT.hpp"

FPT::FPT(
		ConnectedComponent& cc,
		double time_limit,
		double stepSize,
		double mockInf)
	:
		cc(cc),
		time_limit(time_limit),
		stepSize(stepSize),
		maxK(0),
		inf(mockInf),
		solution_found(false),
		solution_cost(0)
{ }


void FPT::cluster(ClusteringResult &cr)
{
	cr.cost = -1;
	start = std::chrono::system_clock::now();
	while(!solution_found){
		/**********************************************************************
		 * Track time
		 *********************************************************************/
		if(getDeltaTime() > time_limit){ return; }

		/**********************************************************************
		 * Init root node
		 *********************************************************************/
		Node fptn;
		fptn.size = cc.size();
		fptn.cost = 0;

		/* Construct 'nodeParents'
		 * vector of vectors with the index number of each node
		 * [[0],[1],...,[n]]
		 */
		for(unsigned i = 0; i < fptn.size;i++)
		{
			fptn.nodeParents.push_back(std::vector<unsigned>(1,i));
		}

		/* Construct 'edgeCost'
		 * A 2d matrix of edge costs, initially a copy of cc's similaritys
		 */
		for(unsigned i = 0; i < fptn.size;i++)
		{
			fptn.edgeCost.push_back(std::vector<double>());
			for(unsigned j = 0; j < fptn.size;j++)
			{
				if(i == j)
				{
					fptn.edgeCost.at(i).push_back(0);
				}else{
					fptn.edgeCost.at(i).push_back(cc.at(i,j,false));
				}
			}
		}

		/*************************************************************************
		 * Reduce node
		 ************************************************************************/
		reduce(fptn);
		/*************************************************************************
		 * Start recursion
		 ************************************************************************/
		find_solution(fptn);

		/*************************************************************************
		 * Increase K
		 ************************************************************************/
		maxK += stepSize;
	}
	buildSolution(cr);
}

void FPT::reduce(Node& fptn)
{
reduceLoopStart:
	// terminaiton conditions
	if(getDeltaTime() > time_limit){ return; }

	for(unsigned u = 0; u < fptn.size; u++)
	{
		for(unsigned v = u+1; v < fptn.size; v++)
		{
			if(fptn.edgeCost.at(u).at(u) <= 0){ continue; }

			double cost_uv = fptn.edgeCost[u][v];
			double sumIcf = std::max(0.0,cost_uv) + costSetForbidden(fptn,u,v);
			double sumIcp = std::max(0.0,-cost_uv) + costSetPermanent(fptn,u,v);

			if(sumIcf + fptn.cost > maxK && sumIcp + fptn.cost > maxK)
			{
				fptn.cost = inf;
				return;
				goto reduceLoopStart;
			}else if( sumIcf + fptn.cost > maxK)
			{
				mergeNodes(fptn,u,v,sumIcp);
				goto reduceLoopStart;
			}else if(sumIcp + fptn.cost > maxK)
			{
				fptn.cost += std::max(0.0,cost_uv);
				fptn.edgeCost[u][v] = -inf;
				fptn.edgeCost[v][u] = -inf;
				goto reduceLoopStart;
			}

		}
	}
}
double FPT::costSetForbidden(
		Node& fptn,
		unsigned u,
		unsigned v)
{
	double costs = 0;

	for (unsigned w = 0; w < fptn.size; w++)
	{
		if( u == w || v == w){ continue; }

		if (fptn.edgeCost.at(u).at(w) > 0
				&& fptn.edgeCost.at(v).at(w) > 0)
		{
			costs += std::min(
					fptn.edgeCost.at(u).at(w),
					fptn.edgeCost.at(v).at(w));
		}
	}
	return costs;
}

double FPT::costSetPermanent(
		Node& fptn,
		unsigned u,
		unsigned v)
{
	double cost = 0;

	for (unsigned  w = 0; w < fptn.size; w++)
	{
		if (w == u || w == v)
		{
			continue;
		}
		// sum cost of symmetric set difference
		if( (fptn.edgeCost.at(u).at(w) > 0)
				!= (fptn.edgeCost.at(v).at(w) > 0) )
		{
			cost += std::min(
				std::abs(fptn.edgeCost.at(u).at(w)),
				std::abs(fptn.edgeCost.at(v).at(w))
			);
		}
	}
	return cost;
}

void FPT::find_solution(Node& fptn0)
{
	// terminaiton conditions
	if(getDeltaTime() > time_limit){
		return;
	}
	/*************************************************************************
	 * Find conflict triple
	 ************************************************************************/
	unsigned edge[2];
	double highestoccurence = 0;
	double occurence = 0;

	for (unsigned u = 0; u < fptn0.size; u++)
	{
		for (unsigned v = u + 1; v < fptn0.size; v++)
		{

			if (fptn0.edgeCost[u][v] > 0) {

				//occurence = std::abs(
				//		costSetPermanent(fptn0, u, v)
				//		- costSetForbidden(fptn0,u, v)
				//		);
				occurence = std::max(
						costSetPermanent(fptn0, u, v),
						costSetForbidden(fptn0, u, v)
						);

				if (occurence > highestoccurence)
				{
					highestoccurence = occurence;
					edge[0] = u;
					edge[1] = v;
				}
			}
		}
	}

	if(highestoccurence == 0){
		solution_found = true;
		solution_cost = fptn0.cost;
		solution_edgeCost = fptn0.edgeCost;
		solution_nodeParents = fptn0.nodeParents;
		maxK = fptn0.cost;
		return;
	}

	unsigned u = edge[0];
	unsigned v = edge[1];
	double cost_uv = fptn0.edgeCost[u][v];

	/*************************************************************************
	 * Branch merge
	 ************************************************************************/
	double csp = std::max(0.0,-cost_uv) + costSetPermanent(fptn0,u,v);
	if (csp + fptn0.cost <= maxK)
	{
		Node fptn1;
		clone_node(fptn0,fptn1);

		mergeNodes(fptn1,u,v, csp);
		find_solution(fptn1);
	}

	/*************************************************************************
	 * Branch forbidden
	 ************************************************************************/
	double csf = std::max(0.0,cost_uv) + costSetForbidden(fptn0,u,v);
	if (csf + fptn0.cost <= maxK)
	{
		double origCost = fptn0.edgeCost[u][v];

		fptn0.cost += std::max(0.0,cost_uv);
		fptn0.edgeCost[u][v] = -inf;
		fptn0.edgeCost[v][u] = -inf;

		find_solution(fptn0);

		fptn0.cost -= std::max(0.0,cost_uv);
		fptn0.edgeCost[u][v] = origCost;
		fptn0.edgeCost[v][u] = origCost;
	}

}
void FPT::mergeNodes(Node& fptn, unsigned node_i,unsigned node_j, double costForMerging)
{
	unsigned l = std::min(node_i,node_j);
	unsigned h = std::max(node_i,node_j);
	/* EDGECOST MATRIX */

	// add costs from the node with the higher index to the node with the
	// lower index
	for(unsigned i = 0; i < fptn.size; i++)
	{
		if( i == l ){
			fptn.edgeCost.at(l).at(i) = 0.0;
		}else{


			fptn.edgeCost.at(l).at(i) += fptn.edgeCost.at(h).at(i);
			fptn.edgeCost.at(i).at(l) += fptn.edgeCost.at(i).at(h);
		}
	}
	// remove the element with the higher index from the matrix (in both the
	// horizontal and vertical direction)
	// vertial
	fptn.edgeCost.erase(fptn.edgeCost.begin()+h);
	// horizontal
	for(unsigned i = 0; i < fptn.edgeCost.size(); i++)
	{
		fptn.edgeCost.at(i).erase(fptn.edgeCost.at(i).begin()+h);
	}
	/* NODE PARENTS */
	fptn.nodeParents.at(l).insert(
			fptn.nodeParents.at(l).end(),
			fptn.nodeParents.at(h).begin(),
			fptn.nodeParents.at(h).end());

	fptn.nodeParents.erase(fptn.nodeParents.begin()+h);

	fptn.cost += costForMerging;
	fptn.size -= 1;
}

void FPT::clone_node(Node& fptn0,Node& fptn1){
	fptn1.cost = fptn0.cost;
	fptn1.size = fptn0.size;
	fptn1.nodeParents = fptn0.nodeParents;
	fptn1.edgeCost = fptn0.edgeCost;
}

void FPT::buildSolution(ClusteringResult &cr)
{
	std::vector<unsigned> membership(
			solution_edgeCost.size(),
			std::numeric_limits<unsigned>::max());

	std::vector<std::vector<unsigned>> result;
	result.push_back(std::vector<unsigned>());
	std::queue<unsigned> Q;

	unsigned componentId = 0;
	Q.push(0);
	membership.at(0) = componentId;
	result.at(componentId).push_back(0);
	while(true){

		unsigned i = Q.front();
		Q.pop();

		for(unsigned j = 0; j < solution_edgeCost.size();j++)
		{
			if(membership.at(j) == std::numeric_limits<unsigned>::max())
			{
				if(solution_edgeCost.at(i).at(j) > 0)
				{
					Q.push(j);
					membership.at(j) = componentId;
					result.at(componentId).push_back(j);
				}
			}
		}

		if(Q.empty())
		{
			componentId++;
			// could be done smarter by by saving 'last known nonmember'
			for(unsigned s = 0; s < membership.size();s++)
			{
				if(membership.at(s) == std::numeric_limits<unsigned>::max())
				{
					Q.push(s);
					membership.at(s) = componentId;
					result.push_back(std::vector<unsigned>());
					result.at(componentId).push_back(s);
					break;
				}
			}
			if(Q.empty()){
				break;
			}
		}
	}
	cr.membership.resize(cc.size());
	for(unsigned i = 0; i < solution_edgeCost.size(); i++)
	{
		for(unsigned j = 0; j < solution_nodeParents.at(i).size(); j++)
		{
			cr.membership.at(solution_nodeParents.at(i).at(j)) = membership.at(i);
		}
	}
	cr.cost = solution_cost;
}
