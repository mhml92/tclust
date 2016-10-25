#include "transclust/ConnectedComponent.hpp"

//ConnectedComponent::ConnectedComponent(
//		const std::string &filename,
//		bool use_custom_fallback,
//		float sim_fallback,
//		std::string ft)
ConnectedComponent::ConnectedComponent(
		const std::string &filename,
		TCC::TransClustParams& tcp)
	:
		//m(filename,use_custom_fallback,sim_fallback,ft),
		id(getNewId()),
		threshold(0.0),
		m(filename,tcp,id)
{ 
	init_normalization_context(tcp);
}

ConnectedComponent::ConnectedComponent(
		std::vector<float>& sim_matrix_1d,
		unsigned num_o,
		TCC::TransClustParams& tcp)
	:
		id(getNewId()),
		threshold(0.0),
		m(sim_matrix_1d,num_o,id)
{
	init_normalization_context(tcp);
}

ConnectedComponent::ConnectedComponent(
		ConnectedComponent &cc,
		const std::vector<unsigned> &objects,
		float th,
		TCC::TransClustParams& tcp)
	:
		id(getNewId()),
		threshold(th),
		m(cc.getMatrix(),objects,id,th,TCC::round(threshold-cc.getThreshold()))
{
	init_normalization_context(tcp);
}

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

