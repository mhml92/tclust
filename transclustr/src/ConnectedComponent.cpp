#include "transclust/ConnectedComponent.hpp"

//ConnectedComponent::ConnectedComponent(
//		const std::string &filename,
//		bool use_custom_fallback,
//		double sim_fallback,
//		std::string ft)
ConnectedComponent::ConnectedComponent(
		const std::string &filename,
		TCC::TransClustParams& tcp
   )
	:
		id(getNewId()),
		m(filename,tcp,id),
		threshold(0.0),
		cost(-1)
{
	init_normalization_context(tcp);
}

ConnectedComponent::ConnectedComponent(
		std::vector<double>& sim_matrix_1d,
		unsigned num_o,
		TCC::TransClustParams& tcp)//,
		//bool use_custom_fallback,
		//double sim_fallback)
	:
		id(getNewId()),
		m(sim_matrix_1d,num_o,id),//,use_custom_fallback,sim_fallback),
		threshold(0.0),
		cost(-1)
{
	init_normalization_context(tcp);
}

ConnectedComponent::ConnectedComponent(
		const ConnectedComponent &cc,
		const std::vector<unsigned> &objects,
		double th,
		TCC::TransClustParams& tcp)
	:
		id(getNewId()),
		m(cc.getMatrix(),objects,id),
		threshold(th),
		cost(-1.0)
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

