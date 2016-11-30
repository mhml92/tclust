#ifndef TRANSCLUST_HPP
#define TRANSCLUST_HPP
#include <string>
#include <queue>
#include <deque>
#include <vector>
#include <map>
#include <cmath>
#include <plog/Log.h>
#include "transclust/Common.hpp"
#include "transclust/InputParser.hpp"
#include "transclust/ConnectedComponent.hpp"
#include "transclust/ClusteringResult.hpp"
#ifndef NDEBUG
#	include "transclust/DEBUG.hpp"
#	define DEBUG_COST(cc,clustering,cost) DEBUG::test_cost(cc,clustering,cost);
#else
#	define DEBUG_COST(cc,clustering,cost) {} 
#endif

class TransClust{

	public:
		TransClust(
				TCC::TransClustParams& _tcp,
				unsigned process_id = 0
				);

		void cluster(
				std::deque<ConnectedComponent>& css,
				RES::ClusteringResult& result
				);
	private:
		void addResult(
				ConnectedComponent& cc,
				RES::ClusteringResult& cr,
				RES::ClusteringResult& result);

		//void run_fpt(
		//		std::deque<ConnectedComponent>& ccs,
		//		std::deque<unsigned>& force_cc_small,
		//		std::deque<unsigned>& force_cc_large
		//		);

		//void run_FORCE_concurrent(
		//		std::deque<ConnectedComponent>& ccs,
		//		std::deque<unsigned>& force_cc_small
		//		);

		//void run_FORCE_parallel(
		//		std::deque<ConnectedComponent>& ccs,
		//		std::deque<unsigned>& force_cc_large
		//		);
		TCC::TransClustParams tcp;
		unsigned process_id;
		//std::string filename;
		//float total_cost = 0;
		//std::deque<ConnectedComponent> ccs;
		//std::deque<std::string> id2name; 
		//std::deque<std::deque<unsigned>> clusters;
		
};
#endif
