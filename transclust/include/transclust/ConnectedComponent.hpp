#ifndef CONNECTEDCOMPONENT_HPP
#define CONNECTEDCOMPONENT_HPP
#include <string>
#include <deque>
#include <vector>
#include <cmath>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <plog/Log.h>
#include <boost/algorithm/clamp.hpp>
#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include "transclust/Common.hpp"

class ConnectedComponent
{
	public:
		struct Cost {
			uint64_t id;
			float cost;
			Cost(){}
			Cost(uint64_t _id, float _cost)
				:id(_id), cost(_cost)
			{ }
			
		};
		ConnectedComponent(unsigned id,long _num_positive_edges ,TCC::TransClustParams& _tcp);

		inline void addNode(unsigned id)
		{
			localId2globalId.push_back(id);
			globalId2localId.insert(std::make_pair(id,localId2globalId.size()-1));
		}

		inline unsigned getId(){ return cc_id; }
		inline unsigned size(){ return localId2globalId.size(); }
		//inline long getNumEdges(){return num_positive_edges;}
		inline bool isTransitive(){ return transitive; }
		//inline float getMaxCost(){ return max_cost; }
		inline std::deque<unsigned> getIndex2ObjectId(){ return localId2globalId; }
		inline long getNumConflictingEdges(){ return num_conflicting_edges; }
		inline unsigned getObjectId(unsigned i){ return localId2globalId.at(i); }

		void getBufferedCost(
				std::vector<float>& buffer,
				unsigned i,
				unsigned j,
				bool normalized = true);

		inline float getCost(unsigned i,unsigned j,bool normalized = true){

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



		// TODO might be smart to add a buffer... 
		inline void addCost(uint64_t id, float cost)
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


			FILE* flatFile = fopen(flat_file_path.c_str(),"a");
			if(flatFile != NULL)
			{
				Cost _cost(lid,cost); 
				fwrite(&_cost,sizeof(struct Cost),1,flatFile);
				fclose(flatFile);
			}else{
				std::cout 
					<< "ERROR FILE COULD NOT BE OPENED " 
					<< flat_file_path << std::endl;
				exit(1);
			}

		}

		// the connected component has now been given all known information
		// and now a decision can be made as to how this
		// information can be stored most efficiantly 
		void commit();

		inline void load()
		{
			// check that the file has not already ben configured and that it is 
			// not transitive
			if(!is_mmf_configured && !transitive)
			{
				////////////////////////////////////////////////////////////////////
				// LAOD
				////////////////////////////////////////////////////////////////////
				// If there is no memory limit
				if(tcp.memory_limit == 0)
				{
					// assume it fits in memory
					fit_in_memory = true;

					// load entire file
					switch(cff)
					{
						//////////////////////////////////////////////////////////////
						// MATRIX FILE FORMAT
						//////////////////////////////////////////////////////////////
						case CostFileFormat::MATRIX:
							initMatrixFile();
							mm_file.open(matrix_file_path);
							break;
						//////////////////////////////////////////////////////////////
						// FLAT FILE FORMAT
						//////////////////////////////////////////////////////////////
						case CostFileFormat::FLAT:
							initFlatFile();
							mm_file.open(flat_file_path);
							break;
					}
				}else{
					long f_size = 0;
					switch(cff)
					{
						//////////////////////////////////////////////////////////////
						// MATRIX FILE FORMAT
						//////////////////////////////////////////////////////////////
						case CostFileFormat::MATRIX:
							// get the size of the file (in bytes)
							f_size = TCC::calc_num_sym_elem(localId2globalId.size()); 

							// Test if the file fits within the limited amount of ram
							if(tcp.memory_limit >= ((double)f_size/1024)/1024)
							{
								// the file fits in memory
								// load entire file
								fit_in_memory = true;
								initMatrixFile();
								mm_file.open(matrix_file_path);
							}else{
								// the file needs to be loaded with offset every time we need 
								// a value :(
								fit_in_memory = false;
								initMatrixFile();
							}
							break;
						//////////////////////////////////////////////////////////////
						// FLAT FILE FORMAT
						//////////////////////////////////////////////////////////////
						case CostFileFormat::FLAT:
							// get the size of the file (in bytes)
							f_size = boost::filesystem::file_size(flat_file_path);

							// Test if the file fits within the limited amont of ram
							if(tcp.memory_limit >= ((double)f_size/1024)/1024)
							{
								// the file fits in memory
								// load entire file
								fit_in_memory = true;
								initFlatFile();
								mm_file.open(flat_file_path);
							}else{
								// the file needs to be loaded with offset every time we need 
								// a value :(
								fit_in_memory = false;
								initFlatFile();
							}
							break;
					}
				}

				is_mmf_configured = true;
			}
		}

