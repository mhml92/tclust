#include <stdio.h>
#include "transclust/ConnectedComponent.hpp"

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
	if(num_positive_edges == std::numeric_limits<long>::lowest()){
		num_positive_edges = 0;
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
		std::cout << "[ERROR] cost file not loaded" << std::endl;
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
	//LOGI_IF(globalId2localId.size() > 1000) << "max_value: " << max_value << "\t" << "min_value: " << min_value;
	// this is no longer needed as all costs have been added and given local ids
	globalId2localId.clear();

	// get a filepath for the external file
	matrix_file_path = getFilePath().string();

	// The number of edges in the triangular matrix
	long expected_num_edges = TCC::calc_num_sym_elem(localId2globalId.size());

	// number of negative weight edges (these edges contribute to the
	// cost of the connected component)
	num_conflicting_edges = expected_num_edges - num_positive_edges;

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

		// add the cost of all the explicitly missing edges
		//max_cost += (expected_num_edges-num_cost_values)*std::fabs(TCC::round(tcp.sim_fallback-tcp.threshold));

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
				
		//cff = CostFileFormat::MATRIX;
		//switch(cff)
		//{
		//	case CostFileFormat::FLAT:
		//		LOGI << "DEBUG FLAT FILE FORMAT";
		//		break;
		//	case CostFileFormat::MATRIX:
		//		LOGI << "DEBUG MATRIX FILE FORMAT";
		//		break;
		//}

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
						<< "ERROR FILE COULD NOT BE OPENED -MATRIX FILE" 
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
				<< flat_file_path << std::endl;
			exit(1);
		}
		boost::filesystem::remove(flat_file_path);
		is_external_file_configured = true;
	}


	/////////////////////////////////////////////////////////////////////////////
	//
	// The number of edges in the triangular matrix
	//long expected_num_edges = TCC::calc_num_sym_elem(localId2globalId.size());

	//// boost stuff for setting up the memory mapped file
	//boost::iostreams::mapped_file_params mfp;
	//mfp.path = matrix_file_path;

	//// As we are saving the cost values externally as floats we can
	//// calculate the size of the file we need to create
	//mfp.new_file_size = expected_num_edges*sizeof(float);
	//boost::iostreams::mapped_file_sink mfs;

	//mfs.open(mfp);

	//if(mfs.is_open()){

	//	float* mmf = (float*)mfs.data();

	//	// if not all cost values are available we need to prefill the 
	//	// file with the default value and then overwrite these values
	//	if(num_cost_values < expected_num_edges){
	//		for(unsigned i = 0; i < num_cost_values; i++){
	//			mmf[i] = tcp.sim_fallback-tcp.threshold;
	//		}
	//	}

	//	FILE* tmpFile = fopen(flat_file_path.c_str(),"rb"); 
	//	if(tmpFile != NULL)
	//	{
	//		struct Cost _cost;
	//		while ( fread(&_cost,sizeof(struct Cost), 1, tmpFile) == 1 )
	//		{
	//			std::pair<unsigned,unsigned> ids = TCC::defuse(_cost.id);
	//			float val = _cost.cost;

	//			// get the indexes of the nodes for which the cost belongs <- shakespear level commenting... sorry
	//			unsigned i,j;
	//			i = globalId2localId[ids.first];
	//			j = globalId2localId[ids.second];

	//			// insert the cost at the correct position
	//			mmf[index(i,j)] = val;
	//		}
	//		fclose(tmpFile);
	//	}else{
	//		std::cout 
	//			<< "ERROR FILE COULD NOT BE OPENED " 
	//			<< flat_file_path << std::endl;
	//		exit(1);
	//	}
	//	mfs.close();
	//}
}
