#include <chrono>
#include "transclust/TransClust.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/InputParser.hpp"
//#include "transclust/FindConnectedComponents.hpp"
#include "transclust/FORCE.hpp"
#include "transclust/FPT.hpp"
#include "transclust/ClusteringResult.hpp"
#include "transclust/Result.hpp"
#include "transclust/Common.hpp"

TransClust::TransClust(
		const std::string& filename,
		TCC::TransClustParams& _tcp
		)
	:
		tcp(_tcp),
		ip(filename,tcp),
		ccs(),
		id2name()
{ }

clustering TransClust::cluster()
{

	ip.getConnectedComponents(ccs,id2name);

	Result result(id2name);

	LOGI << "Starting clustering";

	unsigned num_nodes_clustered = 0;
	unsigned num_nodes = id2name.size(); 
	unsigned num_nodes_digits = 0;
	unsigned n = id2name.size();
	while(n > 0){ n /= 10; num_nodes_digits++; }

	//std::cout << "cc_size,num_conflicting_edges,force_time,force_cost,force_relative_cost,fpt_time,fpt_cost,fpt_relative_cost," << std::endl; 
	while(!ccs.empty()){
		ConnectedComponent& cc = ccs.front();
		cc.load();
		LOGI << "clustering cc of size: " << cc.size();

		ClusteringResult cr;


		// set initial cost to negativ, indicating 'no solution found (yet)'
		cr.cost = -1;

		// if cc is at least a conflict tripple
		if(!cc.isTransitive()){
			LOGD << "The cluster is not transitive!";

			//std::cout << cc.size() << ",";
			//std::cout << cc.getNumConflictingEdges() << ",";

			/**********************************************************************
			 * Cluster using FPT
			 *********************************************************************/
			if(!tcp.disable_fpt 
					&& (cc.getNumConflictingEdges() <= tcp.fpt_max_edge_conflicts
					|| cc.size() <= tcp.fpt_max_cc_size))
			{
				LOGD << "Clustering using fpt";

				float tmp_force_cost = cr.cost;
				//auto fpt_t1 = std::chrono::high_resolution_clock::now();
				FPT fpt(cc,tcp.fpt_time_limit,tcp.fpt_step_size,cc.getMaxCost());
				fpt.cluster(cr);
				//auto fpt_t2 = std::chrono::high_resolution_clock::now();
				//std::cout 
				//	<< std::chrono::duration_cast<std::chrono::nanoseconds>(fpt_t2-fpt_t1).count() 
				//	<< "," 
				//	<< cr.cost 
				//	<< "," 
				//	<< cr.cost/cc.getMaxCost()<<","; 

				if(cr.cost < 0){
					cr.cost = tmp_force_cost;
				}
			}		


			/**********************************************************************
			 * Cluster using FORCE
			 *********************************************************************/
			if(!tcp.disable_force && cr.cost < 0){
				LOGD << "clustering with FORCE";
				// init position array
				std::vector<std::vector<float>> pos;
				pos.resize(cc.size(), std::vector<float>(tcp.dim,0));

				// layout
				//auto force_t1 = std::chrono::high_resolution_clock::now();
				FORCE::layout(cc, pos, tcp.p, tcp.f_att, tcp.f_rep, tcp.R, tcp.start_t, tcp.dim);
				//auto layout_t2 = std::chrono::high_resolution_clock::now();
				
				// partition
				//auto partition_t1 = std::chrono::high_resolution_clock::now();
				FORCE::partition(cc, pos, cr, tcp.d_init, tcp.d_maximal, tcp.s_init, tcp.f_s);
				//auto force_t2 = std::chrono::high_resolution_clock::now();
				//std::cout 
				//	<< std::chrono::duration_cast<std::chrono::nanoseconds>(force_t2-force_t1).count() 
				//	<< "," 
				//	<< cr.cost 
				//	<< ","
				//	<< cr.cost/cc.getMaxCost() << ",";
			}

		}else{
			LOGD << "The cluster is already transitive!";
			// cc consist of 1 or 2 nodes and is a cluster
			cr.cost = 0;
			cr.membership = std::vector<unsigned>(cc.size(),0);
		}
		LOGD << "DONE the cost is: " << cr.cost;
		
		result.add(cc,cr);
		//std::cout << std::endl;
		num_nodes_clustered += cc.size();
		LOGI << std::setw(num_nodes_digits) << num_nodes_clustered << " out of " << std::setw(num_nodes_digits) << num_nodes <<" (" << std::floor(100*((float)num_nodes_clustered/num_nodes)) << " %)";
		
		cc.free();
		ccs.pop_front();
	}
	return result.get();
}
