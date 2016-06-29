#include "TransClust.hpp"
#include "ConnectedComponent.hpp"
#include "FindConnectedComponents.hpp"
#include "FORCE.hpp"
//#include "New_FPT.hpp"
#include "FPT.hpp"
#include "ClusteringResult.hpp"
#include "Result.hpp"
#include "DEBUG.hpp"
#include <plog/Log.h>

TransClust::TransClust(
		const std::string& filename,
		const std::string ref,
		float sim_fallback,
		float th_min,
		float th_max,
		float th_step,
		float p,
		float f_att,
		float f_rep,
		unsigned R,
		unsigned dim,
		float start_t,
		float d_init,
		float d_maximal,
		float s_init,
		float f_s
		)
	:
		ref(ref),
		threshold_min(th_min),
		threshold_max(th_max),
		threshold_step(th_step),
		p(p),
		f_att(f_att),
		f_rep(f_rep),
		R(R),
		start_t(start_t),
		dim(dim),
		d_init(d_init),
		d_maximal(d_maximal),
		s_init(s_init),
		f_s(f_s)
{
	LOG_DEBUG << "Reading input file " << filename;

	// Read input similarity file
	ConnectedComponent sim_matrix(filename,sim_fallback);
	id2object = sim_matrix.getObjectNames();

	if(true){
		threshold_min = 0;//sim_matrix.getMinSimilarity();
		threshold_max = 100;//sim_matrix.getMaxSimilarity();
		threshold_step = 1;//(threshold_max-threshold_min)/100;
	}else{

		threshold_min = sim_matrix.getMinSimilarity();
		threshold_max = sim_matrix.getMaxSimilarity();
		threshold_step =(threshold_max-threshold_min)/100;
	}

	FCC::findConnectedComponents(sim_matrix,ccs,threshold_min);
}


void TransClust::cluster()
{

	Result result(id2object);

	while(!ccs.empty()){
		ConnectedComponent& cc = ccs.front();   
		LOG_DEBUG << "Processing ConnectedComponent with id: " << cc.getId() 
			<< " of size: " << cc.size() 
			<< " and threshold: " << cc.getThreshold() ;

		ClusteringResult cr;
		// set initial cost to negativ, indicating 'no solution found (yet)'
		cr.cost = -1;

		// if cc is at least a conflict tripple
		if(cc.size() > 2){
			/**********************************************************************
			 * Cluster using FORCE
			 *********************************************************************/
			
			// init position array
			std::vector<std::vector<float>> pos;
			pos.resize(cc.size(), std::vector<float>(dim,0));

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
			//FORCE::DEBUG_position(cc,pos,cc.getThreshold());
			// partition
			FORCE::partition(cc,pos,cr,d_init,d_maximal,s_init,f_s);
			
			LOG_DEBUG << "FORCE found solution with " << DEBUG::num_clusters(cr.membership) 
				<< " clusters and  cost: " << cr.cost
				<< ", cost should be: " << DEBUG::calculate_cluster_cost(cc,cr);

			/**********************************************************************
			 * Cluster using FPT
			 *********************************************************************/
			//if(cr.cost <= maxFPTCost){
			//	LOG_DEBUG << "Cost small enough to try and solve it with FPT";
			//	LOG_VERBOSE << "Clustering cc-" << cc.getId() << " with FTP";
			//	// temp hack
			//	float tmp_force_cost = cr.cost;
			//	FPT fpt(cc,time_limit,5,cr.cost+1); // DEBUG TEST 1000000 mockInf
			//	//New_FPT fpt(cc,cr,time_limit,cr.cost/10);
			//	fpt.cluster(cr);   

			//	// temp hack continued
			//	if(cr.cost < 0){
			//		cr.cost = tmp_force_cost;
			//	}else{
			//		LOG_DEBUG << "FPT found solution with " 
			//			<< DEBUG::num_clusters(cr.membership) 
			//			<< " clusters and  cost: " << cr.cost
			//			<< ", cost should be: " << DEBUG::calculate_cluster_cost(cc,cr);
			//	}
			//}
		}else{
			LOG_VERBOSE << "CC is only one or two nodes";
			// cc consist of 1 or 2 nodes and is a cluster
			cr.cost = 0;
			cr.membership = std::vector<unsigned>(cc.size(),0);
		}
		result.add(cc,cr);
		//FORCE::DEBUG_linking(res.getClusters(),pos,cc.getThreshold(),cc.getId());
		float new_threshold = cc.getThreshold()+threshold_step;

		LOG_VERBOSE << "Increasing threshold to " << new_threshold;
		if(new_threshold <= threshold_max){
			FCC::findConnectedComponents(cc,ccs,new_threshold);
		}
		ccs.pop();
		LOG_DEBUG << "----------------------------------------------";
	}
	result.dump();
}
