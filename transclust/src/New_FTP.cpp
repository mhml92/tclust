#include "New_FPT.hpp"
#include "plog/Log.h"

New_FPT::New_FPT(
	ConnectedComponent& cc,
	ClusteringResult& cr,
	double time_limit,
	double stepSize
):
	cc(cc),
	time_limit(time_limit),
	stepSize(stepSize),
	maxK(stepSize)
{
	start = std::chrono::system_clock::now(); 
	while(!solution_found){
		/**********************************************************************
		 * Track time
		 *********************************************************************/
		if(getDeltaTime() > time_limit){ return; }

		/*************************************************************************
		 * Init root node
		 ************************************************************************/
		Node fptn;
		initializeRootNode(cc,cr,fptn);

		// apply reduction rules
		//reduce(fptn);
		
		// start search
		find_solution(fptn);
		
		// incrment parameter
		maxK += stepSize;
	}

}

void New_FPT::cluster(
	ClusteringResult& cr
)
{
	start = std::chrono::system_clock::now(); 
	while(!solution_found){
		/**********************************************************************
		 * Track time
		 *********************************************************************/
		if(getDeltaTime() > time_limit){ return; }

		/*************************************************************************
		 * Init root node
		 ************************************************************************/
		Node fptn;
		initializeRootNode(cc,cr,fptn);

		// apply reduction rules
		//reduce(fptn);
		
		// start search
		find_solution(fptn);
		
		// incrment parameter
		maxK += stepSize;
		return;
	}

}

//void New_FPT::reduce(Node& fptn)
//{
//	//
//}

double New_FPT::icf(Node& fptn, unsigned u,unsigned v)
{
	double cost = 0;

	for(unsigned w = 0; w < fptn.size; w++)
	{
		if(w == u || w == v){ continue; }

		// w \in N(u) \cap N(v)
		if( fptn.hasEdge[u][w] && fptn.hasEdge[v][w] )
		{
			cost += std::min(
				fptn.edgeCost[u][w],
				fptn.edgeCost[v][w]
			);
		}
	}
	return cost;
}

double New_FPT::icp(Node& fptn, unsigned u,unsigned v)
{
	double cost = 0;

	for (unsigned  w = 0; w < fptn.size; w++) 
	{
		if (w == u || w == v){ continue; }

		// sum cost of symmetric set difference
		if( fptn.hasEdge[u][w] != fptn.hasEdge[v][w] )
		{
			cost += std::min(
				std::abs(fptn.edgeCost[v][w]),
				std::abs(fptn.edgeCost[u][w])
			);
		}
	}
	return cost;
}

void New_FPT::find_solution(Node &fptn)
{

	/**************************************************************************** 
	 * Conflict Triple vector
	 * If a conflict triple is found will the vector contain the three node 
	 * indexes u,v and w, where u is the node with cardinality 2 and v,w are the
	 * leafs
	 */
	std::vector<unsigned> ct;
	findConflictTriple(fptn,ct);
	
	//LOG_DEBUG << ct[0] << ", " << ct[1] << ", " << ct[2];
	// no conflict tripled
	if(ct.empty())
	{
		solution = fptn;
		solution_found = true;
		maxK = solution.cost;
		return;
	}
		
}

void New_FPT::findConflictTriple(Node& fptn, std::vector<unsigned>& ct)
{
	// dumb heuristic: return first found conflict (is it dumb? maybe I'm dumb..)	
	for(unsigned u = 0; u < fptn.size; u++)
	{
		for(unsigned v = u+1; v < fptn.size; v++)
		{
			for(unsigned w = v+1; w < fptn.size; w++)
			{
				if( fptn.hasEdge[u][v] + fptn.hasEdge[u][w] + fptn.hasEdge[v][w] == 2 ) 
				{
					LOG_DEBUG << fptn.hasEdge[u][v] <<" - "<< fptn.hasEdge[u][w] <<" - "<< fptn.hasEdge[v][w];
					// place node with cardinality first
					if(!fptn.hasEdge[u][v]){
						ct.push_back(w);	
						ct.push_back(v);	
						ct.push_back(u);	
					}else if(!fptn.hasEdge[u][w]){
						ct.push_back(v);	
						ct.push_back(w);	
						ct.push_back(u);	
					}else if(!fptn.hasEdge[w][v]){
						ct.push_back(u);	
						ct.push_back(w);	
						ct.push_back(v);	
					}
					return;
				}
			}
		}
	}
}



void New_FPT::initializeRootNode(ConnectedComponent& cc,ClusteringResult& cr, Node& fptn)
{
		fptn.size = cr.membership.size();
		fptn.cost = cr.cost;

		/************************************************************************* 
		 * Construct 'nodeParents' 
		 * vector of vectors with the index number of each node
		 * [[0],[1],...,[n]]
		 */
		for(unsigned i = 0; i < fptn.size;i++)
		{
			fptn.nodeParents.push_back(std::vector<unsigned>(1,i));
		}

		/*************************************************************************
		 * Construct 'edgeCost'
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
		/************************************************************************
		 * Constructs 'hasEdge'
		 * A 2d matrix of bool, indicating of nodes i,j has an edge between them
		 */
		fptn.hasEdge = std::vector<std::vector<bool>>(
			fptn.size,
			std::vector<bool>(fptn.size,false)
		);
		for(unsigned i = 0; i < cr.membership.size(); i++)
		{
			for(unsigned j = i+1; j < cr.membership.size(); j++)
			{
				if(cr.membership[i] == cr.membership[j])
				{
					fptn.hasEdge[i][j] = true;	
					fptn.hasEdge[j][i] = true;	
				}
			}
		}
}
