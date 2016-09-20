#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>
#include <limits>
#include <cmath>
#include <utility>
#include <stdlib.h>
#include "transclust/TriangularMatrix.hpp"

TriangularMatrix::TriangularMatrix(
		const TriangularMatrix &m,
		const std::vector<unsigned> &objects)
{
	maxValue = std::numeric_limits<double>::lowest();
	minValue = std::numeric_limits<double>::max();

	unsigned num_o = objects.size();
	unsigned msize = ((num_o * num_o) - num_o) / 2;

	// indexes
	//index2ObjName.push_back(m.getObjectName(objects.at(0)));
	//index2ObjId.push_back(m.getObjectId(objects.at(0)));

	matrix.resize(msize);
	for (unsigned i = 0; i < num_o; i++)
	{
		// indexes
		index2ObjName.push_back(m.getObjectName(objects.at(i)));
		index2ObjId.push_back(m.getObjectId(objects.at(i)));
		for (unsigned j = i + 1; j < num_o; j++)
		{
			double val = m(objects.at(i), objects.at(j));
			if (maxValue < val){maxValue = val;}
			if (minValue > val){minValue = val;}
			
			matrix.at(index(i, j)) = val;
		}
	}
}

void TriangularMatrix::parseLegacySimDataFile(
		std::map<std::string, unsigned> &object2index,
		std::map<std::pair<std::string, std::string>, double> & sim_value,
		std::map<std::pair<std::string, std::string>, bool> &hasPartner,
		bool use_custom_fallback,
		double sim_fallback)
{
	// for each pair of object, tjeck if it has a partner and if so assign the 
	// minimum similarity value. If a pair only has similarity data in one 
	// direction then assign the defautl similarity value
	unsigned num_o = index2ObjName.size();
	for (unsigned i = 0; i < num_o; i++)
	{
		for (unsigned j = i + 1; j < num_o; j++)
		{
			std::pair<std::string, std::string> key;
			key = std::make_pair(index2ObjName[i], index2ObjName[j]);

			// default value if pair is missing
			double val = sim_fallback;
			if(!use_custom_fallback)
			{
				val = minValue;
			}

			// if the pair is in simvalue and it has a partner
			// choose the smallest sim value
			// else
			// use defualt value
			if(sim_value.find(key) != sim_value.end())
			{
				if(hasPartner.find(key) != hasPartner.end())
				{
					std::pair<std::string, std::string> rkey;
					rkey = std::make_pair(index2ObjName[j], index2ObjName[i]);
					val = std::min(sim_value[key],sim_value[rkey]);
				}
			}
			if(maxValue < val){maxValue = val;};
			matrix.at(index(i, j)) = val;
		}
	}

}

void TriangularMatrix::parseSimpleSimDataFile(
		std::map<std::string, unsigned> &object2index,
		std::map<std::pair<std::string, std::string>, double> & sim_value,
		std::map<std::pair<std::string, std::string>, bool> &hasPartner,
		bool use_custom_fallback,
		double sim_fallback)
{
	std::vector<std::pair<unsigned,unsigned>> positive_inf;
	unsigned num_o = index2ObjName.size();
	#pragma omp parallel for 
	for (unsigned i = 0; i < num_o; i++)
	{
		for (unsigned j = i + 1; j < num_o; j++)
		{
			std::pair<std::string, std::string> key;
			key = std::make_pair(index2ObjName[i], index2ObjName[j]);

			double val = sim_fallback;
			if(!use_custom_fallback)
			{
				val = minValue;
			}

			if(sim_value.find(key) != sim_value.end())
			{
				double o1_o2 = sim_value[key];

				if(hasPartner[key])
				{
					std::pair<std::string, std::string> rkey;
					rkey = std::make_pair(index2ObjName[j], index2ObjName[i]);

					double o2_o1 = sim_value[rkey];
					if(std::min(o1_o2,o2_o1) == std::numeric_limits<double>::lowest()){
						val = std::max(o1_o2,o2_o1);
					}else{
						val = std::min(o1_o2,o2_o1);
					}
				}else{
					val = o1_o2; 
				}
			}

			if(val == std::numeric_limits<double>::max()){
				#pragma omp critical 
				{
					positive_inf.push_back(std::make_pair(i,j));
				}
			}else if(maxValue < val){
				#pragma omp critical 
				{
					maxValue = val;
				}
			}
			matrix.at(index(i, j)) = val;
		}
	}
	for(std::pair<unsigned,unsigned> &p:positive_inf){
		matrix.at(index(p.first,p.second)) = maxValue;
	}

}

