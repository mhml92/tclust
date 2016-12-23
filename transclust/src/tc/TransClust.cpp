#include <chrono>
#ifdef _OPENMP
#	include <omp.h>
#endif
#include <plog/Log.h>
#include "transclust/TransClust.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/InputParser.hpp"
#include "transclust/FORCE.hpp"
#include "transclust/FPT.hpp"
#include "transclust/ClusteringResult.hpp"
#include "transclust/Common.hpp"

TransClust::TransClust(
		TCC::TransClustParams& _tcp,
		unsigned process_id
		)
	:
		tcp(_tcp),
		process_id(process_id)
{ }

void TransClust::cluster(
		std::deque<ConnectedComponent>& ccs,
		RES::ClusteringResult& result)
{
	unsigned num_fpt_solved = 0;
	unsigned num_transitive = 0;
	unsigned num_force_solved = 0;

	std::deque<ConnectedComponent> large_cc;
	#pragma omp parallel
	{
		#pragma omp for schedule(dynamic)
		for(unsigned i = 0; i < ccs.size(); i++)
		{
			// get a reference to the connected component at i 
			ConnectedComponent& cc = ccs.at(i);

			// initialize a struct holding the clustering result
			RES::ClusteringResult cr;

			// set initial cost to negativ, indicating 'no solution found'
			cr.cost = -1;

			// if the connected component is already transitive, just add to result
			if(cc.isTransitive())
			{
				#pragma omp critical(addResult)
				{
					num_transitive++;
					addResult(cc,cr,result);
				}
				cc.free(true);
				continue;	
			}

			if(cc.size() >= tcp.force_min_size_parallel && tcp.force_min_size_parallel >= 0){
				#pragma omp critical(add_large_force)
				{
					large_cc.push_back(cc);
				}
				continue;
			}

			// load the memorymapped file holding the cost-matrix 
			cc.load(tcp);

			// if the connected component is small enough we can try clustering 
			// using the FPT
			if(cc.size() <= tcp.fpt_max_cc_size && !tcp.disable_fpt)
			{
				FPT fpt(cc,tcp.fpt_time_limit,tcp.fpt_step_size);
				fpt.cluster(cr);
			}		

			// if no solution is found in fpt we cluster with FORCE
			if(cr.cost < 0)
			{
				#pragma omp atomic
				num_force_solved++;

				std::vector<std::vector<float>> pos;
				pos.resize(cc.size(), std::vector<float>(tcp.dim,0));

				// layout
				FORCE::layout(
						cc, 
						pos, 
						tcp.p, 
						tcp.f_att, 
						tcp.f_rep, 
						tcp.R, 
						tcp.start_t, 
						tcp.dim,
						tcp.seed);

				// partition
				FORCE::partition(
						cc, 
						pos, 
						cr, 
						tcp.d_init, 
						tcp.d_maximal, 
						tcp.s_init, 
						tcp.f_s);

			}else{
				#pragma omp atomic
				num_fpt_solved++;
			}

			// add clusters to solution
			#pragma omp critical(addResult)
			{
				addResult(cc,cr,result);
			}

			cc.free(true);
		}
	}

	for(unsigned i = 0; i < large_cc.size(); i++)
	{
		// get a reference to the connected component at i 
		ConnectedComponent& cc = large_cc.at(i);

		// load the memorymapped file holding the cost-matrix 
		cc.load(tcp);

		// initialize a struct holding the clustering result
		RES::ClusteringResult cr;

		// set initial cost to negativ, indicating 'no solution found'
		cr.cost = -1;

		num_force_solved++;

		std::vector<std::vector<float>> pos;
		pos.resize(cc.size(), std::vector<float>(tcp.dim,0));

		// layout
		FORCE::layout_parallel(
				cc, 
				pos, 
				tcp.p, 
				tcp.f_att, 
				tcp.f_rep, 
				tcp.R, 
				tcp.start_t, 
				tcp.dim,
				tcp.seed);

		// partition
		FORCE::partition(
				cc, 
				pos, 
				cr, 
				tcp.d_init, 
				tcp.d_maximal, 
				tcp.s_init, 
				tcp.f_s);

		addResult(cc,cr,result);
		cc.free(true);
	}
	
	LOGI_IF(num_transitive > 0) 
		<< "p_" << process_id << ": " 
		<< num_transitive 
		<< " Connected Components were already transitive";

	LOGI_IF(!tcp.disable_fpt && num_fpt_solved > 0) 
		<< "p_" << process_id << ": "
		<< "Clustered " 
		<< num_fpt_solved 
		<< " Connected Components to optimality";

	LOGI_IF(num_force_solved > 0) 
		<< "p_" << process_id << ": " 
		<< "Clustered " 
		<< num_force_solved 
		<< " Connected Components with FORCE";
}

void TransClust::addResult(
		ConnectedComponent& cc,
		RES::ClusteringResult& cr,
		RES::ClusteringResult& result)
{
	if(cc.isTransitive())
	{
		// the connected component is transitive => cost == 0
		cr.cost = 0;

		// as we know that all elements belong in the same cluster we can just
		// add the index2ObjectId list as the cluster
		result.clusters.push_back(cc.getIndex2ObjectId());
	}else{

		// Each cluster in the clustering result needs to be addad to the final
		// result and the local ids must to converted to global ids
		for(auto& cluster:cr.clusters)
		{
			// add a new deque to hold the cluster
			result.clusters.push_back(std::deque<unsigned>());
			for(unsigned local_id:cluster)
			{
				// add all elements with their global ids
				result.clusters.back().push_back(cc.getObjectId(local_id));
			}
		}
	}
	// finally add the cost of the connectede component to the total cost
	result.cost += cr.cost;
}
//void TransClust::run_FORCE_parallel(
//		std::deque<ConnectedComponent>& ccs,
//		std::deque<unsigned>& force_cc_large
//		)
//{
//	if(!force_cc_large.empty())
//	{
//		LOGI 
//			<< "Clustering " 
//			<< force_cc_large.size() 
//			<< " connected components using parallel FORCE";
//
//		for(unsigned i = 0; i < force_cc_large.size();i++)
//		{
//			ConnectedComponent& cc = ccs.at(force_cc_large.at(i)); 
//
//			RES::ClusteringResult cr;
//			cc.load();
//			LOGD << "clustering with FORCE";
//			// init position array
//			std::vector<std::vector<float>> pos;
//			pos.resize(cc.size(), std::vector<float>(tcp.dim,0));
//
//			// layout
//			FORCE::layout_parallel(
//					cc, 
//					pos, 
//					tcp.p, 
//					tcp.f_att, 
//					tcp.f_rep, 
//					tcp.R, 
//					tcp.start_t, 
//					tcp.dim,
//					tcp.seed);
//
//			// partition
//			FORCE::partition(
//					cc, 
//					pos, 
//					cr, 
//					tcp.d_init, 
//					tcp.d_maximal, 
//					tcp.s_init, 
//					tcp.f_s);
//			addResult(cc,cr);
//			cc.free(true);
//		}
//	}
//}
