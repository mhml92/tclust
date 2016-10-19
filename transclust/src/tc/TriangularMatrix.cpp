#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>
#include <limits>
#include <cmath>
#include <utility>
#include <stdlib.h>
#include "transclust/TriangularMatrix.hpp"

/**
 * Contruct an TriangularMatrix based on an input file.
 */
TriangularMatrix::TriangularMatrix(
		const std::string &filename,
		TCC::TransClustParams& _tcp,
		unsigned cc_id)
{
	// unique id (used when makeing filename for external data storage)
	id = cc_id;

	// TransClust parameters
	tcp = _tcp;

	// map <object name> -> <index in matrix>
	std::map<std::string, unsigned> object2index;

	// map for similarity value pair<<object1 name>-<object2 name>> -> <value>
	std::map<std::pair<std::string, std::string>, float> sim_value;

	// map for findeing one-way sim values
	std::map<std::pair<std::string, std::string>, bool> hasPartner;

	// should the data reside in memory or on disk ("external" from memory)
	is_external = tcp.external;

	// is the content of the external file loaded in to memory
	is_loaded = false;

	// reads the input file - fills object2index,sim_value,hasPartner
	readFile(filename,object2index,sim_value,hasPartner);

	// number of nodes (objects)
	num_o = index2ObjName.size();

	// size of the cost matrix (number of edges in the fulle connected graph)
	msize = ((num_o * num_o) - num_o) / 2;

	// set file type enum
	if(tcp.file_type == "LEGACY"){
		ft = FileType::LEGACY;
	}else if(tcp.file_type == "SIMPLE"){
		ft = FileType::SIMPLE;
	}

	// write cost matrix
	if(tcp.external){
		writeToFile(
			object2index,
			sim_value,
			hasPartner);	
	}else{
		writeToMemory(
			object2index,
			sim_value,
			hasPartner);	
	}
}

/**
 * Contrust an TriangularMatrix based on another TriangularMatrix and a 
 * membership vector
 */
TriangularMatrix::TriangularMatrix(
		TriangularMatrix &m,
		const std::vector<unsigned> &objects,
		unsigned cc_id)
{
	tcp = m.getTcp();
	id = cc_id;
	maxValue = std::numeric_limits<float>::lowest();
	minValue = std::numeric_limits<float>::max();

	num_o = objects.size();
	msize = ((num_o * num_o) - num_o) / 2;
	if(tcp.external && msize > 0){

		bin_file_path = getFilePath().string();

		boost::iostreams::mapped_file_params mfp;
		mfp.path = bin_file_path;
		mfp.new_file_size = msize*sizeof(float);

		boost::iostreams::mapped_file_sink mfs;

		mfs.open(mfp);
			
		if(mfs.is_open()){

			float* mmf = (float*)mfs.data();

			for (unsigned i = 0; i < num_o; i++)
			{
				// indexes
				index2ObjName.push_back(m.getObjectName(objects.at(i)));
				index2ObjId.push_back(m.getObjectId(objects.at(i)));

				for (unsigned j = i + 1; j < num_o; j++)
				{
					float val = m.get(objects.at(i), objects.at(j));
					if (maxValue < val){maxValue = val;}
					if (minValue > val){minValue = val;}

					// read the similarity values and write the values (column-wise lower 
					// right half of matrix) to the binary 1d matrix.
					//
					//		a
					//		bc    -> 1d column-wise lower right half
					//		def   ->	
					//		ghij  -> [a,b,d,g,k,c,e,h,l,f,i,m,j,n,o]    
					//		klmno
					//
					//		index 1d matrix at i,j => (#rows * i - (i+1)*(i)/2 + j-(i+1))
					//

					mmf[index(i,j)] = val;
				}
			}
			mfs.close();
		}else{
			std::cout << "ERROR OPENING EXTERNAL FILE" << std::endl;
			exit(1);
		}
	}else{

		matrix.resize(msize);
		for (unsigned i = 0; i < num_o; i++)
		{
			// indexes
			index2ObjName.push_back(m.getObjectName(objects.at(i)));
			index2ObjId.push_back(m.getObjectId(objects.at(i)));
			for (unsigned j = i + 1; j < num_o; j++)
			{
				float val = m.get(objects.at(i), objects.at(j));
				if (maxValue < val){maxValue = val;}
				if (minValue > val){minValue = val;}

				matrix.at(index(i, j)) = val;
			}
		}
	}
}

