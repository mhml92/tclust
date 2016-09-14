#include "TriangularMatrix.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>
#include <limits>
#include <utility>
#include <stdlib.h>

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

TriangularMatrix::TriangularMatrix(
		const std::string &filename,
		bool use_custom_fallback,
		double sim_fallback)
{
	// map <object name> -> <index in matrix>
	std::map<std::string, unsigned> object2index;

	// map for similarity value <<object1 name>-<object2 name>> -> <value>
	std::map<std::pair<std::string, std::string>, double> sim_value;
	// map for findeing one-way sim values
	std::map<std::pair<std::string, std::string>, bool> hasPartner;

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
			if (sim_value[key] < value)
			{
				value = sim_value[key];
			}
		}else{
			hasPartner[key] = false;
		}

		if (minValue > value){ minValue = value; }
		sim_value[key] = value;

	}
	unsigned num_o = index2ObjName.size();
	unsigned msize = ((num_o * num_o) - num_o) / 2;
	matrix.resize(msize);

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
				if(hasPartner[key])
				{
					val = sim_value[key];
				}else{
					//val = sim_value[key];
					val = sim_fallback;
				}
			}else{
				val = sim_fallback;
			}

			if (maxValue < val){ maxValue = val; }
			matrix.at(index(i, j)) = val;
		}
	}
}
