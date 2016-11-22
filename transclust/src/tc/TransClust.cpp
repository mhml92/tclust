#include <chrono>
#ifdef _OPENMP
#	include <omp.h>
#endif
#include "transclust/TransClust.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/InputParser.hpp"
#include "transclust/FORCE.hpp"
#include "transclust/FPT.hpp"
#include "transclust/ClusteringResult.hpp"
#include "transclust/Common.hpp"

TransClust::TransClust(
		const std::string& filename,
		TCC::TransClustParams& _tcp
		)
	:
		tcp(_tcp),
		ip(filename,tcp),
		ccs(),
		result()
{ }

RES::Clustering TransClust::cluster()
{

	// read input file and initialize connected components
	ip.getConnectedComponents(ccs,result);
	result.threshold = tcp.threshold;

	// array holding indexes for connected components which could not succesfully
	// be clustered using the FPT algorithm but is to small enough to be run 
	// single threaded
	std::deque<unsigned> force_cc_small;
	std::deque<unsigned> force_cc_large;

	/////////////////////////////////////////////////////////////////////////////
	// Try to cluster all connected components using the FPT algorithm
	// If a connected component could not be clusterd it will clusterd with 
	// FORCE
	/////////////////////////////////////////////////////////////////////////////
	if(!tcp.disable_fpt)
	{
		LOGI << "Clustering using FPT algorithm";
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

				// If the connected compoent was already found to be transitive 
				// then we can skip any analysis and just add it to the result
				if(!cc.isTransitive())
				{
					// if the connected component is small enough we try clustering 
					// using the FPT
					if(cc.size() <= tcp.fpt_max_cc_size)
					{
						FPT fpt(cc,tcp.fpt_time_limit,tcp.fpt_step_size);
						fpt.cluster(cr);
					}		

					if(cr.cost < 0)
					{
						// test value... should be cml setable
						if(cc.size() < tcp.force_min_size_parallel){
							#pragma omp critical
							force_cc_small.push_back(i);
						}else{
							#pragma omp critical
							force_cc_large.push_back(i);
						}

						cc.free();

						continue;
					}
				}

				// add clusters to solution
				#pragma omp critical
				{
					addResult(cc,cr);
				}

				cc.free(true);
			}
		}
		LOGI 
			<< "Clustered " 
			<< ccs.size()-force_cc_small.size() 
			<< " connected components to optimality";

	}else{
		for(unsigned i = 0; i < ccs.size();i++)
		{
			if(ccs.at(i).size() < tcp.force_min_size_parallel){
				force_cc_small.push_back(i);
			}else{
				force_cc_large.push_back(i);
			}
		
		}
		//force_cc_small.resize(ccs.size());
		//std::iota(force_cc_small.begin(),force_cc_small.end(),0);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Cluster using FORCE
	/////////////////////////////////////////////////////////////////////////////
	if(!force_cc_small.empty())
	{
		LOGI 
			<< "Clustering " 
			<< force_cc_small.size() 
			<< " connected components using FORCE";

		#pragma omp parallel
		{
			#pragma omp for schedule(dynamic)
			for(unsigned i = 0; i < force_cc_small.size();i++)
			{
				ConnectedComponent& cc = ccs.at(force_cc_small.at(i)); 

				RES::ClusteringResult cr;
				if(!cc.isTransitive())
				{
					cc.load();
					LOGD << "clustering with FORCE";
					// init position array
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
				
				#pragma omp critical
				{
					addResult(cc,cr);
				}


				cc.free(true);
			}
		}
	}
	if(!force_cc_large.empty())
	{
		LOGI 
			<< "Clustering " 
			<< force_cc_large.size() 
			<< " connected components using parallel FORCE";

			for(unsigned i = 0; i < force_cc_large.size();i++)
			{
				ConnectedComponent& cc = ccs.at(force_cc_large.at(i)); 

				RES::ClusteringResult cr;
				if(!cc.isTransitive())
				{
					cc.load();
					LOGD << "clustering with FORCE";
					// init position array
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
				}				
				
				addResult(cc,cr);
				cc.free(true);
			}
	}
	return result;
}