/**
 * Contrust an TriangularMatrix based on a vector and the number of objects
 */
TriangularMatrix::TriangularMatrix(
		std::vector<float>& sim_matrix_1d,
		unsigned _num_o,
		unsigned cc_id
   )
{
	id = cc_id;
   num_o = _num_o;
	matrix = sim_matrix_1d;
	msize = matrix.size();

	maxValue = std::numeric_limits<float>::lowest();
	minValue = std::numeric_limits<float>::max();

	index2ObjName.resize(num_o);
	index2ObjId.resize(num_o);
	for(unsigned i = 0; i < index2ObjName.size();i++){
		index2ObjName.at(i) = std::to_string(i);
		index2ObjId.at(i) = i;
	}
	for(unsigned i = 0; i < matrix.size(); i++){
			float val = matrix.at(i);
			if (maxValue < val){maxValue = val;}
			if (minValue > val){minValue = val;}
	}
}

void TriangularMatrix::writeToMemory(
		std::map<std::string, unsigned>& object2index,
		std::map<std::pair<std::string, std::string>, float> & sim_value,
		std::map<std::pair<std::string, std::string>, bool> &hasPartner
		)
{
	float val;
	std::vector<std::pair<unsigned,unsigned>> positive_inf;
	matrix.resize(msize);
	for (unsigned i = 0; i < num_o; i++)
	{
		for (unsigned j = i + 1; j < num_o; j++)
		{
			if(ft == FileType::SIMPLE){
				val = parseSimpleEdge(
						positive_inf,
						object2index,
						sim_value,
						hasPartner,
						i,
						j);
				
			}else if(ft == FileType::LEGACY){
				val = parseLegacyEdge(
						object2index,
						sim_value,
						hasPartner,
						i,
						j);
				
			}
			matrix.at(index(i,j)) = val;
		}
	}

	if(ft == FileType::SIMPLE){
		for(std::pair<unsigned,unsigned> &p:positive_inf){
			matrix.at(index(p.first,p.second)) = maxValue;
		}
	}
}

void TriangularMatrix::writeToFile(
		std::map<std::string, unsigned>& object2index,
		std::map<std::pair<std::string, std::string>, float> & sim_value,
		std::map<std::pair<std::string, std::string>, bool> &hasPartner
		)
{
	float val;
	std::vector<std::pair<unsigned,unsigned>> positive_inf;
	// initialize (os agnostic) path to the temporary directory of the 
	// external memory file

	// set class variable with path to file
	bin_file_path = getFilePath().string();

	// read the similarity values and write the values (column-wise lower 
	// right half of matrix) to the binary 1d matrix.
	//
	//		a
	//		bc    -> 1d column-wise lower right half
	//		def   ->	
	//		ghij  -> [a,b,d,g,k,c,e,h,l,f,i,m,j,n,o]    
	//		klmno
	//
	//		index 1d matrix at i,j => (#rows * i - (i+1)*(i)/2 + j-(i+1))
	//

	boost::iostreams::mapped_file_params mfp;
	mfp.path = bin_file_path;
	mfp.new_file_size = msize*sizeof(float);

	boost::iostreams::mapped_file_sink mfs;

	mfs.open(mfp);

	if(mfs.is_open()){

		float* mmf = (float*)mfs.data();

		for (unsigned i = 0; i < num_o; i++)
		{
			for (unsigned j = i + 1; j < num_o; j++)
			{
				if(ft == FileType::SIMPLE){
					val = parseSimpleEdge(
							positive_inf,
							object2index,
							sim_value,
							hasPartner,
							i,
							j);

				}else if(ft == FileType::LEGACY){
					val = parseLegacyEdge(
							object2index,
							sim_value,
							hasPartner,
							i,
							j);

				}
				mmf[index(i,j)] = val;
			}
		}

		if(ft == FileType::SIMPLE){
			for(std::pair<unsigned,unsigned> &p:positive_inf){
				mmf[index(p.first,p.second)] = maxValue;
			}
		}

	}else{
		std::cout << "ERROR OPENING EXTERNAL FILE" << std::endl;
		exit(1);
	}
	mfs.close();
}

