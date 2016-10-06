#include <Rcpp.h>
#include "transclust/TransClust.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/FindConnectedComponents.hpp"
#include "transclust/FORCE.hpp"
#include "transclust/FPT.hpp"
#include "transclust/ClusteringResult.hpp"
#include "transclust/Result.hpp"

TransClust::TransClust(
		const std::string& filename,
		std::string& file_type,
		TransClustParams& tcp
		)
{
   init(tcp);
	// Read input similarity file
	ConnectedComponent sim_matrix(filename,use_custom_fallback,sim_fallback,file_type);
	id2object = sim_matrix.getObjectNames();
	if(use_custom_range){
		threshold_min = sim_matrix.getMinSimilarity();
		threshold_max = sim_matrix.getMaxSimilarity();
		threshold_step = round(threshold_max-threshold_min)/100;
	}
	FCC::findConnectedComponents(sim_matrix,ccs,round(threshold_min));
}

TransClust::TransClust(
      std::vector<double>& sim_matrix_1d,
      unsigned num_o,
		TransClustParams& tcp
		)
{
   init(tcp);
	ConnectedComponent sim_matrix(sim_matrix_1d,num_o,use_custom_fallback,sim_fallback);
	id2object = sim_matrix.getObjectNames();
	if(use_custom_range){
		threshold_min = sim_matrix.getMinSimilarity();
		threshold_max = sim_matrix.getMaxSimilarity();
		threshold_step = round(threshold_max-threshold_min)/100;
	}
	FCC::findConnectedComponents(sim_matrix,ccs,round(threshold_min));
}


clustering TransClust::cluster()
{

	Result result(id2object);

	while(!ccs.empty()){
	   Rcpp::checkUserInterrupt();
		ConnectedComponent& cc = ccs.front();
      Rcpp::Rcout << "clustering cc with " << cc.size() << " objects and threshold " << cc.getThreshold() << std::endl;
		ClusteringResult cr;
		// set initial cost to negativ, indicating 'no solution found (yet)'
		cr.cost = -1;

		// if cc is at least a conflict tripple
		if(cc.size() > 2){
			if(!disable_force){
				/**********************************************************************
				 * Cluster using FORCE
				 *********************************************************************/
				// init position array
				std::vector<std::vector<double>> pos;
				pos.resize(cc.size(), std::vector<double>(dim,0));

				// layout
				FORCE::layout(
						cc,
						pos,
						p,
						f_att,
						f_rep,
						R,
						start_t,
						dim);
				// partition
				FORCE::partition(cc,pos,cr,d_init,d_maximal,s_init,f_s);

			}
			/**********************************************************************
			 * Cluster using FPT
			 *********************************************************************/
			if(cr.cost <= fpt_max_cost && !disable_fpt){
				// temp hack
				double tmp_force_cost = cr.cost;
				FPT fpt(cc,fpt_time_limit,fpt_step_size,cr.cost+1);
				fpt.cluster(cr);

				// temp hack continued
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
		double new_threshold = round(cc.getThreshold()+threshold_step);

		if(new_threshold <= threshold_max){
			FCC::findConnectedComponents(cc,ccs,new_threshold);
		}
		ccs.pop();
	}
	return result.get();
}

void TransClust::init(TransClustParams& tcp){
   // set class vars from tcp
   // general vars
   use_custom_fallback = tcp.use_custom_fallback;
   sim_fallback = tcp.sim_fallback;
   use_custom_range = tcp.use_default_interval;
   threshold_min = tcp.th_min;
   threshold_max = tcp.th_max;
   threshold_step = tcp.th_step;

   // Layout values
   p = tcp.p;
   f_att = tcp.f_att;
   f_rep = tcp.f_rep;
   R = tcp.R;
   start_t = tcp.start_t;
   dim = tcp.dim;

   // partitioning values
   d_init = tcp.d_init;
   d_maximal = tcp.d_maximal;
   s_init = tcp.s_init;
   f_s = tcp.f_s;

   // FPT values
   fpt_time_limit = tcp.fpt_time_limit;
   fpt_max_cost = tcp.fpt_max_cost;
   fpt_step_size = tcp.fpt_step_size;

   disable_force = tcp.disable_force;
   disable_fpt = tcp.disable_fpt;

   seed = tcp.seed;

}
