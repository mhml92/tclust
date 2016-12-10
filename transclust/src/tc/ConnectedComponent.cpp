#include <stdio.h>
#include "transclust/ConnectedComponent.hpp"

ConnectedComponent::ConnectedComponent(){}
ConnectedComponent::ConnectedComponent(
		unsigned id,
		long _num_positive_edges,
		TCC::TransClustParams& _tcp)
	:
		cc_id(id),
		tcp(_tcp),
		num_positive_edges(_num_positive_edges)
{
	flat_file_path = getFlatFilePath().string();
}

// TODO might be smart to add a buffer... 
void ConnectedComponent::addCost(uint64_t id, float cost)
{
	if(max_value < cost){max_value = cost;};
	if(min_value > cost){min_value = cost;};
	//max_cost += std::fabs(cost);

	num_cost_values++;

	// convert global id to local id
	// get global ids
	std::pair<unsigned,unsigned> gid = TCC::defuse(id);

	// conver to local
	unsigned li = globalId2localId[gid.first];
	unsigned lj = globalId2localId[gid.second];

	// create 64bit local id
	unsigned long lid = TCC::fuse(li,lj);

	cost_buffer.push_back(Cost(lid,cost));

	if(cost_buffer.size() > 1000)
	{
		flushCostBuffer();
	}
}

void ConnectedComponent::flushCostBuffer(){
	FILE* flatFile = fopen(flat_file_path.c_str(),"a");
	if(flatFile != NULL)
	{
		for(auto& c:cost_buffer){
			fwrite(&c,sizeof(struct Cost),1,flatFile);
		}
		fclose(flatFile);
		cost_buffer.clear();
	}else{
		std::cout 
			<< "ERROR FILE COULD NOT BE OPENED " 
			<< flat_file_path << std::endl;
		exit(1);
	}
}

void ConnectedComponent::load(TCC::TransClustParams& _tcp)
{
	tcp = _tcp;
	// check that the file has not already ben configured and that it is 
	// not transitive
	if(!is_mmf_configured && !transitive)
	{
		////////////////////////////////////////////////////////////////////
		// LAOD
		////////////////////////////////////////////////////////////////////
		
		// Determine if the file fit in memory
		if(tcp.memory_limit == 0)
		{
			// assume it fits in memory
			fit_in_memory = true;
		}else{
			long f_size = 0;
			switch(cff)
			{
				case CostFileFormat::MATRIX:
					// get the size of the file (in bytes)
					f_size = TCC::calc_num_sym_elem(localId2globalId.size()); 
					break;
				case CostFileFormat::FLAT:
					// get the size of the file (in bytes)
					f_size = boost::filesystem::file_size(flat_file_path);
			}
			fit_in_memory = (tcp.memory_limit >= ((double)f_size/1024)/1024);
		}

		switch(cff)
		{
			case CostFileFormat::MATRIX:
				initMatrixFile();
				if(fit_in_memory){
					mm_file.open(matrix_file_path);
				}
				break;
			case CostFileFormat::FLAT:
				initFlatFile();
				if(fit_in_memory){
					mm_file.open(flat_file_path);
				}
				break;
		}
		is_mmf_configured = true;
	}
}

float ConnectedComponent::getCost(unsigned i,unsigned j,bool normalized){

	// if the cost file does not fit in memory then we need to access it as
	// a buffert cost request
	if(!fit_in_memory){
		std::vector<float> buf(1,std::numeric_limits<float>::max());
		getBufferedCost(buf,i,j,normalized);
		return buf[0];
	}

	// initialize cost value variable
	float cost = std::numeric_limits<float>::lowest();

	// depending on how the data is stored:
	switch(cff)
	{
		////////////////////////////////////////////////////////////////////
		// MATRIX FILE FORMAT
		////////////////////////////////////////////////////////////////////
		case CostFileFormat::MATRIX:

			cost = *(((float*)mm_file.data())+index(i,j));
			break;
			////////////////////////////////////////////////////////////////////
			// FLAT FILE FORMAT
			////////////////////////////////////////////////////////////////////
		case CostFileFormat::FLAT:
			unsigned long key = TCC::fuse(i,j);
			if(flat_map.find(key) == flat_map.end())
			{
				cost = tcp.sim_fallback-tcp.threshold;	
			}else{
				cost = flat_map.at(key);
			}
			break;
	}
	if(normalized)
	{
		return cost / normalization_factor ;
		//return (cost - min_value) /( max_value - min_value) ;
	}else{
		return cost;
	}
}


/**
 * Reads the cost values from value at i,j to i,j+buffer.size()
 */
