#include <cmath>
#include "ConnectedComponent.hpp"

ConnectedComponent::ConnectedComponent(
		const std::string &filename,
		float sim_fallback)
	:
		m(filename,sim_fallback),
		threshold(0),
		normalization_context(
			std::fmax(
				fabs(m.getMaxValue()-threshold),
				fabs(m.getMinValue()-threshold)
			)
		),
		cost(-1),
		id(getNewId())
{ }


ConnectedComponent::ConnectedComponent(
		const ConnectedComponent &cc,
		const std::vector<unsigned> &objects,
		float th)
	:
		m(cc.getMatrix(),objects),
		threshold(th),
		normalization_context(
			std::fmax(
				fabs(m.getMaxValue()-threshold),
				fabs(m.getMinValue()-threshold)
			)
		),
		cost(-1),
		id(getNewId())
{
	

}

/*
	ConnectedComponent::ConnectedComponent(
	const std::vector<std::vector<float>>& pos,
	float th)
	:
	m(pos),
	threshold(th),
	normalization_context(0)
	{ }
	*/

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

