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
		bool use_custom_fallback,
		double sim_fallback,
		bool use_custom_range,
		double th_min,
		double th_max,
		double th_step,
		double p,
		double f_att,
		double f_rep,
		unsigned R,
		unsigned dim,
		double start_t,
		double d_init,
		double d_maximal,
		double s_init,
		double f_s,
		double fpt_time_limit,
		double fpt_max_cost,
		bool disable_force,
		bool disable_fpt 
		)
	:
		use_custom_fallback(use_custom_fallback),
		sim_fallback(sim_fallback),
		use_custom_range(use_custom_range),
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
		f_s(f_s),
		fpt_time_limit(fpt_time_limit),
		fpt_max_cost(fpt_max_cost),
		disable_force(disable_force),
		disable_fpt(disable_fpt)
{
	LOG_DEBUG << "Reading input file " << filename;

	// Read input similarity file
	ConnectedComponent sim_matrix(filename,use_custom_fallback,sim_fallback);
	id2object = sim_matrix.getObjectNames();

	if(!use_custom_range){
		threshold_min = sim_matrix.getMinSimilarity();
		threshold_max = sim_matrix.getMaxSimilarity();
		threshold_step =(std::rint((threshold_max-threshold_min)*100000)/100000)/100;
	}	
	//std::cout << std::setprecision(std::numeric_limits<double>::digits10 + 1) << threshold_step << std::endl;

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
				//FORCE::DEBUG_position(cc,pos,cc.getThreshold());
				// partition
				FORCE::partition(cc,pos,cr,d_init,d_maximal,s_init,f_s);

				LOG_DEBUG << "FORCE found solution with " << DEBUG::num_clusters(cr.membership) 
					<< " clusters and  cost: " << cr.cost
					<< ", cost should be: " << DEBUG::calculate_cluster_cost(cc,cr);

			}
			/**********************************************************************
			 * Cluster using FPT
			 *********************************************************************/
			if(cr.cost <= fpt_max_cost && !disable_fpt){
				LOG_DEBUG << "Cost small enough to try and solve it with FPT";
				LOG_VERBOSE << "Clustering cc-" << cc.getId() << " with FTP";
				// temp hack
				double tmp_force_cost = cr.cost;
				FPT fpt(cc,fpt_time_limit,5,cr.cost+1); // DEBUG TEST 1000000 mockInf
				//New_FPT fpt(cc,cr,time_limit,cr.cost/10);
				fpt.cluster(cr);   

				// temp hack continued
				if(cr.cost < 0){
					cr.cost = tmp_force_cost;
				}else{
					LOG_DEBUG << "FPT found solution with " 
						<< DEBUG::num_clusters(cr.membership) 
						<< " clusters and  cost: " << cr.cost
						<< ", cost should be: " << DEBUG::calculate_cluster_cost(cc,cr);
				}
			}
		}else{
			LOG_VERBOSE << "CC is only one or two nodes";
			// cc consist of 1 or 2 nodes and is a cluster
			cr.cost = 0;
			cr.membership = std::vector<unsigned>(cc.size(),0);
		}
		result.add(cc,cr);
		//FORCE::DEBUG_linking(res.getClusters(),pos,cc.getThreshold(),cc.getId());
		double new_threshold = std::rint((cc.getThreshold()+threshold_step)*100000)/100000;

		LOG_VERBOSE << "Increasing threshold to " << new_threshold;

		//std::cout  
		//	<< "1 found new threshold: " 
		//	<<std::setprecision(std::numeric_limits<double>::digits10 ) 
		//	<<  new_threshold << std::endl;
		
		//std::cout 
		//	<< "2 threshold_max: " 
		//	<< std::setprecision(std::numeric_limits<double>::digits10) 
		//	<< threshold_max << std::endl;
		
		if(new_threshold <= threshold_max){
			//std::cout 
			//	<< std::setprecision(std::numeric_limits<double>::digits10) 
			//	<< "3 new threshold: " << new_threshold << std::endl;
			FCC::findConnectedComponents(cc,ccs,new_threshold);
		}
		ccs.pop();
	}

	// print vars
	//std::cout << "// sim_fallback " << sim_fallback<< std::endl;
	//std::cout << "// th_min       " << threshold_min<< std::endl;
	//std::cout << "// th_max       " << threshold_max<< std::endl;
	//std::cout << "// th_step      " << threshold_step<< std::endl;
	//std::cout << "// p            " << p<< std::endl;
	//std::cout << "// f_att        " << f_att<< std::endl;
	//std::cout << "// f_rep        " << f_rep<< std::endl;
	//std::cout << "// R            " << R<< std::endl;
	//std::cout << "// dim          " << dim<< std::endl;
	//std::cout << "// start_t      " << start_t<< std::endl;
	//std::cout << "// d_init       " << d_init<< std::endl;
	//std::cout << "// d_maximal    " << d_maximal<< std::endl;
	//std::cout << "// s_init       " << s_init<< std::endl;
	//std::cout << "// f_s          " << f_s<< std::endl;
	result.dump();
}