void ConnectedComponent::getBufferedCost(
		std::vector<float>& buffer,
		unsigned i,
		unsigned j,
		bool normalized)
{
	// make sure that the file has bin configured
	if(!is_mmf_configured)
	{
		std::cout << "[ERROR] cost file not loaded - getBufferedCost" << std::endl;
		exit(1);
	}

	if(fit_in_memory)
	{
		// easy case: file fits in memory which means that it is already loaded
		switch(cff)
		{
			case CostFileFormat::MATRIX:
				getInMemoryBufferedCostMatrix(buffer,i,j);
				break;
			///////////////////////////////////////////////////////////////////////
			// FLAT FILE FORMAT
			///////////////////////////////////////////////////////////////////////
			case CostFileFormat::FLAT:
				getInMemoryBufferedCostFlat(buffer,i,j);
				break;
		}
	}else{
		// hard case: file does not fit in memory. We need to load the file at the 
		// desired index
		switch(cff)
		{
			case CostFileFormat::MATRIX:
				getExternalBufferedCostMatrix(buffer,i,j);
				break;
			case CostFileFormat::FLAT:
				getExternalBufferedCostFlat(buffer,i,j);
				break;
		}
	}

	// parse each value and normalize if neccessary
	for(unsigned bi = 0; bi < buffer.size();bi++)
	{
		if(normalized)
		{
			buffer[bi] = buffer[bi] / normalization_factor; 
			//buffer[bi] = (buffer[bi] - min_value) /(max_value-min_value); 
		}
	}
}

void ConnectedComponent::commit()
{
	flushCostBuffer();
	// this is no longer needed as all costs have been added and given local ids
	globalId2localId.clear();

	// The number of edges in the triangular matrix
	long expected_num_edges = TCC::calc_num_sym_elem(localId2globalId.size());

	//// number of negative weight edges (these edges contribute to the
	//// cost of the connected component)
	//num_conflicting_edges = expected_num_edges - num_positive_edges;

	if(expected_num_edges == num_positive_edges){
		// the connected component is already transitive!!! nothing more to 
		// do! :D
		transitive = true;
	}else{
		// the connected component is NOT transitive and must be configured 
		// for computations with FORCE and FPT
		//
		// if some values are missing we need to check if the fallback value
		// (minus the threshold) is the larges or smallest cost value in the
		// connected component
		if(num_cost_values < expected_num_edges){
			float fallback = tcp.sim_fallback-tcp.threshold;
			if(max_value < fallback){max_value = fallback;}
			if(min_value > fallback){min_value = fallback;}
		}
		// determining the most suitable way to store the cost data 
		unsigned cm_size_b = expected_num_edges*sizeof(float);
		unsigned cm_size_mb = cm_size_b/(1024*1024);

		bool cost_matrix_fit_in_memory = (cm_size_mb <= tcp.memory_limit);
		bool cost_matrix_has_smallest_size = (
				cm_size_b <= boost::filesystem::file_size(flat_file_path)
				);

		// decide which format should be used
		if(cost_matrix_fit_in_memory || cost_matrix_has_smallest_size)
		{
			cff = CostFileFormat::MATRIX;
		}else{
			cff = CostFileFormat::FLAT;
		}
		// set the normmalization variable
		normalization_factor = std::max( 
				std::fabs(min_value),
				std::fabs(max_value) 
				);
	}
}

void ConnectedComponent::initFlatFile()
{
	if(fit_in_memory)
	{
		FILE* flatFile = fopen(flat_file_path.c_str(),"rb"); 

		if(flatFile != NULL)
		{
			// read the flat file - one cost struct at a time
			struct Cost _cost;
			while ( fread(&_cost,sizeof(struct Cost), 1, flatFile) == 1 )
			{
				flat_map.insert(std::make_pair(_cost.id,_cost.cost));
			}
			fclose(flatFile);

		}else{
			std::cout 
				<< "ERROR FILE COULD NOT BE OPENED " 
				<< flat_file_path << std::endl;
			exit(1);
		}
	
	}else{
		// as the flat file is to large to store in memory we need to create an
		// index to effeciently find the cost values. 
		
		unsigned flat_map_index_max_size = (tcp.memory_limit*(1024*1024))/sizeof(Cost);	

		FILE* flatFile = fopen(flat_file_path.c_str(),"rb"); 

		// the expected number of elements between each file_map_index element
		unsigned elem_step = num_cost_values/flat_map_index_max_size;

		// number of observed elements
		unsigned elem_count = 0;

		if(flatFile != NULL)
		{
			// read the flat file - one cost struct at a time
			struct Cost _cost;
			while ( fread(&_cost,sizeof(struct Cost), 1, flatFile) == 1 )
			{
				bool is_first_elem = (elem_count == 0);
				bool is_last_elem = (elem_count == num_cost_values-1);
				bool is_step_elem = (elem_count % elem_step == 0);

				if(is_first_elem || is_last_elem || is_step_elem)
				{
					flat_map_index.insert(std::make_pair(_cost.id,elem_count));	
				}
				elem_count++;
			}
			fclose(flatFile);

		}else{
			std::cout 
				<< "ERROR FILE COULD NOT BE OPENED " 
				<< flat_file_path << std::endl;
			exit(1);
		}
	}
}

