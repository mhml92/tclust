#ifndef TRANSCLUST_COMMON_HPP
#define TRANSCLUST_COMMON_HPP
#include <string>
#include <cmath>

// TransClust Commons
namespace TCC
{
	/* Config struct with fluint interface */
	struct TransClustParams {
	  	 std::string   file_type              =   "SIMPLE";
	  	 bool          use_custom_fallback    =   false;
	  	 double        sim_fallback           =   0.0;
	  	 bool          use_default_interval   =   true;
	  	 double        th_min                 =   0.0;
	  	 double        th_max                 =   100;
	  	 double        th_step                =   1.0;
	  	 double        p                      =   1.0;
	  	 double        f_att                  =   100.0;
	  	 double        f_rep                  =   100.0;
	  	 unsigned      R                      =   100;
	  	 unsigned      dim                    =   3;
	  	 double        start_t                =   100;
	  	 double        d_init                 =   0.01;
	  	 double        d_maximal              =   5.0;
	  	 double        s_init                 =   0.01;
	  	 double        f_s                    =   0.01;
	  	 double        fpt_time_limit         =   20;
	  	 double        fpt_max_cost           =   5000;
	  	 double        fpt_step_size          =   10;
	  	 bool          disable_force          =   false;
	  	 bool          disable_fpt            =   false;
	  	 unsigned      seed                   =   42;
	  	 std::string   tmp_dir                =   "/tmp/tclust/";
	  	 bool          external               =   false;
		TransClustParams& set_file_type(std::string val){file_type = val;return *this;}
		TransClustParams& set_use_custom_fallback(bool val){use_custom_fallback = val;return *this;}
		TransClustParams& set_sim_fallback(double val){sim_fallback = val;return *this;}
		TransClustParams& set_use_default_interval(bool val){use_default_interval= val;return *this;}
		TransClustParams& set_th_min(double val){th_min = val;return *this;}
		TransClustParams& set_th_max(double val){th_max = val;return *this;}
		TransClustParams& set_th_step(double val){th_step = val;return *this;}
		TransClustParams& set_p(double val){p = val;return *this;}
		TransClustParams& set_f_att(double val){f_att = val;return *this;}
		TransClustParams& set_f_rep(double val){f_rep = val;return *this;}
		TransClustParams& set_R(double val){R = val;return *this;}
		TransClustParams& set_dim(double val){dim = val;return *this;}
		TransClustParams& set_start_t(double val){start_t = val;return *this;}
		TransClustParams& set_d_init(double val){d_init = val;return *this;}
		TransClustParams& set_d_maximal(double val){d_maximal = val;return *this;}
		TransClustParams& set_s_init(double val){s_init = val;return *this;}
		TransClustParams& set_f_s(double val){f_s = val;return *this;}
		TransClustParams& set_fpt_time_limit(double val){fpt_time_limit = val;return *this;}
		TransClustParams& set_fpt_max_cost(double val){fpt_max_cost = val;return *this;}
		TransClustParams& set_fpt_step_size(double val){fpt_step_size = val;return *this;}
		TransClustParams& set_disable_force(bool val){disable_force = val;return *this;}
		TransClustParams& set_disable_fpt(bool val){disable_fpt = val;return *this;}
		TransClustParams& set_seed(bool val){seed = val;return *this;}
		TransClustParams& set_tmp_dir(std::string val){tmp_dir = val;return *this;}
		TransClustParams& set_external(bool val){external = val;return *this;}
	};

	inline double round(double d){
		return std::rint(d*100000)/100000;
	}
}
#endif
