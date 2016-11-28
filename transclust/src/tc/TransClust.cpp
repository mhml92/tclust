#include <chrono>
#ifdef _OPENMP
#	include <omp.h>
#endif
#include <boost/mpi.hpp>
#include "transclust/TransClust.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/InputParser.hpp"
#include "transclust/FORCE.hpp"
#include "transclust/FPT.hpp"
#include "transclust/ClusteringResult.hpp"
#include "transclust/Common.hpp"

TransClust::TransClust(
		TCC::TransClustParams& _tcp
		)
	:
		tcp(_tcp)
{ }

RES::ClusteringResult TransClust::cluster(
		std::deque<ConnectedComponent>& ccs,
		RES::ClusteringResult& result)
{
	#pragma omp parallel
	{
		#pragma omp for schedule(dynamic)
		for(unsigned i = 0; i < ccs.size(); i++)
		{

			// get a reference to the connected component at i 
			ConnectedComponent& cc = ccs.at(i);

			// load the memorymapped file holding the cost-matrix 
			cc.load();

			// initialize a struct holding the clustering result
			RES::ClusteringResult cr;

			// set initial cost to negativ, indicating 'no solution found'
			cr.cost = -1;

			// if the connected component is already transitive, just add to result
			if(cc.isTransitive())
			{
				#pragma omp critical(addResult)
				{
					addResult(cc,cr,result);
				}
				cc.free(true);
				continue;	
			}

			// if the connected component is small enough we try clustering 
			// using the FPT
			if(cc.size() <= tcp.fpt_max_cc_size && !tcp.disable_fpt)
			{
				FPT fpt(cc,tcp.fpt_time_limit,tcp.fpt_step_size);
				fpt.cluster(cr);
			}		

			// if no solution is found in fpt we cluster with FORCE
			if(cr.cost < 0)
			{
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

			}

			// add clusters to solution
			#pragma omp critical(addResult)
			{
				addResult(cc,cr,result);
			}

			cc.free(true);
		}
	}


	return result;
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



//
//
//
//	// array holding indexes for connected components which could not succesfully
//	// be clustered using the FPT algorithm but is to small enough to be run 
//	// single threaded
//	std::deque<unsigned> force_cc_small;
//
//	// if a connected component is larger then FORCE_max_cc_size will it be 
//	// clustered using parallel force.
//	std::deque<unsigned> force_cc_large;
//
//	/////////////////////////////////////////////////////////////////////////////
//	// FPT
//	//---------------------------------------------------------------------------
//	// Try to cluster all connected components smaller than FPT_max_cc_size using 
//	// the FPT algorithm. If a connected component could not be clusterd it will 
//	// clusterd with FORCE
//	/////////////////////////////////////////////////////////////////////////////
//	
//	if(!tcp.disable_fpt)
//	{
//		run_fpt(
//				ccs,
//				force_cc_small,
//				force_cc_large
//				);
//	}else{
//		for(unsigned i = 0; i < ccs.size();i++)
//		{
//			if(ccs.at(i).size() < tcp.force_min_size_parallel){
//				force_cc_small.push_back(i);
//			}else{
//				force_cc_large.push_back(i);
//			}
//		}
//	}
//
//	/////////////////////////////////////////////////////////////////////////////
//	// FORCE
//	// --------------------------------------------------------------------------
//	// Connected components smaller than FORCE_max_cc_size will be solved single-
//	// threaded but cuncurrent. Larger connected components are solved on at a 
//	// time but using multiple threads.
//	/////////////////////////////////////////////////////////////////////////////
//	run_FORCE_concurrent(ccs,force_cc_small);
//	run_FORCE_parallel(ccs,force_cc_large);
//
//	// each process returns its result
//
//void TransClust::run_fpt(
//		std::deque<ConnectedComponent>& ccs,
//		std::deque<unsigned>& force_cc_small,
//		std::deque<unsigned>& force_cc_large
//		)
//{
//	LOGI << "Clustering using FPT algorithm";
//	#pragma omp parallel
//	{
//		#pragma omp for schedule(dynamic)
//		for(unsigned i = 0; i < ccs.size(); i++)
//		{
//
//			// get a reference to the connected component at i 
//			ConnectedComponent& cc = ccs.at(i);
//
//			// load the memorymapped file holding the cost-matrix 
//			cc.load();
//
//			// initialize a struct holding the clustering result
//			RES::ClusteringResult cr;
//
//			// set initial cost to negativ, indicating 'no solution found'
//			cr.cost = -1;
//
//			// if the connected component is small enough we try clustering 
//			// using the FPT
//			if(cc.size() <= tcp.fpt_max_cc_size)
//			{
//				FPT fpt(cc,tcp.fpt_time_limit,tcp.fpt_step_size);
//				fpt.cluster(cr);
//			}		
//
//			if(cr.cost < 0)
//			{
//				// test value... should be cml setable
//				if(cc.size() < tcp.force_min_size_parallel){
//					#pragma omp critical
//					force_cc_small.push_back(i);
//				}else{
//					#pragma omp critical
//					force_cc_large.push_back(i);
//				}
//				cc.free();
//
//				continue;
//			}
//
//			// add clusters to solution
//			#pragma omp critical
//			{
//				addResult(cc,cr);
//			}
//
//			cc.free(true);
//		}
//	}
//	LOGI 
//		<< "Clustered " 
//		<< ccs.size()-force_cc_small.size() 
//		<< " connected components to optimality";
//
//}
//
//void TransClust::run_FORCE_concurrent(
//		std::deque<ConnectedComponent>& ccs,
//		std::deque<unsigned>& force_cc_small
//		)
//{
//	if(!force_cc_small.empty())
//	{
//		LOGI 
//			<< "Clustering " 
//			<< force_cc_small.size() 
//			<< " connected components using FORCE";
//
//		#pragma omp parallel
//		{
//			#pragma omp for schedule(dynamic)
//			for(unsigned i = 0; i < force_cc_small.size();i++)
//			{
//				ConnectedComponent& cc = ccs.at(force_cc_small.at(i)); 
//				cc.load();
//				RES::ClusteringResult cr;
//				cr.cost = -1;
//
//				LOGD << "clustering with FORCE";
//				// init position array
//				std::vector<std::vector<float>> pos;
//				pos.resize(cc.size(), std::vector<float>(tcp.dim,0));
//
//				// layout
//				FORCE::layout(
//						cc, 
//						pos, 
//						tcp.p, 
//						tcp.f_att, 
//						tcp.f_rep, 
//						tcp.R, 
//						tcp.start_t, 
//						tcp.dim,
//						tcp.seed);
//
//				// partition
//				FORCE::partition(
//						cc, 
//						pos, 
//						cr, 
//						tcp.d_init, 
//						tcp.d_maximal, 
//						tcp.s_init, 
//						tcp.f_s);
//
//
//				#pragma omp critical
//				{
//					addResult(cc,cr);
//				}
//				cc.free(true);
//			}
//		}
//	}
//
//}
//
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
