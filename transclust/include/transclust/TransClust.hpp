#ifndef TRANSCLUST_HPP
#define TRANSCLUST_HPP
#include <string>
#include <queue>
#include <vector>
#include <map>
#include <cmath>
#include "transclust/ConnectedComponent.hpp"
#include "transclust/ClusteringResult.hpp"
#include "transclust/FPT.hpp"
#include "transclust/Result.hpp"

class TransClust{

	public:
		TransClust(
			const std::string& filename,
	  	  	 bool       use_custom_fallback = false,
	  	  	 double     sim_fallback        = 0.0,
	  	  	 bool       deafult_interval    = true,
	  	  	 double     th_min              = 0.0,
	  	  	 double     th_max              = 100,
	  	  	 double     th_step             = 1.0,
	  	  	 double     p                   = 1.0,
	  	  	 double     f_att               = 100.0,
	  	  	 double     f_rep               = 100.0,
	  	  	 unsigned   R                   = 100,
	  	  	 unsigned   dim                 = 3,
	  	  	 double     start_t             = 100,
	  	  	 double     d_init              = 0.01,
	  	  	 double     d_maximal           = 5.0,
	  	  	 double     s_init              = 0.01,
	  	  	 double     f_s                 = 0.01,
	  	  	 double     fpt_time_limit      = 20,
	  	  	 double     fpt_max_cost        = 5000,
	  	  	 double     fpt_step_size       = 10,
	  	  	 bool       disable_force       = false,
	  	  	 bool       disable_fpt         = false,
			FileType file_type              = FileType::LEGACY
		);

		TransClust(
			 std::vector<std::vector<double>>& simmatrix,
	  	  	 bool       use_custom_fallback = false,
	  	  	 double     sim_fallback        = 0.0,
	  	  	 bool       deafult_interval    = true,
	  	  	 double     th_min              = 0.0,
	  	  	 double     th_max              = 100,
	  	  	 double     th_step             = 1.0,
	  	  	 double     p                   = 1.0,
	  	  	 double     f_att               = 100.0,
	  	  	 double     f_rep               = 100.0,
	  	  	 unsigned   R                   = 100,
	  	  	 unsigned   dim                 = 3,
	  	  	 double     start_t             = 100,
	  	  	 double     d_init              = 0.01,
	  	  	 double     d_maximal           = 5.0,
	  	  	 double     s_init              = 0.01,
	  	  	 double     f_s                 = 0.01,
	  	  	 double     fpt_time_limit      = 20,
	  	  	 double     fpt_max_cost        = 5000,
	  	  	 double     fpt_step_size       = 10,
	  	  	 bool       disable_force       = false,
	  	  	 bool       disable_fpt         = false
		);
		clustering cluster();
	private:

		inline double round(double d){
			return std::rint((d)*100000)/100000;
		};
		
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
		double fpt_max_cost;
		double fpt_step_size;

		bool disable_force;
		bool disable_fpt; 

		std::queue<ConnectedComponent> ccs;
		std::vector<std::string> id2object;
};
#endif