void TriangularMatrix::readFile(
	const std::string &filename,
	std::map<std::string, unsigned> &object2index,
	std::map<std::pair<std::string, std::string>, double> & sim_value,
	std::map<std::pair<std::string, std::string>, bool> &hasPartner
	)
{
	maxValue = std::numeric_limits<double>::lowest();
	minValue = std::numeric_limits<double>::max();
	/*********************************
	 * Read the input similarity file
	 ********************************/
	std::ifstream fs(filename);
	for (std::string line; std::getline(fs, line); )
	{
		
		// split line from similarity file <o1,o2,sim val>
		std::istringstream buf(line);
		std::istream_iterator<std::string> beg(buf), end;
		std::vector<std::string> tokens(beg, end);

		// object 1
		std::string o1 = tokens.at(0);
		// object 2
		std::string o2 = tokens.at(1);

		// o1,o2 similarity
		//double value = std::atof(tokens.at(2).c_str());
		double value = std::stod(tokens.at(2));

		// if inf/-inf 
		if(std::isinf(value))
		{
			if(value < 0 ){
				value = std::numeric_limits<double>::lowest(); 
			}else{
				value = std::numeric_limits<double>::max(); 
			}
		}

		// build objects maps
		if (object2index.find(o1) == object2index.end())
		{
			index2ObjName.push_back(o1);
			unsigned _id = index2ObjName.size()-1;
			object2index[o1] = _id;
			index2ObjId.push_back(_id);
		}

		if (object2index.find(o2) == object2index.end())
		{
			index2ObjName.push_back(o2);
			unsigned _id = index2ObjName.size()-1;
			object2index[o2] = _id;
			index2ObjId.push_back(_id);
		}

		// create key for similarity map (smallest id first)
		std::pair<std::string, std::string> key;
		if (object2index[o1] < object2index[o2])
		{
			key = std::make_pair(o1, o2);
		}
		else
		{
			key = std::make_pair(o2, o1);
		}

		// if a similarity value for the two objects exists
		// choose the smallest
		if (sim_value.find(key) != sim_value.end())
		{
			hasPartner[key] = true;
		}else{
			hasPartner[key] = false;
		}

		if (minValue > value && value != std::numeric_limits<double>::lowest())
		{ 
			minValue = value; 
		}
		sim_value[std::make_pair(o1,o2)] = value;
	}
	unsigned num_o = index2ObjName.size();
	unsigned msize = ((num_o * num_o) - num_o) / 2;
	matrix.resize(msize);
}

TriangularMatrix::TriangularMatrix(
		std::vector<std::vector<double>>& sim_matrix,
		bool use_custom_fallback,
		double sim_fallback)
{
	unsigned num_o = sim_matrix.size();
	unsigned msize = ((num_o * num_o) - num_o) / 2;
	matrix.resize(msize);

	maxValue = std::numeric_limits<double>::lowest();
	minValue = std::numeric_limits<double>::max();

	index2ObjName.resize(num_o);
	index2ObjId.resize(num_o);
	for(unsigned i = 0; i < index2ObjName.size();i++){
		index2ObjName.at(i) = std::to_string(i);
		index2ObjId.at(i) = i;
	}

	for(unsigned i = 0; i < sim_matrix.size(); i++)
	{
		for(unsigned j = i + 1; j < sim_matrix.size(); j++)
		{
			double val = std::min(sim_matrix.at(i).at(j),sim_matrix.at(j).at(i));
			if (maxValue < val){maxValue = val;}
			if (minValue > val){minValue = val;}
			matrix.at(index(i,j)) = val;
		}

	}
	
}
TriangularMatrix::TriangularMatrix(
		const std::string &filename,
		bool use_custom_fallback,
		double sim_fallback,
		FileType ft)
{
	// map <object name> -> <index in matrix>
	std::map<std::string, unsigned> object2index;

	// map for similarity value <<object1 name>-<object2 name>> -> <value>
	std::map<std::pair<std::string, std::string>, double> sim_value;

	// map for findeing one-way sim values
	std::map<std::pair<std::string, std::string>, bool> hasPartner;

	// reads the input file and initializes the 'matrix' array
	readFile(filename,object2index,sim_value,hasPartner);

	if(ft == FileType::LEGACY){
		parseLegacySimDataFile(
				object2index,
				sim_value,
				hasPartner,
				use_custom_fallback,
				sim_fallback);		
	}else if(ft == FileType::SIMPLE){
		parseSimpleSimDataFile(
				object2index,
				sim_value,
				hasPartner,
				use_custom_fallback,
				sim_fallback);		
	}
}