		inline void free(bool permanent = false)
		{
			if(is_mmf_configured && mm_file.is_open())
			{
				mm_file.close();
			}
			flat_map.clear();
			flat_map_index.clear();

			if(permanent)
			{
				localId2globalId.clear();
				is_external_file_configured = false;

				if(boost::filesystem::exists(matrix_file_path)){
					boost::filesystem::remove(matrix_file_path);
				}
				if(boost::filesystem::exists(flat_file_path)){
					boost::filesystem::remove(flat_file_path);
				}
			}
			is_mmf_configured = false;
		}

	private:
		void initMatrixFile();
		void initFlatFile();

		inline void getInMemoryBufferedCostMatrix(
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
		inline void getInMemoryBufferedCostFlat(
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

		inline void getExternalBufferedCostFlat(
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

		inline void getExternalBufferedCostMatrix(
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

		/**
		 * Contructs a filename and creates path if neccesary
		 */
		inline boost::filesystem::path getFilePath()
		{
			boost::filesystem::path dir(tcp.tmp_dir);
			if(!boost::filesystem::is_directory(dir))
			{
				boost::filesystem::create_directory(dir);
			}
			boost::filesystem::path _file (
					"cm_id_" + 
					std::to_string(cc_id) + 
					"_objects_" + 
					std::to_string(localId2globalId.size()) + 
					".bcm"
					);
			return dir / _file;
		};

		inline boost::filesystem::path getFlatFilePath()
		{
			boost::filesystem::path dir(tcp.tmp_dir);
			if(!boost::filesystem::is_directory(dir))
			{
				boost::filesystem::create_directory(dir);
			}

			boost::filesystem::path _file (
					"cm_id_" + 
					std::to_string(cc_id) + 
					".bcm"
					);
			return dir / _file;
		};

		inline unsigned long index(unsigned i,unsigned j) 
		{
			///* row-major index */
			//if(j > i){
			// std::swap(j,i);
			//}else if(i == j){
			//	std::cout << "Error: attempt to index diagonal in TriangularMatrix" << std::endl;
			//	return 0;
			//}
			//
			//return (((i*(i-1))/2)+j);

			/* column-mojor index */

			if(j < i){
				std::swap(j,i);
			}else if(i == j){
				std::cout << "Error: attempt tojindex diagonal in TriangularMatrix" << std::endl;
				return 0;
			}
			return (size()*i - ((i+1)*i)/2 + j-(i+1));
		};

		unsigned cc_id;
		TCC::TransClustParams tcp;
		boost::unordered_map<unsigned,unsigned> globalId2localId;
		std::deque<unsigned> localId2globalId;
		bool transitive = false;
		long num_positive_edges;
		float max_value = std::numeric_limits<float>::lowest();
		float min_value = std::numeric_limits<float>::max();
		//float max_cost = 0;
		long num_conflicting_edges = 0;
		long num_cost_values = 0;
		enum CostFileFormat {MATRIX,FLAT};
		CostFileFormat cff;

		std::string matrix_file_path;
		std::string flat_file_path;

		boost::unordered_map<unsigned long,float> flat_map;
		std::map<unsigned long,unsigned> flat_map_index;

		boost::iostreams::mapped_file_source mm_file;
		bool is_mmf_configured = false;
		bool is_external_file_configured = false;
		bool fit_in_memory = false;
		float normalization_factor;
};
#endif
