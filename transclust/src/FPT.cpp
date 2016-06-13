#include "FPT.hpp"
#include <chrono>
#include <limits>
#include <algorithm> 
#include <queue>

namespace FPT{

	/****************************************************************************
	 * MAIN LOOP
	 ***************************************************************************/
	void cluster(
			ConnectedComponent& cc,
			ClusteringResult& cr,
			float time_limit,
			float maxK,
			float stepSize)
	{

		// initial K
		float K = stepSize;
		// set initial cost to negativ, indicating 'no solution found (yet)'
		cr.cost = -1;

		// init start time
		std::chrono::time_point<std::chrono::system_clock> t1,t2; 
		t1 = std::chrono::system_clock::now(); 
		while(cr.cost < 0){
			//std::cout << "K: " << K << std::endl;

			//dumpCR(cr);

			/**********************************************************************
			 * Track time
			 *********************************************************************/
			t2 = std::chrono::system_clock::now(); 
			std::chrono::duration<float> delta_time = t2-t1;

			// terminaiton conditions
			if(delta_time.count() > time_limit 
					|| K > maxK)
			{
				return; 
			}
			LOG_VERBOSE << "K size: " << K;
			LOG_VERBOSE << "FTP delta_time: " << delta_time.count();

			/**********************************************************************
			 * Init first tree node
			 * 1. Create tree node
			 * 2. reduce
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
			 * (normalized)
			 */
			for(unsigned i = 0; i < fptn.size;i++)
			{
				//fptn.nodeParents.push_back(std::vector<unsigned>());
				fptn.edgeCost.push_back(std::vector<float>());
				for(unsigned j = 0; j < fptn.size;j++)
				{
					if(i == j)
					{
						fptn.edgeCost.at(i).push_back(0);
					}else{
						fptn.edgeCost.at(i).push_back(cc.at(i,j));
					}
				}

			}
			/* Constructs forbidden
			 * A 2d matrix of bool's representing forbidden edges
			 * true: forbidden
			 * false: allowed
			for(unsigned i = 0; i < fptn.size;i++)
			{
				fptn.forbidden.push_back(std::vector<bool>(fptn.size,false));
			}
			*/
			LOG_VERBOSE << "node size:" << fptn.size;
			/* Reduce node */
			reduce(cc,cr,fptn,K);
			LOG_VERBOSE << "reduced node size:" << fptn.size;

			/**********************************************************************
			 * Start recursion
			 *********************************************************************/
			find_solution(cc,cr,fptn,K);

			K += stepSize;
		}
	}

	void reduce(
			ConnectedComponent& cc,
			ClusteringResult& cr,
			Node& fptn,
			float K)
	{
		reduceLoopStart:
		//dumpCost(fptn);


		if(fptn.cost > K){
			return;
		}

		for(unsigned i = 0; i < fptn.size; i++)
		{
			for(unsigned j = i+1; j < fptn.size; j++)
			{
				/*************************************************************
				 * If edge cost of (i,j) <= 0 continue
				 ************************************************************/
				if(fptn.edgeCost.at(i).at(j) <= 0)
				{
					continue;
				}

				float sumIcf = costSetForbidden(fptn,i,j);
				float sumIcp = costSetPermanent(fptn,i,j);

				if(sumIcf + fptn.cost > K && sumIcp + fptn.cost > K)
				{
					//solution infeasible
					fptn.cost = std::numeric_limits<float>::max();
					goto reduceLoopStart;
				}else if(sumIcf + fptn.cost > K)
				{
					mergeNodes(fptn,i,j,sumIcp);
					goto reduceLoopStart;
				}else if(sumIcp + fptn.cost > K)
				{
					fptn.cost = (fptn.cost + fptn.edgeCost.at(i).at(j));
					fptn.edgeCost.at(i).at(j) = std::numeric_limits<float>::min();
					fptn.edgeCost.at(j).at(i) = std::numeric_limits<float>::min();
					goto reduceLoopStart;
				}
			}
		}
	}
	// TODO REMEMBER FORBIDDEN BOOL MATRIX
	void setForbiddenAndFindSolution(
			ConnectedComponent& cc,
			ClusteringResult& cr,
			Node& fptn,
			float K,
			unsigned node_i, 
			unsigned node_j, 
			float costsForSetForbidden)
	{
		fptn.cost = (fptn.cost + costsForSetForbidden);
		//fptn.cost = (fptn.cost + fptn.edgeCost.at(node_i).at(node_j));
		float old_cost = fptn.edgeCost.at(node_i).at(node_j);
		fptn.edgeCost.at(node_i).at(node_j) = std::numeric_limits<float>::max();
		fptn.edgeCost.at(node_j).at(node_i) = std::numeric_limits<float>::max();
		find_solution(cc,cr,fptn,K);

		fptn.cost = (fptn.cost - costsForSetForbidden);
		fptn.edgeCost.at(node_i).at(node_j) = old_cost;// costsForSetForbidden;
		fptn.edgeCost.at(node_j).at(node_i) = old_cost; //costsForSetForbidden;
	}



