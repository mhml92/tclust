#include <iterator>
#include <limits>
#include <cmath>
#include <utility>
#include <stdlib.h>
#include <boost/unordered_map.hpp>
#include "transclust/TriangularMatrix.hpp"



TriangularMatrix::TriangularMatrix(
		TCC::TransClustParams& _tcp
		)
{
	tcp = _tcp;
}

void TriangularMatrix::init(
		boost::unordered_map<unsigned,unsigned>& objectId2index,
		std::deque<std::pair<std::pair<unsigned,unsigned>,float>>& cost_values
		)
{
	num_o = objectId2index.size();

	long expected_num_edges = (((long) num_o*num_o)-num_o)/2;

	matrix.resize(expected_num_edges,TCC::round(tcp.sim_fallback-tcp.threshold));
	std::cout << "How to decide storage??? " << __FILE__ << " " << __LINE__ << std::endl;

	std::vector<std::pair<unsigned,unsigned>> positive_inf;
	std::cout << "size of cost value: " << cost_values.size() << std::endl;
	std::cout << "size of CC: " << num_o << std::endl;
	for(auto& it:cost_values)
	{

		// unpack data from cost_value
		std::pair<unsigned,unsigned> objectIds = it.first;
		unsigned o1_id = objectIds.first;
		unsigned o2_id = objectIds.second;
		float value = it.second;

		unsigned i,j;
		i = objectId2index.at(o1_id); 
		j = objectId2index.at(o2_id); 

		std::cout << "i: " << i << ", j: " << j <<", " << cost_values.size() <<std::endl;
		if(value == std::numeric_limits<float>::max()){
			positive_inf.push_back(std::make_pair(i,j));	
		}else{

			if(value < minValue){minValue = value;}
			if(value > maxValue){maxValue = value;}

			matrix.at(index(i,j))= value;
		}
	}

	// set all +Inf values to maxValue
	for(auto& p:positive_inf){
		matrix.at(index(p.first,p.second)) = maxValue;
	}
}

/**
 * Contruct an TriangularMatrix based on an input file.
 */
/*
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
boost::unordered_map<std::string, unsigned> object2index;

// map for similarity value pair<<object1 name>-<object2 name>> -> <value>
boost::unordered_map<std::pair<std::string, std::string>, float> sim_value;

// map for findeing one-way sim values
boost::unordered_map<std::pair<std::string, std::string>, bool> hasPartner;

// should the data reside in memory or on disk ("external" from memory)
//is_external = tcp.external;
if(tcp.external){
sm = StorageMethod::EXTERNAL;
}else{
sm = StorageMethod::INTERNAL;
}

// is the content of the external file loaded in to memory
is_loaded = false;

// reads the input file - fills object2index,sim_value,hasPartner
readFile(filename,object2index,sim_value,hasPartner);

// number of nodes (objects)
num_o = index2ObjName.size();

// size of the cost matrix (number of edges in the fully connected graph)
msize = ((num_o * num_o) - num_o) / 2;

// set file type enum
if(tcp.file_type == "LEGACY"){
ft = FileType::LEGACY;
}else if(tcp.file_type == "SIMPLE"){
ft = FileType::SIMPLE;
}

// write cost matrix
switch(sm){
case StorageMethod::EXTERNAL:
LOGI << "writing to file";
writeToFile(
object2index,
sim_value,
hasPartner);	
break;
case StorageMethod::INTERNAL:
LOGI << "writing to memory";
writeToMemory(
object2index,
sim_value,
hasPartner);	
break;
default:
std::cout << "ERROR no StorageMethod" << std::endl;
exit(0);
}
}
*/
/**
 * Contrust an TriangularMatrix based on another TriangularMatrix and a 
 * membership vector
 */
