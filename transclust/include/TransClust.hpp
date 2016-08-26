#ifndef TRANSCLUST_HPP
#define TRANSCLUST_HPP
#include <string>
#include <queue>
#include <vector>
#include <map>
#include "ConnectedComponent.hpp"
#include "ClusteringResult.hpp"
#include "FPT.hpp"
#include "Result.hpp"
#include <plog/Log.h>

class TransClust{

	public:
		TransClust(
			const std::string& filename,
	  	  	 bool       use_custom_fallback   =   true,
	  	  	 double     sim_fallback          =   0.0,
	  	  	 bool       deafult_interval      =   true,
	  	  	 double     th_min                =   0.0,
	  	  	 double     th_max                =   100,
	  	  	 double     th_step               =   1.0,
	  	  	 double     p                     =   1.0,
	  	  	 double     f_att                 =   100.0,
	  	  	 double     f_rep                 =   100.0,
	  	  	 unsigned   R                     =   100,
	  	  	 unsigned   dim                   =   3,
	  	  	 double     start_t               =   100,
	  	  	 double     d_init                =   0.01,
	  	  	 double     d_maximal             =   5.0,
	  	  	 double     s_init                =   0.01,
	  	  	 double     f_s                   =   0.01,
	  	  	 double     fpt_time_limit        =   60,
	  	  	 double     fpt_max_cost          =   5000,
	  	  	 bool       disable_force         =   false,
	  	  	 bool       disable_fpt           =   false
		);
		void cluster();
	private:
		std::string ref;

		// general vars
		bool use_custom_fallback;
		double sim_fallback;
		bool use_custom_range;
		double threshold_min;
		double threshold_max;
		double threshold_step;

		// Layout values
		double p;
		double f_att;
		double f_rep;
		unsigned R;
		double start_t;
		unsigned dim;

		// partitioning values
		double d_init;
		double d_maximal;
		double s_init;
		double f_s;

		// FPT values
		double fpt_time_limit;
		unsigned fpt_max_cost;

		bool disable_force;
		bool disable_fpt; 



		std::queue<ConnectedComponent> ccs;

		std::vector<std::string> id2object;
};
#endif