	void mergeNodes(Node& fptn, unsigned node_i,unsigned node_j, float costForMerging)
	{

		//std::cout << "MERGING NODES: " << node_i << ", " << node_j << std::endl; 
		//dumpFPTN(fptn);
		unsigned l = std::min(node_i,node_j);
		unsigned h = std::max(node_i,node_j);

		/* EDGECOST MATRIX */

		// add costs from the node with the higher index to the node with the
		// lower index 
		for(unsigned i = 0; i < fptn.size; i++)
		{
			if( i == l ){
				fptn.edgeCost.at(l).at(i) = 0;    
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

		/* FORBIDDEN MATRIX */
		// combine the two forbidden bool vectors (similar to edgecost)
		/*
		for(unsigned i = 0; i < fptn.size; i++)
		{
			fptn.forbidden.at(l).at(i) = (fptn.forbidden.at(l).at(i) || fptn.forbidden.at(h).at(i));    
		}
		*/

		// remove the element with the higher index from the matrix (in both the 
		// horizontal and vertical direction)
		// vertial 
		/*
		fptn.forbidden.erase(fptn.forbidden.begin()+h);
		// horizontal
		for(unsigned i = 0; i < fptn.forbidden.size(); i++)
		{
			fptn.forbidden.at(i).erase(fptn.forbidden.at(i).begin()+h);    
		}
		*/
		/* NODE PARENTS */
		fptn.nodeParents.at(l).insert(
				fptn.nodeParents.at(l).end(), 
				fptn.nodeParents.at(h).begin(), 
				fptn.nodeParents.at(h).end());

		fptn.nodeParents.erase(fptn.nodeParents.begin()+h);

		fptn.cost += costForMerging;
		fptn.size -= 1;
		//std::cout << "mergenodes after" << std::endl;
		//dumpFPTN(fptn);

	}

	float costSetPermanent(
			Node& fptn, 
			unsigned node_i,
			unsigned node_j) 
	{
		float cost = 0;

		for (unsigned  i = 0; i < fptn.size; i++) 
		{
			if (i == node_i || i == node_j) 
			{
				continue;
			}

			// sum cost of symmetric set difference
			if( (fptn.edgeCost.at(node_i).at(i) > 0) 
					!= (fptn.edgeCost.at(node_j).at(i) > 0) )
			{
				cost += std::min(
					std::abs(fptn.edgeCost.at(node_i).at(i)),		
					std::abs(fptn.edgeCost.at(node_j).at(i))		
				);
			
			}
/*
			else if ((fptn.edgeCost.at(i).at(node_i) > 0 
						&& fptn.edgeCost.at(i).at(node_j) <= 0)) 
			{
				cost += std::min(
						fptn.edgeCost.at(i).at(node_i),
						-fptn.edgeCost.at(i).at(node_j));
			} 
			else if ((fptn.edgeCost.at(i).at(node_i) <= 0 
						&& fptn.edgeCost.at(i).at(node_j) > 0)) 
			{
				cost += std::min(
						-fptn.edgeCost.at(i).at(node_i),
						fptn.edgeCost.at(i).at(node_j));
			}
		*/
		}
		return cost;
	}


	float costsSetForbidden(
			Node fptn, 
			unsigned node_i,
			unsigned node_j) 
	{
		float costs = 0;

		for (unsigned i = 0; i < fptn.size; i++) 
		{
			if (fptn.edgeCost.at(node_i).at(i) > 0 
					&& fptn.edgeCost.at(node_j).at(i) > 0) 
			{
				costs += std::min(
						fptn.edgeCost.at(node_i).at(i),
						fptn.edgeCost.at(node_j).at(i));
			}
		}
		costs += fptn.edgeCost.at(node_i).at(node_j);
		return costs;
	}


	void find_solution(
			ConnectedComponent& cc,
			ClusteringResult& cr,
			Node& fptn0,
			float K)
	{

		if(cr.cost > -1){
			return;
		}
		/*************************************************************************
		 * Find conflict triple
		 ************************************************************************/
		// if no triple return this as solution
		unsigned edge[2];
		float highestoccurence = 0;
		float occurence = 0;

		for (unsigned i = 0; i < fptn0.size; i++) 
		{
			for (unsigned j = i + 1; j < fptn0.size; j++) 
			{
				if (fptn0.edgeCost.at(i).at(j) > 0) {

					occurence = std::abs(
							costSetPermanent(fptn0, i, j) 
							- costSetForbidden(fptn0,i, j)
							);

					if (occurence > highestoccurence) 
					{
						highestoccurence = occurence;
						edge[0] = i;
						edge[1] = j;
					}
				}
			}
		}

		if (highestoccurence == 0) 
		{
			LOG_VERBOSE << "Solution found:";
			LOG_VERBOSE << "cost: " << fptn0.cost;
			cr.cost = fptn0.cost;
			buildSolution(cc,cr,fptn0);   
			return;
		}
		/*************************************************************************
		 * Create new fpt node based on old
		 ************************************************************************/
		Node fptn1;
		clone_node(fptn0,fptn1);


		// calculate cost for set forbidden
		/*************************************************************************
		 * Branch forbidden
		 ************************************************************************/
		float csf = costsSetForbidden(fptn1, edge[0], edge[1]);
		if (fptn1.cost + csf <= K)
		{ 
			unsigned node_i = edge[0];
			unsigned node_j = edge[1];

			float cost = fptn1.edgeCost.at(node_i).at(node_j);
			fptn1.cost = (fptn1.cost + cost);
			//fptn.cost = (fptn.cost + fptn.edgeCost.at(node_i).at(node_j));
			float old_cost = fptn1.edgeCost.at(node_i).at(node_j);
			fptn1.edgeCost.at(node_i).at(node_j) = std::numeric_limits<float>::max();
			fptn1.edgeCost.at(node_j).at(node_i) = std::numeric_limits<float>::max();
			find_solution(cc,cr,fptn1,K);

			fptn1.cost -= cost;
			fptn1.edgeCost.at(node_i).at(node_j) = old_cost;// costsForSetForbidden;
			fptn1.edgeCost.at(node_j).at(node_i) = old_cost; //costsForSetForbidden;

			/*
			setForbiddenAndFindSolution(
					cc,
					cr,
					fptn1,
					K,
					edge[0], 
					edge[1], 
					fptn1.edgeCost.at(edge[0]).at(edge[1]));
					*/
		}

		// calculate cost for merge
		/*************************************************************************
		 * Branch merge
		 ************************************************************************/
		float csm = costSetPermanent(fptn1,edge[0], edge[1]);
		if (csm + fptn1.cost <= K) 
		{

			mergeNodes(fptn1, edge[0],edge[1], csm);
			find_solution(cc,cr,fptn1,K);
		}

	}

	float costSetForbidden(
			Node& fptn, 
			unsigned node_i,
			unsigned node_j)
	{
		float costs = 0;

		for (unsigned i = 0; i < fptn.size; i++) 
		{
			if (fptn.edgeCost.at(node_i).at(i) > 0 && fptn.edgeCost.at(node_j).at(i) > 0) 
			{
				costs += std::min(fptn.edgeCost.at(node_i).at(i),fptn.edgeCost.at(node_j).at(i));
			}
		}
		//costs += fptn.edgeCost.at(node_i).at(node_j);
		return costs;
	}

	void clone_node(Node& fptn0,Node& fptn1){
		fptn1.cost = fptn0.cost;
		fptn1.size = fptn0.size;
		fptn1.nodeParents = fptn0.nodeParents;
		fptn1.edgeCost = fptn0.edgeCost;
		//fptn1.forbidden = fptn0.forbidden;
	}

	void buildSolution(
			ConnectedComponent& cc,
			ClusteringResult& cr, 
			Node& fptn)
	{
		std::vector<unsigned> membership(
				fptn.size,
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

			for(unsigned j = 0; j < fptn.size;j++)
			{
				if(membership.at(j) == std::numeric_limits<unsigned>::max())
				{
					if(fptn.edgeCost.at(i).at(j) > 0)
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
		for(unsigned i = 0; i < fptn.size; i++)
		{
			for(unsigned j = 0; j < fptn.nodeParents.at(i).size(); j++)
			{
				cr.membership.at(fptn.nodeParents.at(i).at(j)) = membership.at(i);
			}
		}
		cr.cost = fptn.cost;
	}
}
