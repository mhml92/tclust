//#include <iostream>
//#include <fstream>
//#include <plog/Log.h>
//#include "transclust/SimilarityData.hpp"
//
//
//SimilarityData::SimilarityData(
//		const std::string& _filename,
//		TCC::TransClustParams& _tcp
//		)
//{
//	tcp = _tcp;
//	filename = _filename;
//}
//
//void SimilarityData::init(std::deque<std::string>& id2name)
//{
//	if(tcp.external){
//		//init_external();
//	}else{
//		init_internal(id2name);
//	}
//}
//
//void SimilarityData::init_internal(std::deque<std::string>& id2name)
//{
//	LOGI << "Reading input...";
//	// init max and min values
//	maxValue = std::numeric_limits<float>::lowest();
//	minValue = std::numeric_limits<float>::max();
//
//	// read file line by line
//	std::ifstream fs(filename);
//	for (std::string line; std::getline(fs, line); )
//	{
//		// split line: o1, o2, val
//		std::istringstream buf(line);
//		std::istream_iterator<std::string> beg(buf), end;
//		std::vector<std::string> tokens(beg, end);
//
//		// object 1
//		std::string o1 = tokens.at(0);
//		// object 2
//		std::string o2 = tokens.at(1);
//
//		// we dont care about selfsimilarities
//		if(o1 == o2){
//			continue;
//		}
//
//		// similarity
//		//float value = std::stod(tokens.at(2));
//		float value = std::stof(tokens.at(2));
//
//		// if inf/-inf replace with numericlimits 
//		if(std::isinf(value))
//		{
//			LOGW_IF(!has_inf_in_input) << "Input contains 'Inf' or '-Inf' values. These will be truncated to max/min observed value in input."; 
//			has_inf_in_input = true;
//			if(value < 0 ){
//				value = std::numeric_limits<float>::lowest();
//			}else{
//				value = std::numeric_limits<float>::max();
//			}
//		}
//
//		buildObjectIdMaps(o1,id2name);
//		buildObjectIdMaps(o2,id2name);
//
//		// create key
//		std::pair<std::string, std::string> key;
//		if (name2id[o1] < name2id[o2])
//		{
//			key = std::make_pair(o1, o2);
//		}else{
//			key = std::make_pair(o2, o1);
//		}
//
//
//		if(values.find(key) != values.end()){
//			// if this pair has been seen before only update if the value is 
//			// smaller
//			if(values.at(key) > value){
//				values[key] = value;
//			}
//		}else{
//			values[key] = value;
//		}
//
//		// update min observed value
//		if (minValue > value && value != std::numeric_limits<float>::lowest())
//		{
//			minValue = value;
//		}
//	}
//	fs.close();
//	LOGI << "done";
//	LOGI << "minValue: " << minValue;
//
//	long num_edges = values.size();
//	long expected_num_edges = ((((long)id2name.size())*id2name.size()) - id2name.size() )/2; 
//	long diff_edges = expected_num_edges - num_edges;
//	if(diff_edges > 0){
//		LOGW << "Expected " << expected_num_edges << " but only " << num_edges << " viable edges found in input. Missing edges are given the default similarity value of " << tcp.sim_fallback;
//	}
//
//}
//
//bool SimilarityData::hasValue(std::pair<std::string,std::string>& key){
//	return (values.find(key) != values.end());
//}
//
//float SimilarityData::getValue(std::pair<std::string,std::string>& key){
//	return values[key];
//}
//
