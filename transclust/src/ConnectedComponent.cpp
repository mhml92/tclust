#include "ConnectedComponent.hpp"

ConnectedComponent::ConnectedComponent(
		const std::string &filename,
		bool use_custom_fallback,
		double sim_fallback)
	:
		m(filename,use_custom_fallback,sim_fallback),
		threshold(0.0),
		normalization_context(
			std::max(
				std::abs(m.getMaxValue()-threshold),
				std::abs(m.getMinValue()-threshold)
			)
		),
		cost(-1),
		id(getNewId())
{ }


ConnectedComponent::ConnectedComponent(
		const ConnectedComponent &cc,
		const std::vector<unsigned> &objects,
		double th)
	:
		m(cc.getMatrix(),objects),
		threshold(th),
		normalization_context(
			std::max(
				std::abs(m.getMaxValue()-threshold),
				std::abs(m.getMinValue()-threshold)
			)
		),
		cost(-1.0),
		id(getNewId())
{ }

void ConnectedComponent::dump()
{
	unsigned num_o = size();
	std::cout << num_o << std::endl;
	for(auto &name:m.getObjectNames()){
		std::cout << name << std::endl;
	}
	for(unsigned i = 0; i < num_o;i++){
		for(unsigned j = i+1;j < num_o;j++){
			std::cout << at(i,j,false)<< "\t";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

