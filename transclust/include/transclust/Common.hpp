#ifndef TRANSCLUST_COMMON_HPP
#define TRANSCLUST_COMMON_HPP
#include <string>
#include <cmath>
#include <vector>
#include <deque>

// TransClust Commons
namespace TCC
{
	/* Config struct with fluint interface */
	struct TransClustParams {
		float threshold = 0.0;

		// force parameters
		float p = 1.0;
		float f_att = 100.0;
		float f_rep = 100.0;
		unsigned R = 100;
		unsigned dim = 3;
		float start_t = 100;

		// geometric linking parameters
		float d_init = 0.01;
		float d_maximal = 5.0;
		float s_init = 0.01;
		float f_s = 0.01;

		// fpt parameters
		float fpt_time_limit = 0.02;
		float fpt_step_size = 500;
		unsigned fpt_max_edge_conflicts = 500;
		unsigned fpt_max_cc_size = 50;

		// misc
		float sim_fallback = 0.0;
		bool disable_force = false;
		bool disable_fpt = false;
		std::string normalization = "ABSOLUTE";
		std::string tmp_dir = "/tmp/tclust/";
		unsigned seed = 42;
		unsigned max_ram = 1024;

		// debug
		bool debug_cost_only = false;


		TransClustParams& set_threshold(float val){threshold = val;return *this;}

		TransClustParams& set_p(float val){p = val;return *this;}
		TransClustParams& set_f_att(float val){f_att = val;return *this;}
		TransClustParams& set_f_rep(float val){f_rep = val;return *this;}
		TransClustParams& set_R(float val){R = val;return *this;}
		TransClustParams& set_dim(float val){dim = val;return *this;}
		TransClustParams& set_start_t(float val){start_t = val;return *this;}

		TransClustParams& set_d_init(float val){d_init = val;return *this;}
		TransClustParams& set_d_maximal(float val){d_maximal = val;return *this;}
		TransClustParams& set_s_init(float val){s_init = val;return *this;}
		TransClustParams& set_f_s(float val){f_s = val;return *this;}

		TransClustParams& set_fpt_time_limit(float val){fpt_time_limit = val;return *this;}
		TransClustParams& set_fpt_max_edge_conflicts(unsigned val){fpt_max_edge_conflicts = val;return *this;}
		TransClustParams& set_fpt_step_size(float val){fpt_step_size = val;return *this;}
		TransClustParams& set_fpt_max_cc_size(unsigned val){fpt_max_cc_size = val;return *this;}

		TransClustParams& set_sim_fallback(float val){sim_fallback = val;return *this;}
		TransClustParams& set_disable_force(bool val){disable_force = val;return *this;}
		TransClustParams& set_disable_fpt(bool val){disable_fpt = val;return *this;}
		TransClustParams& set_normalization(std::string val){normalization = val;return *this;}
		TransClustParams& set_tmp_dir(std::string val){tmp_dir = val;return *this;}
		TransClustParams& set_seed(bool val){seed = val;return *this;}

		TransClustParams& set_debug_cost_only(bool val){debug_cost_only = val;return *this;}
	};

	
	inline uint64_t fuse(unsigned i,unsigned j){
		if(i > j){ std::swap(i,j);}
		return (( (std::uint64_t)j << 32) | (std::uint64_t)i);
	}
	inline std::pair<unsigned,unsigned> defuse(long l){
		uint64_t l64 = (uint64_t)l;
		uint32_t i = (uint32_t)(l64 >> 32);
		uint32_t j = (uint32_t)(l64);
		return std::make_pair(i,j);
	}

	inline float round(float f){
		return std::rint(f*100000.0f)/100000.0f;
	}

	inline float dist(std::vector<std::vector<float>>& pos,unsigned i, unsigned j)
	{
		float res = 0;
		for(unsigned d = 0; d < pos[0].size(); d++){
			float side = pos[i][d] - pos[j][d];
			res += side*side;
		}
		return std::sqrt(res);
	}

	/**
	 * Calcualte the number of elements there would be in a symetric matrix 
	 * without the diagonal and one of the (redundant) halfs.
	 */
	inline long calc_num_sym_elem(unsigned n){
		return (((long)n*n)-n)/2;
	}
}
#endif