void ConnectedComponent::initMatrixFile()
{
	if(!is_external_file_configured)
	{
		// get a filepath for the external file
		matrix_file_path = getMatrixFilePath().string();

		// read the flat file and build MATRIX file
		FILE* tmpFile = fopen(flat_file_path.c_str(),"rb"); 

		// matrix file pointer
		FILE* matFile = fopen(matrix_file_path.c_str(),"wb");

		unsigned  current_index = 0;
		float fallback = tcp.sim_fallback-tcp.threshold;

		if(tmpFile != NULL)
		{
			// read the flat file - one cost struct at a time
			struct Cost _cost;
			while ( fread(&_cost,sizeof(struct Cost), 1, tmpFile) == 1 )
			{
				// parse the struct
				std::pair<unsigned,unsigned> ids = TCC::defuse(_cost.id);
				float val = _cost.cost;

				// get the indexes of the nodes for which the cost belongs <- shakespear level commenting... sorry
				unsigned i,j;
				i = ids.first;
				j = ids.second;

				// taking in to acount that some values might be mmissing we have to 
				// keep track of which index we are writing and fill the missing 
				// indexes with the fallback value ( minus the threshold) 
				if(matFile != NULL)
				{
					unsigned new_index = index(i,j);
					while(current_index <= new_index)
					{
						if(current_index == new_index)
						{
							fwrite(&val,sizeof(float),1,matFile);
						}else{
							fwrite(&fallback,sizeof(float),1,matFile);
						}
						current_index++;
					}
				}else{
					std::cout 
						<< "ERROR FILE COULD NOT BE OPENED MATRIX FILE" 
						<< matrix_file_path << std::endl;
					exit(1);
				}
			}
			fclose(matFile);
			fclose(tmpFile);
			// this file is no longer needed so we can get rid of it
		}else{
			std::cout 
				<< "ERROR FILE COULD NOT BE OPENED FLAT FILE" 
				<< flat_file_path  << std::endl;
			exit(1);
		}
		boost::filesystem::remove(flat_file_path);
		is_external_file_configured = true;
	}else{
		LOGI << "external file is configured?";
	}


}

unsigned long ConnectedComponent::index(unsigned i,unsigned j) 
{
	if(j < i){
		std::swap(j,i);
	}else if(i == j){
		std::cout << "Error: attempt tojindex diagonal in TriangularMatrix" << std::endl;
		exit(0);
	}
	///* row-major index */
	//return (((i*(i-1))/2)+j);

	/* column-mojor index */
	return (size()*i - ((i+1)*i)/2 + j-(i+1));
}

boost::filesystem::path ConnectedComponent::getFlatFilePath()
{
	boost::filesystem::path dir(tcp.tmp_dir);
	if(!boost::filesystem::is_directory(dir))
	{
		boost::filesystem::create_directory(dir);
	}

	boost::filesystem::path _file (
			"flat_id_" + 
			std::to_string(cc_id) + 
			".bcm"
			);
	return dir / _file;
}

boost::filesystem::path ConnectedComponent::getMatrixFilePath()
{
	boost::filesystem::path dir(tcp.tmp_dir);
	if(!boost::filesystem::is_directory(dir))
	{
		boost::filesystem::create_directory(dir);
	}
	boost::filesystem::path _file (
			"matrix_id_" + 
			std::to_string(cc_id) + 
			"_objects_" + 
			std::to_string(localId2globalId.size()) + 
			".bcm"
			);
	return dir / _file;
}

