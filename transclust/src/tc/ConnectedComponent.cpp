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
	tmp_file_path = getTmpFilePath().string();
	if(num_positive_edges == std::numeric_limits<long>::lowest()){
		num_positive_edges = 0;
	}
}
