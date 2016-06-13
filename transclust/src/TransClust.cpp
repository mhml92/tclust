#include "TransClust.hpp"
#include "ConnectedComponent.hpp"
#include "FindConnectedComponents.hpp"
#include "FORCE.hpp"
#include "FPT.hpp"
#include "ClusteringResult.hpp"
#include "Result.hpp"
#include <plog/Log.h>

TransClust::TransClust(
		const std::string& filename,
		const std::string ref,
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
	LOG_VERBOSE << "Reading input file " << filename;

	// Read input similarity file
	ConnectedComponent sim_matrix(filename);
	id2object = sim_matrix.getObjectNames();
		threshold_min = sim_matrix.getMinSimilarity();
		threshold_max = sim_matrix.getMaxSimilarity();
		threshold_step = (threshold_max-threshold_min)/10;
	FCC::findConnectedComponents(sim_matrix,ccs,threshold_min);

	//FCC::findConnectedComponents(sim_matrix,ccs,32.0);
}


void TransClust::cluster()
{

	Result result(id2object);

	while(!ccs.empty()){
		ConnectedComponent& cc = ccs.front();   
		LOG_VERBOSE << "Proccessing ConnectedComponent with id: " << cc.getId();
		LOG_VERBOSE << "CC size: " << cc.size();

		ClusteringResult cr;
		// set initial cost to negativ, indicating 'no solution found (yet)'
		cr.cost = -1;

		// if cost is less than 0 no solution has been found
		/*
		if(cr.cost < 0){ 
			// init position array
			std::vector<std::vector<float>> pos;
			pos.resize(cc.size(), std::vector<float>(dim,0));

			// layout
			FORCE::layout(cc,pos,p,f_att,f_rep,R,start_t,dim);
			//FORCE::DEBUG_position(cc,pos,cc.getThreshold());

			FORCE::partition(cc,pos,cr,d_init,d_maximal,s_init,f_s);
		}
		*/
		if(cc.size() <= maxFPTSize){
			LOG_VERBOSE << "Clustering cc-" << cc.getId() << " with FTP";
			FPT::cluster(cc,cr,time_limit,maxK,stepSize);   
		}
		result.add(cc,cr);
		//FORCE::DEBUG_linking(res.getClusters(),pos,cc.getThreshold(),cc.getId());
		float new_threshold = cc.getThreshold()+threshold_step;

		LOG_VERBOSE << "Increasing threshold to " << new_threshold;
		if(new_threshold <= threshold_max){
			FCC::findConnectedComponents(cc,ccs,new_threshold);
		}
		ccs.pop();
	}
	result.dump();
}