void ConnectedComponent::getExternalBufferedCostMatrix(
		std::vector<float>& buffer,
		unsigned i,
		unsigned j)
{
	// Since the file does not fit in memory, we need to calculate the offset
	// from which the file should be mapped and then only map from this offset 
	// to the size of the buffer. 
	// BUT we can not offset a file from any position.Any offset must be a 
	// multiple of the operating system's virtual memory allocation 
	// granularity.

	// get the Memory Allocation Granularity
	unsigned mag = mm_file.alignment();

	// get the Start Index (index by byte) of the values 
	unsigned si = index(i,j)*sizeof(float);

	// calculate the Offset Scalar with respect to the memory allocation 
	// granularity
	unsigned os = si/mag;

	// calculate File Offset
	unsigned fo = os*mag;

	// calculate the Index Offset (in float values - 4 bytes) from the file 
	// offset to the start index
	unsigned io = (si - fo)/4;

	// calculate number of bytes to map from the file offset (in bytes)
	unsigned nb = ((buffer.size()+io)*sizeof(float));

	// open the file from the offset
	mm_file.open(matrix_file_path,nb,fo);

	float* data_p = (float*)mm_file.data();

	// read the data in to the buffer and close
	for(unsigned i = 0; i  < buffer.size();i++)
	{
		buffer[i] = *(data_p + io+i);
	}
	mm_file.close();

}

void ConnectedComponent::getExternalBufferedCostFlat(
		std::vector<float>& buffer,
		unsigned i,
		unsigned j)
{
	// We need to load all values (possibly pasdded with falback values)
	// from the flat file in the range from (i,j) to 
	// (i,j+(buffer.size()-1))

	// first value
	unsigned long lokey = TCC::fuse(i,j);

	// last value
	unsigned long upkey = TCC::fuse(i,j+(buffer.size()-1));

	// lower bound of the indexes
	std::map<unsigned long,unsigned>::iterator itlow,itup;

	// get the first value which is NOT smaller then lokey - equale 
	// or larger
	itlow = flat_map_index.lower_bound(lokey);

	// the same for the upper bound
	itup = flat_map_index.lower_bound(upkey);


	// if the lokey id has a strictly higher id that that of the upper 
	// bound value, we know that none of the values in the range are in the
	// file, and we can fill the buffer with fallback values
	if(lokey > itup->first)
	{
		for(unsigned bi = 0; bi  < buffer.size();bi++)
		{
			buffer[bi] = tcp.sim_fallback-tcp.threshold;
		}
		return;
	}

	// get the Memory Allocation Granularity
	unsigned mag = mm_file.alignment();

	// get the Start Index (index by byte) of the values 
	unsigned si = itlow->second*sizeof(Cost);

	// calculate the Offset Scalar with respect to the memory allocation 
	// granularity
	unsigned os = si/mag;

	// calculate File Offset
	unsigned fo = os*mag;

	//// calculate the Index Offset (in Cost structs - sizeof(Cost)) from the 
	//// file offset to the start index
	unsigned io = (si - fo)/sizeof(Cost);
	//
	// length of the range we need to load in to memory
	unsigned num_elems_to_load = (itup->second - itlow->second)+1;

	// calculate number of bytes to map from the file offset (in bytes)
	unsigned nb = (num_elems_to_load+io)*sizeof(Cost);

	// open the file from the offset
	mm_file.open(flat_file_path,nb,fo);

	// read the data in to the buffer and close
	for(unsigned bi = 0; bi  < buffer.size();bi++)
	{
		unsigned long expected_index = TCC::fuse(i,j+bi);

		Cost _cost = *(((Cost*)mm_file.data())+io+bi);
		while(expected_index < _cost.id && bi < buffer.size())
		{
			buffer[bi] = tcp.sim_fallback-tcp.threshold;
			bi++;
			expected_index = TCC::fuse(i,j+bi);
		}

		if(bi < buffer.size())
		{
			buffer[bi] = _cost.cost;
		}
	}
	mm_file.close();
}

void ConnectedComponent::getInMemoryBufferedCostMatrix(
		std::vector<float>& buffer,
		unsigned i,
		unsigned j)
{
	// file index
	unsigned fi = index(i,j);

	float* data_p = (float*)mm_file.data();
	// read sequential memory from the file and add it to the buffer
	// TODO a more effecient way to read the sequence meight exist
	for(unsigned bi = 0; bi < buffer.size();bi++)
	{
		buffer[bi] = *(data_p + fi+bi);
	}

}

void ConnectedComponent::getInMemoryBufferedCostFlat(
		std::vector<float>& buffer,
		unsigned i,
		unsigned j)
{
	for(unsigned bi = 0; bi < buffer.size(); bi++)
	{
		long key = TCC::fuse(i,j+bi);
		if(flat_map.find(key) == flat_map.end())
		{
			buffer[bi] = tcp.sim_fallback-tcp.threshold;	
		}else{
			buffer[bi] = flat_map.at(key);
		}
	}
}