/*
	TriangularMatrix::TriangularMatrix(
	TriangularMatrix &m,
	const std::vector<unsigned> &objects,
	unsigned cc_id,
	float threshold,
	float delta_threshold)
	{
	tcp = m.getTcp();
	id = cc_id;

	LOGD << id << ": "  << "Creating new TriangularMatrix with id: " << id;

// should the data reside in memory or on disk ("external" from memory)
if(tcp.external){
LOGD << id << ": "  << "Using StorageMethod::EXTERNAL";
sm = StorageMethod::EXTERNAL;
}else{
LOGD << id << ": "  << "Using StorageMethod::INTERNAL";
sm = StorageMethod::INTERNAL;
}

maxValue = std::numeric_limits<float>::lowest();
minValue = std::numeric_limits<float>::max();

num_o = objects.size();

LOGD << id << ": "  << "num_o: " << num_o;

msize = ((num_o * num_o) - num_o) / 2;

LOGD << id << ": "  << "msize: " << msize;

setCostStructure();

switch(sm){
case StorageMethod::EXTERNAL:
///////////////////////////////////////////////////////////////////////
// EXTERNAL
///////////////////////////////////////////////////////////////////////
switch(cs){
case CostStructure::COST_MATRIX:
////////////////////////////////////////////////////////////////////
// EXTERNAL MATRIX FROM TRIANGULAR MATRIX
////////////////////////////////////////////////////////////////////
if(msize > 0){
data_file_path = getFilePath().string();

boost::iostreams::mapped_file_params mfp;
mfp.path = data_file_path;
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
float val = TCC::round(m.get(objects.at(i), objects.at(j))-delta_threshold);
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
	index2ObjName.push_back(m.getObjectName(objects.at(0)));
	index2ObjId.push_back(m.getObjectId(objects.at(0)));
}
break;
case CostStructure::COST_MAP:
/////////////////////////////////////////////////////////////////
// EXTERNAL MAP FROM TRIANGULAR MATRIX
/////////////////////////////////////////////////////////////////
if(msize > 0){
	data_file_path = getFilePath().string();

	std::ofstream ofs;
	ofs.open(data_file_path);

	// TODO changes to sparse map
	if(ofs.is_open()){
		for (unsigned i = 0; i < num_o; i++)
		{
			// indexes
			index2ObjName.push_back(m.getObjectName(objects.at(i)));
			index2ObjId.push_back(m.getObjectId(objects.at(i)));

			for (unsigned j = i + 1; j < num_o; j++)
			{
				float val = m.get(objects.at(i), objects.at(j));

				if(val == std::numeric_limits<float>::lowest())
				{
					float val2 = -threshold;	
					if (maxValue < val2){maxValue = val2;}
					if (minValue > val2){minValue = val2;}
				}else{
					val = TCC::round(val - delta_threshold);
					if (maxValue < val){maxValue = val;}
					if (minValue > val){minValue = val;}
					writePrecision(ofs,i,j,val);
				}
			}
		}
		ofs.close();
	}else{
		std::cout << "ERROR OPENING EXTERNAL FILE" << std::endl;
		exit(1);
	}
}else{
	index2ObjName.push_back(m.getObjectName(objects.at(0)));
	index2ObjId.push_back(m.getObjectId(objects.at(0)));
}
break;
default:
std::cout << "ERROR no CostRespresentation" << std::endl;
exit(0);

}
break;
case StorageMethod::INTERNAL:
///////////////////////////////////////////////////////////////////////
// INTERNAL
///////////////////////////////////////////////////////////////////////
float val;
switch(cs){
	case CostStructure::COST_MATRIX:
		/////////////////////////////////////////////////////////////////
		// INTERNAL MATRIX
		/////////////////////////////////////////////////////////////////
		matrix.resize(msize);
		for (unsigned i = 0; i < num_o; i++)
		{
			// indexes
			index2ObjName.push_back(m.getObjectName(objects.at(i)));
			index2ObjId.push_back(m.getObjectId(objects.at(i)));
			for (unsigned j = i + 1; j < num_o; j++)
			{
				val = m.get(objects.at(i), objects.at(j));

				if(val == std::numeric_limits<float>::lowest())
				{
					val = tcp.sim_fallback-threshold;
				}
				else
				{
					val = TCC::round(val-delta_threshold);
				}

				if (maxValue < val){maxValue = val;}
				if (minValue > val){minValue = val;}

				matrix.at(index(i, j)) = val;
			}
		}
		break;
	case CostStructure::COST_MAP:
		/////////////////////////////////////////////////////////////////
		// INTERNAL MAP
		/////////////////////////////////////////////////////////////////
		cost_map.reserve(msize/4);
		for (unsigned i = 0; i < num_o; i++)
		{
			// indexes
			index2ObjName.push_back(m.getObjectName(objects.at(i)));
			index2ObjId.push_back(m.getObjectId(objects.at(i)));
			for (unsigned j = i + 1; j < num_o; j++)
			{
				val = m.get(objects.at(i), objects.at(j));

				if(val != std::numeric_limits<float>::lowest())
				{
					val =  TCC::round(val-delta_threshold);
					if (maxValue < val){maxValue = val;}
					if (minValue > val){minValue = val;}

					cost_map.insert(std::make_pair(TCC::fuse(i,j),val));
				}else{
					float val2 = -threshold;	
					if (maxValue < val2){maxValue = val2;}
					if (minValue > val2){minValue = val2;}
				}
			}
		}

		break;
	default:
		std::cout << "ERROR no CostRespresentation" << std::endl;
		exit(0);
}
break;
default:
std::cout << "ERROR no StorageMethod" << std::endl;
exit(0);
}
}
*/
/**
 * Contrust an TriangularMatrix based on a vector and the number of objects
 */
