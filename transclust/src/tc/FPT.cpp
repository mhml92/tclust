#include <chrono>
#include <limits>
#include <iomanip>
#include <queue>
#include <iomanip>

#ifdef _OPENMP
#	include <omp.h>
#endif

#include "transclust/FPT.hpp"

FPT::FPT(
		ConnectedComponent& cc,
		float time_limit,
		float stepSize,
		float mockInf)
	:
		cc(cc),
		time_limit(time_limit),
		stepSize(stepSize),
		maxK(0),
		inf(mockInf),
		solution_found(false),
		solution_cost(0)
{ }


void FPT::cluster(RES::ClusteringResult &cr)
{
	cr.cost = -1;
	start = std::chrono::system_clock::now();
	while(!solution_found)
	{
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
			fptn.edgeCost.push_back(std::vector<float>());
			for(unsigned j = 0; j < fptn.size;j++)
			{
				if(i == j)
				{
					fptn.edgeCost.at(i).push_back(0);
				}else{
					fptn.edgeCost.at(i).push_back(cc.getCost(i,j,false));
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
			if(fptn.edgeCost.at(u).at(v) <= 0){ continue; }

			float cost_uv = fptn.edgeCost[u][v];
			float sumIcf = std::max(0.0f,cost_uv) + costSetForbidden(fptn,u,v);
			float sumIcp = std::max(0.0f,-cost_uv) + costSetPermanent(fptn,u,v);

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
				fptn.cost += std::max(0.0f,cost_uv);
				fptn.edgeCost[u][v] = -inf;
				fptn.edgeCost[v][u] = -inf;
				goto reduceLoopStart;
			}

		}
	}
}
float FPT::costSetForbidden(
		Node& fptn,
		unsigned u,
		unsigned v)
{
	float costs = 0;

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

float FPT::costSetPermanent(
		Node& fptn,
		unsigned u,
		unsigned v)
{
	float cost = 0;

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
	float highestoccurence = 0;
	float occurence = 0;

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
	float cost_uv = fptn0.edgeCost[u][v];

	/*************************************************************************
	 * Branch merge
	 ************************************************************************/
	float csp = std::max(0.0f,-cost_uv) + costSetPermanent(fptn0,u,v);
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
	float csf = std::max(0.0f,cost_uv) + costSetForbidden(fptn0,u,v);
	if (csf + fptn0.cost <= maxK)
	{
		float origCost = fptn0.edgeCost[u][v];

		fptn0.cost += std::max(0.0f,cost_uv);
		fptn0.edgeCost[u][v] = -inf;
		fptn0.edgeCost[v][u] = -inf;

		find_solution(fptn0);

		fptn0.cost -= std::max(0.0f,cost_uv);
		fptn0.edgeCost[u][v] = origCost;
		fptn0.edgeCost[v][u] = origCost;
	}

}
void FPT::mergeNodes(Node& fptn, unsigned node_i,unsigned node_j, float costForMerging)
{
	unsigned l = std::min(node_i,node_j);
	unsigned h = std::max(node_i,node_j);
	/* EDGECOST MATRIX */

	// add costs from the node with the higher index to the node with the
	// lower index
	for(unsigned i = 0; i < fptn.size; i++)
	{
		if( i == l ){
			fptn.edgeCost.at(l).at(i) = 0.0f;
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

void FPT::buildSolution(RES::ClusteringResult &cr)
{

	cr.cost = solution_cost;

	/////////////////////////////////////////////////////////////////////////////
	// find connected components in the reduced fptn
	/////////////////////////////////////////////////////////////////////////////
	std::list<unsigned> nodes;
	// fill list of nodes
	for (unsigned i=1; i< solution_edgeCost.size();i++)
	{
		nodes.push_back(i);
	}

	std::vector<std::vector<unsigned>> reduced_clusters(1,std::vector<unsigned>());

	std::queue<unsigned> Q;
	unsigned componentId = 0;
	Q.push(0);
	reduced_clusters.at(componentId).push_back(0);
	while(!nodes.empty()){

		unsigned i = Q.front();
		for (auto it = nodes.begin(); it != nodes.end();)
		{
			unsigned j = *it;
			if(j != i)
			{
				if (solution_edgeCost.at(i).at(j) > 0.0)
				{
					Q.push(j);
					reduced_clusters.at(componentId).push_back(j);
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
				componentId++;
				reduced_clusters.push_back(std::vector<unsigned>());
				Q.push(nodes.front());
				reduced_clusters.at(componentId).push_back(nodes.front());
				nodes.pop_front();
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////
	// Add result to the cr
	/////////////////////////////////////////////////////////////////////////////
	for(auto& cluster:reduced_clusters)
	{
		cr.clusters.push_back(std::deque<unsigned>());
		for(unsigned i = 0; i < cluster.size(); i++)
		{
			for(unsigned j = 0; j < solution_nodeParents.at(cluster.at(i)).size();j++)
			{
				cr.clusters.back().push_back(solution_nodeParents.at(cluster.at(i)).at(j));
			}
		}
	}
}
