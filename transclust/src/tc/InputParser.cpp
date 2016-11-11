#ifdef _OPENMP
#	include <omp.h>
#endif
#include <iostream>
#include <fstream>
#include <deque>
#include <stxxl/sorter>
#include <stxxl/stats>
#include <plog/Log.h>
#include "transclust/Common.hpp"
#include <boost/unordered_map.hpp>
#include "transclust/InputParser.hpp"




/**
 * Initializes the InputParser class
 *
 * This class handles reading an input similarity file and initialization of
 * ConnectedCompoents. 
 */
InputParser::InputParser(
		std::string _filename,
		TCC::TransClustParams& _tcp
		)
	:
		filename(_filename),
		tcp(_tcp),
		duf()
{ }


void InputParser::getConnectedComponents(
		std::deque<ConnectedComponent>& ccs,
		RES::Clustering& result
		)
{
	boost::unordered_map<std::string, unsigned> name2id;

	// create sorter object (CompareType(), MainMemoryLimit)
	external_sorter similarity_sorter(similarity_comparator(),1024 * 1024 * 1024);

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	LOGI << "Reading input file";
	/////////////////////////////////////////////////////////////////////////////
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

		// we dont care about selfsimilarities
		if(o1 == o2){ continue;}

		// similarity
		//float value = std::stod(tokens.at(2));
		float value = std::stof(tokens.at(2));

		// if inf/-inf replace with numericlimits 
		if(std::isinf(value))
		{
			LOGW_IF(!has_inf_in_input) << "Input contains 'Inf' or '-Inf' values. These will be truncated to max/min observed value in input."; 
			has_inf_in_input = true;
			if(value < 0 ){
				value = std::numeric_limits<float>::lowest();
			}else{
				value = std::numeric_limits<float>::max();
			}
		}

		buildObjectIdMaps(o1,name2id,result);
		buildObjectIdMaps(o2,name2id,result);

		// create key
		uint64_t key;
		unsigned o1_id = name2id[o1];
		unsigned o2_id = name2id[o2];
		if (o1_id < o2_id)
		{
			key = TCC::fuse(o1_id, o2_id);
		}else{
			key = TCC::fuse(o2_id, o1_id);
		}
		similarity_sorter.push(similarity(key,value));
	}
	fs.close();
	/////////////////////////////////////////////////////////////////////////////
	LOGI << "Reading input file...done";
	/////////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	LOGI << "Sorting input";
	/////////////////////////////////////////////////////////////////////////////
	similarity_sorter.sort();
	/////////////////////////////////////////////////////////////////////////////
	LOGI << "Sorting input...done";
	/////////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	LOGI << "Building DUF";
	/////////////////////////////////////////////////////////////////////////////
	// ensure that we only use the first occurence of each pair 
	uint64_t current_id = 0;
	while (!similarity_sorter.empty())
	{
		// if this is the first occurence of this pair
		if((*similarity_sorter).id != current_id){
			
			// update current_id
			current_id = (*similarity_sorter).id;

			// get the ids of the objects
			std::pair<uint32_t,uint32_t> ids = TCC::defuse((*similarity_sorter).id);

			// get the similarity value
			float value = (*similarity_sorter).sim;

			// if the similarity value is larger then the threshold we know these 
			// two objects belong to the same connected component
			if(tcp.threshold < value){
				duf.funion(ids.first,ids.second);
			}
		}
		++similarity_sorter;
	}
	// make sure all elements are represented
	duf.find(result.id2name.size()-1);
	/////////////////////////////////////////////////////////////////////////////
	LOGI << "Building DUF...done";
	/////////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	LOGI << "Initializing ConnectedComponents";
	/////////////////////////////////////////////////////////////////////////////
	// add all nodes to their respective connected components
	for(unsigned i = 0; i < duf.size(); i++){

		// find the root for element at i
		unsigned rootId = duf.find(i);

		// see if the connected component has not been created 
		if(rootId2cc.find(rootId) == rootId2cc.end())
		{
			// if it has not been created do so and update the rootId2cc
			unsigned cc_id = ccs.size();

			// insert a new connected component with id,number of edges and tcp
			ccs.push_back( ConnectedComponent(cc_id,-duf.at(rootId),tcp) );

			// insert rootId -> cc_id in map
			rootId2cc.insert(std::make_pair(rootId,cc_id));
		}

		// get the index of the connected compoenent in ccs
		unsigned cc_index = rootId2cc[rootId];

		// add the node
		ccs.at(cc_index).addNode(i);
	}
	/////////////////////////////////////////////////////////////////////////////
	LOGI << "Initializing ConnectedComponents...done";
	LOGI << "Found " << ccs.size() << " ConnectedComponents";
	/////////////////////////////////////////////////////////////////////////////
	
	//LOGI << "Found " << ccs.size() << " Connected Component";
	//for(auto& cc:ccs){
	//	LOGI << "cc with id: " << cc.getId() << " has " << cc.size() << " nodes and #edges: " << cc.getNumEdges();
	//}

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	LOGI << "Adding costs to connected components";
	/////////////////////////////////////////////////////////////////////////////
	// reset internal pointer to start of input
	similarity_sorter.rewind();

	// To ensure that we only use the first occurance of an object pair we keep 
	// track of the current id (no object pair has id 0)
	current_id = 0;
	long num_added_edges = 0;
	while (!similarity_sorter.empty())
	{
		// if we see an object pair which we have not seen before
		if((*similarity_sorter).id != current_id){

			num_added_edges++;

			// update current_id 
			current_id = (*similarity_sorter).id;

			// get id of each object
			std::pair<uint32_t,uint32_t> ids = TCC::defuse((*similarity_sorter).id);

			// get the similarity value
			float value = (*similarity_sorter).sim;

			// find the roots of each element
			unsigned root_id_1 = duf.find(ids.first);
			unsigned root_id_2 = duf.find(ids.second);

			// if the objects have the same root (i.e. they belong to the same 
			// connected component) then add cost value
			if(root_id_1 == root_id_2)
			{
				// the cost is (similarity - threshold)
				float cost = TCC::round(value - tcp.threshold);

				// find the index in 'ccs' which holds the relevant connected 
				// component
				unsigned cc_index = rootId2cc[root_id_1];

				// add the cost to the connected component
				ccs.at(cc_index).addCost((*similarity_sorter).id,cost);	
			}
		}
		++similarity_sorter;
	}
	// we now have all the information that we need and the original input can
	// be discarded
	similarity_sorter.finish_clear();
	/////////////////////////////////////////////////////////////////////////////
	LOGI << "Adding costs to connected components...done";

	LOGW_IF(num_added_edges != TCC::calc_num_sym_elem(result.id2name.size())) 
		<< "Similarity file is incomplete. Only " 
		<< num_added_edges 
		<< " out of " 
		<< TCC::calc_num_sym_elem(result.id2name.size()) 
		<< " possible edges for " 
		<< result.id2name.size() 
		<< " nodes were found";
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	LOGI << "Committinig connected components";
	/////////////////////////////////////////////////////////////////////////////
	for(auto& cc:ccs){ cc.commit(); }
	/////////////////////////////////////////////////////////////////////////////
	LOGI << "Committinig connected components...done";
	/////////////////////////////////////////////////////////////////////////////
}