/*
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
	boost::unordered_map<std::string, unsigned>& object2index,
	boost::unordered_map<std::pair<std::string, std::string>, float> & sim_value,
	boost::unordered_map<std::pair<std::string, std::string>, bool> &hasPartner
	)
	{
// variable holding the cost of the current cost
float val;

// if the input format is SIMPLE we need to keep track of which values have
// +inf values. After the entire matrix has been written these values must
// be chaged to maxValue
std::vector<std::pair<unsigned,unsigned>> positive_inf;

setCostStructure();

// initialize whichever format we use to store the cost
switch(cs){
case CostStructure::COST_MATRIX:
///////////////////////////////////////////////////////////////////////
// INTERNAL MATRIX
///////////////////////////////////////////////////////////////////////
matrix.resize(msize);
// for each pair of objects
for (unsigned i = 0; i < num_o; i++)
{
for (unsigned j = i + 1; j < num_o; j++)
{
// get the cost based on the file format

switch(ft){
case FileType::SIMPLE:
val = parseSimpleEdge(
positive_inf,
object2index,
sim_value,
hasPartner,
i,
j);
if(val == std::numeric_limits<float>::lowest())
{
val = tcp.sim_fallback;
}
break;
case FileType::LEGACY:
val = parseLegacyEdge(
		object2index,
		sim_value,
		hasPartner,
		i,
		j);
if(val == std::numeric_limits<float>::lowest())
{
	val = minValue;
	// if some other value should be used
	if(tcp.use_custom_fallback)
	{
		val = tcp.sim_fallback;
	}
}
break;
}
// insert
matrix.at(index(i,j)) = val;
}
}
break;
case CostStructure::COST_MAP:
///////////////////////////////////////////////////////////////////////
// INTERNAL MAP
///////////////////////////////////////////////////////////////////////
cost_map.reserve(sim_value.size());
// for each pair of objects
for (unsigned i = 0; i < num_o; i++)
{
	for (unsigned j = i + 1; j < num_o; j++)
	{

		// get the cost based on the file format
		switch(ft){
			case FileType::SIMPLE:
				val = parseSimpleEdge(
						positive_inf,
						object2index,
						sim_value,
						hasPartner,
						i,
						j);
				break;
			case FileType::LEGACY:
				val = parseLegacyEdge(
						object2index,
						sim_value,
						hasPartner,
						i,
						j);
				break;
		}

		// in the map format we dont need to store missing edges explicitly
		if(val != std::numeric_limits<float>::lowest())
		{
			// note that we 'fuse' the two unsinged interger values, 
			// representing the objects, in to a single unsigned long
			cost_map.insert(std::make_pair(TCC::fuse(i,j),val));	
		}
	}
}
break;
default:
std::cout << "ERROR no CostStructure" << std::endl;
exit(0);

}

// if the file format is SIMPLE and there were inf values in the input
if(ft == FileType::SIMPLE)
{
	for(std::pair<unsigned,unsigned> &p:positive_inf)
	{
		if(cs == CostStructure::COST_MATRIX)
		{
			matrix.at(index(p.first,p.second)) = maxValue;
		}
		else
		{
			cost_map[TCC::fuse(p.first,p.second)] = maxValue;	
		}
	}
}
}

void TriangularMatrix::writeToFile(
		boost::unordered_map<std::string, unsigned>& object2index,
		boost::unordered_map<std::pair<std::string, std::string>, float> & sim_value,
		boost::unordered_map<std::pair<std::string, std::string>, bool> &hasPartner
		)
{
	float val;
	std::vector<std::pair<unsigned,unsigned>> positive_inf;

	// set class variable with path to file
	data_file_path = getFilePath().string();
	LOGI << "filename: " << data_file_path;

	setCostStructure();

	switch(cs){
		case CostStructure::COST_MAP:
			///////////////////////////////////////////////////////////////////////
			// EXTERNAL MAP FROM FILE
			///////////////////////////////////////////////////////////////////////
			{
				std::ofstream ofs;
				ofs.open (data_file_path);

				if(ofs.is_open()){
					switch(ft){
						case FileType::SIMPLE:
							float val;
							for (auto it = sim_value.begin(); it != sim_value.end(); ++it )
							{
								std::string o1,o2;
								o1 = it->first.first;
								o2 = it->first.second;

								unsigned i,j;
								i = object2index[o1];
								j = object2index[o2];
								if(j < i){std::swap(i,j);}

								val = parseSimpleEdge(
										positive_inf,
										object2index,
										sim_value,
										hasPartner,
										i,
										j);

								if(val != std::numeric_limits<float>::lowest())
								{
									writePrecision(ofs,i,j,val);
								}
							}
							//////////////////////////////////////////////////////////////
							for(std::pair<unsigned,unsigned> &p:positive_inf){
								writePrecision(ofs,p.first,p.second,maxValue);
							}
							break;
						case FileType::LEGACY:
							for (unsigned i = 0; i < num_o; i++)
							{
								for (unsigned j = i + 1; j < num_o; j++)
								{
									val = parseLegacyEdge(
											object2index,
											sim_value,
											hasPartner,
											i,
											j);

									if(val == std::numeric_limits<float>::lowest())
									{
										val = minValue;

										// if some other value should be used
										if(tcp.use_custom_fallback)
										{
											val = tcp.sim_fallback;
										}
									}
									writePrecision(ofs,i,j,val);
								}
							}
							break;
					}

					//for (unsigned i = 0; i < num_o; i++)
					//{
					//	for (unsigned j = i + 1; j < num_o; j++)
					//	{
					//		switch(ft){
					//			case FileType::SIMPLE:
					//				val = parseSimpleEdge(
					//						positive_inf,
					//						object2index,
					//						sim_value,
					//						hasPartner,
					//						i,
					//						j);
					//				break;
					//			case FileType::LEGACY:
					//				val = parseLegacyEdge(
					//						object2index,
					//						sim_value,
					//						hasPartner,
					//						i,
					//						j);

					//				if(val == std::numeric_limits<float>::lowest())
					//				{
					//					val = minValue;

					//					// if some other value should be used
					//					if(tcp.use_custom_fallback)
					//					{
					//						val = tcp.sim_fallback;
					//					}
					//				}
					//				break;
					//		}
					//		if(val != std::numeric_limits<float>::lowest())
					//		{
					//			writePrecision(ofs,i,j,val);
					//		}
					//	}
					//}

					//if(ft == FileType::SIMPLE){
					//	for(std::pair<unsigned,unsigned> &p:positive_inf){
					//		writePrecision(ofs,p.first,p.second,maxValue);
					//	}
					//}

				}else{
					std::cout << "ERROR opening file for map" << std::endl;
					exit(0);
				}

				ofs.close();
			}
			break;
		case CostStructure::COST_MATRIX:
			///////////////////////////////////////////////////////////////////////
			// EXTERNAL MATRIX
			///////////////////////////////////////////////////////////////////////
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
			mfp.path = data_file_path;
			mfp.new_file_size = msize*sizeof(float);

			boost::iostreams::mapped_file_sink mfs;

			mfs.open(mfp);

			if(mfs.is_open())
			{
				float* mmf = (float*)mfs.data();

				for (unsigned i = 0; i < num_o; i++)
				{
					for (unsigned j = i + 1; j < num_o; j++)
					{
						switch(ft){
							case FileType::SIMPLE:
								val = parseSimpleEdge(
										positive_inf,
										object2index,
										sim_value,
										hasPartner,
										i,
										j);
								if(val == std::numeric_limits<float>::lowest())
								{
									val = tcp.sim_fallback;
								}
								break;
							case FileType::LEGACY:
								val = parseLegacyEdge(
										object2index,
										sim_value,
										hasPartner,
										i,
										j);

								if(val == std::numeric_limits<float>::lowest())
								{
									val = minValue;

									// if some other value should be used
									if(tcp.use_custom_fallback)
									{
										val = tcp.sim_fallback;
									}
								}
								break;
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
			break;
	}
}

float TriangularMatrix::parseLegacyEdge(
		boost::unordered_map<std::string, unsigned>& object2index,
		boost::unordered_map<std::pair<std::string, std::string>, float> & sim_value,
		boost::unordered_map<std::pair<std::string, std::string>, bool> &hasPartner,
		unsigned i,
		unsigned j)
{
	// key for object pair in sim_value map
	std::pair<std::string, std::string> key;
	key = std::make_pair(index2ObjName[i], index2ObjName[j]);

	float val = std::numeric_limits<float>::lowest();

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
		boost::unordered_map<std::string, unsigned>& object2index,
		boost::unordered_map<std::pair<std::string, std::string>, float> & sim_value,
		boost::unordered_map<std::pair<std::string, std::string>, bool> &hasPartner,
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
		val = std::numeric_limits<float>::lowest();
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
		boost::unordered_map<std::string, unsigned> &object2index,
		boost::unordered_map<std::pair<std::string, std::string>, float> & sim_value,
		boost::unordered_map<std::pair<std::string, std::string>, bool> &hasPartner
		)
{
	LOGI << "Reading input...";
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
	LOGI << "done";
	LOGI << "minValue: " << minValue;
}
*/

