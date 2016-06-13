#include <TriangularMatrix.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>
#include <limits>
#include <utility>

TriangularMatrix::TriangularMatrix(
		const TriangularMatrix &m,
		const std::vector<unsigned> &objects)
{
	maxValue = std::numeric_limits<float>::min();
	minValue = std::numeric_limits<float>::max();

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

			float val = m(objects.at(i), objects.at(j));
			if (maxValue < val)
			{
				maxValue = val;
			}
			if (minValue > val)
			{
				minValue = val;
			}
			matrix.at(index(i, j)) = val;
		}
	}
}

TriangularMatrix::TriangularMatrix(const std::string &filename)
{
	// map <object name> -> <index in matrix>
	std::map<std::string, unsigned> object2index;

	// map for similarity value <<object1 name>-<object2 name>> -> <value>
	std::map<std::pair<std::string, std::string>, float> sim_value;

	maxValue = std::numeric_limits<float>::min();
	minValue = std::numeric_limits<float>::max();

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
		// object 3
		std::string o2 = tokens.at(1);

		// o1,o2 similarity
		float value = stof(tokens.at(2));

		// update max and min value
		if (minValue > value)
		{
			minValue = value;
		}
		if (maxValue < value)
		{
			maxValue = value;
		}

		// build objects maps
		if (object2index.find(o1) == object2index.end())
		{
			index2ObjName.push_back(o1);
			unsigned _id = object2index.size();
			object2index[o1] = _id;
			index2ObjId.push_back(_id);
		}

		if (object2index.find(o2) == object2index.end())
		{
			index2ObjName.push_back(o2);
			unsigned _id = object2index.size();
			object2index[o2] = _id;
			index2ObjId.push_back(_id);
		}

		// create key for similarity map (smalles id first)
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
			if (sim_value[key] < value)
			{
				value = sim_value[key];
			}
		}
		sim_value[key] = value;

	}
	unsigned num_o = index2ObjName.size();
	unsigned msize = ((num_o * num_o) - num_o) / 2;
	matrix.resize(msize);

	for (unsigned i = 0; i < num_o; i++)
	{
		for (unsigned j = i + 1; j < num_o; j++)
		{
			float val = sim_value[std::make_pair(index2ObjName[i], index2ObjName[j])];

			if (maxValue < val)
			{
				maxValue = val;
			}
			if (minValue > val)
			{
				minValue = val;
			}
			matrix.at(index(i, j)) = val;
		}
	}
}


/*
// find the object pair (might be o1o2 or o2o1)
if(sim_value.find(index2ObjName[i]+'-'+index2ObjName[j]) != sim_value.end())
{
val = sim_value[index2ObjName[i]+'-'+index2ObjName[j]];
}
else if(sim_value.find(index2ObjName[j]+'-'+index2ObjName[i]) != sim_value.end())
{
val = sim_value[index2ObjName[j]+'-'+index2ObjName[i]];
}
else
{
//if it is not present then it is 0

// TO DO: this must be configurable
val = 0;
}
*/

/*
	if(object2index.empty()){
	index2ObjName.push_back(tokens.at(0));
	object2index[tokens.at(0)] = 0;
	index2ObjId.push_back(0);
	}else{
	if(index2ObjName.back() != tokens.at(0)){
	index2ObjId.push_back(index2ObjName.size());
	object2index[tokens.at(0)] = index2ObjName.size();
	index2ObjName.push_back(tokens.at(0));
	}
	if(object2index.find(tokens.at(1)) == object2index.end()){
	std::cout << "need to fix: read sim file" << std::endl;
	}
	}
// if o1,o2 || o2,o1 exists in sim_value
float current_sval = std::numeric_limits<float>::min();
float sval = atof(tokens.at(2).c_str());

// hack
bool zeroOne = true;
if(sim_value.find(tokens.at(0)+'-'+tokens.at(1))!= sim_value.end()){
current_sval = sim_value[tokens.at(0)+'-'+tokens.at(1)];
}else if(sim_value.find(tokens.at(1)+'-'+tokens.at(0)) != sim_value.end()){
current_sval = sim_value[tokens.at(1)+'-'+tokens.at(0)];
zeroOne = false;
}

*/
/* non symetric similarity warning
	if(current_sval != std::numeric_limits<float>::min() && current_sval != sval){
// warning if non symetrical
std::cout << "Warning:\tNon symmetric similarity values for objects: "
<< tokens.at(0) << ", " << tokens.at(1)
<< ": Choosing smallest similarity" << std::endl;
}
*/
/*

	if(current_sval == std::numeric_limits<float>::min() || current_sval > sval){
	if(zeroOne){
	sim_value[tokens.at(0)+'-'+tokens.at(1)] = sval;
	}else{
	sim_value[tokens.at(1)+'-'+tokens.at(0)] = sval;
	}
	}
	*/