float TriangularMatrix::parseLegacyEdge(
		std::map<std::string, unsigned>& object2index,
		std::map<std::pair<std::string, std::string>, float> & sim_value,
		std::map<std::pair<std::string, std::string>, bool> &hasPartner,
		unsigned i,
		unsigned j)
{
	// key for object pair in sim_value map
	std::pair<std::string, std::string> key;
	key = std::make_pair(index2ObjName[i], index2ObjName[j]);

	// default value if pair is missing (minimum observed value)
	float val = minValue;

	// if some other value should be used
	if(tcp.use_custom_fallback)
	{
		val = tcp.sim_fallback;
	}

	// if the pair is in sim_value and it has a partner
	// 	choose the smallest sim value
	if(sim_value.find(key) != sim_value.end())
	{
		if(hasPartner.find(key) != hasPartner.end())
		{
			std::pair<std::string, std::string> rkey;
			rkey = std::make_pair(index2ObjName[j], index2ObjName[i]);
			val = std::min(sim_value[key],sim_value[rkey]);
		}
	}

	// update maxValue 
	if(maxValue < val){maxValue = val;};
	return val;
}

float TriangularMatrix::parseSimpleEdge(
		std::vector<std::pair<unsigned,unsigned>>& positive_inf,
		std::map<std::string, unsigned>& object2index,
		std::map<std::pair<std::string, std::string>, float> & sim_value,
		std::map<std::pair<std::string, std::string>, bool> &hasPartner,
		unsigned i,
		unsigned j)
{
	std::pair<std::string, std::string> key;
	key = std::make_pair(index2ObjName[i], index2ObjName[j]);

	float val = 0;

	if(sim_value.find(key) != sim_value.end())
	{
		float o1_o2 = sim_value[key];

		if(hasPartner[key])
		{
			std::pair<std::string, std::string> rkey;
			rkey = std::make_pair(index2ObjName[j], index2ObjName[i]);

			float o2_o1 = sim_value[rkey];
			if(std::min(o1_o2,o2_o1) == std::numeric_limits<float>::lowest()){
				val = std::max(o1_o2,o2_o1);
			}else{
				val = std::min(o1_o2,o2_o1);
			}
		}else{
			val = o1_o2;
		}
	}else{
		val = tcp.sim_fallback;
	}

	if(val == std::numeric_limits<float>::max()){
		positive_inf.push_back(std::make_pair(i,j));
	}else if(maxValue < val){
		maxValue = val;
	}
	return val;

}

void TriangularMatrix::readFile(
	const std::string &filename,
	std::map<std::string, unsigned> &object2index,
	std::map<std::pair<std::string, std::string>, float> & sim_value,
	std::map<std::pair<std::string, std::string>, bool> &hasPartner
	)
{
	// init max and min values
	maxValue = std::numeric_limits<float>::lowest();
	minValue = std::numeric_limits<float>::max();

	// read file line by line
	std::ifstream fs(filename);
	for (std::string line; std::getline(fs, line); )
	{
		// split line: o1, o2, val
		std::istringstream buf(line);
		std::istream_iterator<std::string> beg(buf), end;
		std::vector<std::string> tokens(beg, end);

		// object 1
		std::string o1 = tokens.at(0);
		// object 2
		std::string o2 = tokens.at(1);

		// similarity
		//float value = std::stod(tokens.at(2));
		float value = std::stof(tokens.at(2));

		// if inf/-inf replace with numericlimits 
		if(std::isinf(value))
		{
			if(value < 0 ){
				value = std::numeric_limits<float>::lowest();
			}else{
				value = std::numeric_limits<float>::max();
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

		// create key to check if the pair has a partner
		std::pair<std::string, std::string> key;
		if (object2index[o1] < object2index[o2])
		{
			key = std::make_pair(o1, o2);
		}
		else
		{
			key = std::make_pair(o2, o1);
		}

		if (sim_value.find(key) != sim_value.end())
		{
			hasPartner[key] = true;
		}else{
			hasPartner[key] = false;
		}

		if (minValue > value && value != std::numeric_limits<float>::lowest())
		{
			minValue = value;
		}

		sim_value[std::make_pair(o1,o2)] = value;
	}
	fs.close();
}


