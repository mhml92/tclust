#include "transclust/ConnectedComponent.hpp"

ConnectedComponent::ConnectedComponent(
		unsigned id,
		long _num_positive_edges,
		TCC::TransClustParams& _tcp)
	:
		cc_id(id),
		tcp(_tcp),
		num_positive_edges(_num_positive_edges)
{
		pcost_map = new external_cost_map_type((external_cost_map_type::node_block_type::raw_size)*3,(external_cost_map_type::leaf_block_type::raw_size)*3);

		if(num_positive_edges == std::numeric_limits<long>::lowest()){
			num_positive_edges = 0;
		}
}
