#include <plog/Log.h>
#include "transclust/TransClust.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/FindConnectedComponents.hpp"
#include "transclust/FORCE.hpp"
#include "transclust/FPT.hpp"
#include "transclust/ClusteringResult.hpp"
#include "transclust/Result.hpp"

TransClust::TransClust(
		const std::string& filename,
		TCC::TransClustParams& _tcp
		)
{
   tcp = _tcp;
	// Read input similarity file
	ConnectedComponent sim_matrix(filename,tcp);
	sim_matrix.load();
	id2object = sim_matrix.getObjectNames();
	if(tcp.use_default_interval){
		tcp.th_min = sim_matrix.getMinSimilarity();
		tcp.th_max = sim_matrix.getMaxSimilarity();
		tcp.th_step = TCC::round(tcp.th_max-tcp.th_min)/100;
	}
	FCC::findConnectedComponents(tcp,sim_matrix,ccs,TCC::round(tcp.th_min));
	sim_matrix.free();
}

TransClust::TransClust(
      std::vector<float>& sim_matrix_1d,
      unsigned num_o,
		TCC::TransClustParams& _tcp
		)
{
	tcp = _tcp;
	ConnectedComponent sim_matrix(sim_matrix_1d,num_o,tcp);
	sim_matrix.load();
	id2object = sim_matrix.getObjectNames();
	if(tcp.use_default_interval){
		tcp.th_min = sim_matrix.getMinSimilarity();
		tcp.th_max = sim_matrix.getMaxSimilarity();
		tcp.th_step = TCC::round(tcp.th_max-tcp.th_min)/100;
	}
	FCC::findConnectedComponents(tcp,sim_matrix,ccs,TCC::round(tcp.th_min));
	sim_matrix.free();
}


clustering TransClust::cluster()
{

	Result result(id2object);

	while(!ccs.empty()){
		ConnectedComponent& cc = ccs.front();

		if(cc.getThreshold() > log_current_threshold){
			log_current_threshold = cc.getThreshold();
			LOGI << "Clustering for threshold: " << log_current_threshold;
		};

		cc.load();

		ClusteringResult cr;

		// set initial cost to negativ, indicating 'no solution found (yet)'
		cr.cost = -1;

		// if cc is at least a conflict tripple
		if(cc.size() > 2){

			/**********************************************************************
			 * Cluster using FORCE
			 *********************************************************************/
			if(!tcp.disable_force){
				// init position array
				std::vector<std::vector<float>> pos;
				pos.resize(cc.size(), std::vector<float>(tcp.dim,0));

				// layout
				FORCE::layout(cc, pos, tcp.p, tcp.f_att, tcp.f_rep, tcp.R, tcp.start_t, tcp.dim);
				// partition
				FORCE::partition(cc, pos, cr, tcp.d_init, tcp.d_maximal, tcp.s_init, tcp.f_s);
			}

			/**********************************************************************
			 * Cluster using FPT
			 *********************************************************************/
			if(cr.cost <= tcp.fpt_max_cost && !tcp.disable_fpt){

				float tmp_force_cost = cr.cost;
				FPT fpt(cc,tcp.fpt_time_limit,tcp.fpt_step_size,cr.cost+1);
				fpt.cluster(cr);

				if(cr.cost < 0){
					cr.cost = tmp_force_cost;
				}
			}
		}else{
			// cc consist of 1 or 2 nodes and is a cluster
			cr.cost = 0;
			cr.membership = std::vector<unsigned>(cc.size(),0);
		}

		result.add(cc,cr);
		float new_threshold = TCC::round(cc.getThreshold()+tcp.th_step);

		if(new_threshold <= tcp.th_max){
			FCC::findConnectedComponents(tcp,cc,ccs,new_threshold);
		}
		
		cc.free();
		ccs.pop();
	}
	return result.get();
}
