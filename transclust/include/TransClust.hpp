#ifndef TRANSCLUST_HPP
#define TRANSCLUST_HPP
#include <string>
#include <queue>
#include <vector>
#include <map>
#include "ConnectedComponent.hpp"
#include "ClusteringResult.hpp"
//#include "New_FPT.hpp"
#include "FPT.hpp"
#include "Result.hpp"
#include <plog/Log.h>

class TransClust{



	public:
		TransClust(
			const std::string& filename,
			const std::string ref = "",
			float sim_fallback  = 0.0,
			float th_min      = 0.0,
			float th_max      = 100.0,
			float th_step     = 1.0,
			float p           = 0.5,
			float f_att       = 1.0,
			float f_rep       = 1.9,
			unsigned R        = 200,
			unsigned dim      = 3,
			float start_t     = 100,
			float d_init      = 0.01,
			float d_maximal   = 5,
			float s_init      = 0.01,
			float f_s         = 1.01
		);
		void cluster();
	private:
		std::string ref;

		// general vars
		float threshold_min;
		float threshold_max;
		float threshold_step;

		// Layout values
		float p;
		float f_att;
		float f_rep;
		unsigned R;
		float start_t;
		unsigned dim;

		// partitioning values
		float d_init;
		float d_maximal;
		float s_init;
		float f_s;

		// FPT values
		unsigned maxFPTCost = 30000;
		float time_limit = 50*60;



		std::queue<ConnectedComponent> ccs;

		std::vector<std::string> id2object;
};
#endif
