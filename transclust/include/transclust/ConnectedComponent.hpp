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
		struct CompareLess
		{
			bool operator () (const uint64_t & a, const uint64_t & b) const
			{ return a < b; }

			static uint64_t max_value()
			{ 
				return std::numeric_limits<uint64_t>::max(); 
			}
		};
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
			index2objectId.push_back(id);
			objectId2index.insert(std::make_pair(id,index2objectId.size()-1));
		}

		inline unsigned getId(){return cc_id;}
		inline unsigned size(){return index2objectId.size();}
		//inline long getNumEdges(){return num_positive_edges;}
		inline bool isTransitive(){return transitive;}
		inline float getThreshold(){return tcp.threshold;}
		inline float getMaxCost(){return max_cost;}
		inline std::deque<unsigned> getIndex2ObjectId(){return index2objectId;}
		inline long getNumConflictingEdges(){return num_conflicting_edges;}
		inline unsigned getObjectId(unsigned i){return index2objectId.at(i);}
		inline float getCost(unsigned i,unsigned j,bool normalized = true){

			float cost =  *((float*)mm_file.data()+index(i,j));


			///////////////////////////////////////////////////////////////////////
			// BINARY SEARCH A SORTED FILE FOR AN ID-COST PAIR
			///////////////////////////////////////////////////////////////////////
			//uint64_t key = TCC::fuse(i,j);
			////Cost* dp = ((Cost*)mm_file.data());
			//float cost = std::numeric_limits<float>::lowest();
			//long L = 0;
			//long R = num_data_values-1;
			//while(L < R){
			//	long m = floor((L+R)/2);
			//	Cost c = *((Cost*)mm_file.data()+m);
			//	//LOGD << "id: " << c.id;
			//	//LOGD << "cost: " << c.cost;
			//	if(c.id < key){
			//		L = m+1;
			//	}else if(c.id > key){
			//		R = m-1;
			//	}else{
			//		//LOGD << "found :D " << key;
			//		cost = c.cost;
			//		break;
			//	}
			//}

			//if((*pcost_map).find(key) != (*pcost_map).end()){
			//	cost = (*pcost_map)[key]; 
			//}else{
			//	cost = TCC::round(tcp.sim_fallback-tcp.threshold);
			//}

			if(cost == std::numeric_limits<float>::max()){
				return 1;
			}

			if(normalized){
				if(cost > 0){
					return TCC::round( boost::algorithm::clamp(cost / normalization_context_positive,-1,1));
				}else{
					return TCC::round( boost::algorithm::clamp(cost / normalization_context_negative,-1,1));
				}
			}else{
				return cost;
			}
		}
		inline void addCost(uint64_t id, float cost)
		{
			if(maxValue < cost){maxValue = cost;};
			if(minValue > cost){minValue = cost;};

			FILE* tmpFile = fopen(tmp_file_path.c_str(),"a");
			if(tmpFile != NULL)
			{
				Cost _cost(id,cost); 
				fwrite(&_cost,sizeof(struct Cost),1,tmpFile);
				fclose(tmpFile);
			}else{
				std::cout << "ERROR FILE COULD NOT BE OPENED " << tmp_file_path << std::endl;
				exit(1);
			}

		}

		// the connected component have now been given all known information
		// and based on this can a decision be made as how to store this
		// information most efficiantly 
		inline void commit(){

			// get a filepath for the external file
			data_file_path = getFilePath().string();

			// The number of edges in the triangular matrix
			long expected_num_edges = TCC::calc_num_sym_elem(index2objectId.size());

			// number of edges negative weight edges (these edges contribute to the
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
				if(num_data_values < expected_num_edges){
					float fallback = TCC::round(tcp.sim_fallback-tcp.threshold);
					if(maxValue < fallback){maxValue = fallback;}
					if(minValue > fallback){minValue = fallback;}
				}

				LOGD << "writing bin file w name: " << data_file_path;
				

				// boost stuff for setting up the memory mapped file
				boost::iostreams::mapped_file_params mfp;
				mfp.path = data_file_path;

				// As we are saving the cost values externally as floats we can
				// calculate the size of the file we need to create
				mfp.new_file_size = (((objectId2index.size()*objectId2index.size())-objectId2index.size())/2)*sizeof(float);
				boost::iostreams::mapped_file_sink mfs;

				mfs.open(mfp);

				if(mfs.is_open()){

					float* mmf = (float*)mfs.data();

					// if not all cost values are available we need to prefill the 
					// file with the default value and then overwrite these values
					if(num_data_values < expected_num_edges){
						for(unsigned i = 0; i < num_data_values; i++){
							mmf[i] = tcp.sim_fallback;
						}
					}

					FILE* tmpFile = fopen(tmp_file_path.c_str(),"rb"); 
					if(tmpFile != NULL)
					{
						struct Cost _cost;
						while ( fread(&_cost,sizeof(struct Cost), 1, tmpFile) == 1 )
						{
							std::pair<unsigned,unsigned> ids = TCC::defuse(_cost.id);
							float val = _cost.cost;

							// get the indexes of the nodes for which the cost belongs <- shakespear level commenting... sorry
							unsigned i,j;
							i = objectId2index[ids.first];
							j = objectId2index[ids.second];

							// insert the cost at the correct position
							mmf[index(i,j)] = val;
						}
						fclose(tmpFile);
					}else{
						std::cout << "ERROR FILE COULD NOT BE OPENED " << tmp_file_path << std::endl;
						exit(1);
					}


					/////////////////////////////////////////////////////////////////

					// get each cost and put it in the external file at the correct 
					// position
					//external_cost_map_type::iterator it;
					//for (it = (*pcost_map).begin(); it != (*pcost_map).end(); ++it)
					//{
					//	// parse the data from the map
					//	std::pair<unsigned,unsigned> ids = TCC::defuse(it->first);
					//	float val = it->second;

					//	// get the indexes of the nodes for which the cost belongs <- shakespear level commenting... sorry
					//	unsigned i,j;
					//	i = objectId2index[ids.first];
					//	j = objectId2index[ids.second];

					//	// insert the cost at the correct position
					//	mmf[index(i,j)] = val;
					//}

					//FILE* f = fopen(data_file_path.c_str(),"w");
					//external_cost_map_type::iterator it;
					//for (it = (*pcost_map).begin(); it != (*pcost_map).end(); ++it)
					//{
					//	Cost _cost(it->first,it->second);
					//	fwrite(&_cost,sizeof(struct Cost),1,f);
					//}
					//fclose(f);

					mfs.close();
				}
			}

			// set the normmalization variables
			if(tcp.normalization == "ABSOLUTE"){
				// this is standard normalization
				normalization_context_positive = std::max( std::fabs(minValue),std::fabs(maxValue) );
				normalization_context_negative = std::max( std::fabs(minValue),std::fabs(maxValue) );
			}else if(tcp.normalization == "RELATIVE"){
				// this normalization favors positive values. Can in some situations 
				// produce better results
				normalization_context_positive = std::fabs(maxValue);
				normalization_context_negative = std::fabs(maxValue);
			}

			LOGD << "writing bin file...done";

			// clear old data
			objectId2index.clear();
			boost::filesystem::remove(tmp_file_path);
		}

		void load(){
			if(!is_loaded && !transitive){
				mm_file.open(data_file_path);
				is_loaded = true;
			}
		}

		void free(){
			if(is_loaded){
				mm_file.close();
				boost::filesystem::remove(data_file_path);
			}
			is_loaded = false;
		}

		//void dump();

	private:
		/**
		 * Contructs a filename and creates path if neccesary
		 */
		inline boost::filesystem::path getFilePath(){
			boost::filesystem::path dir (tcp.tmp_dir);
			if(!boost::filesystem::is_directory(dir)){
				boost::filesystem::create_directory(dir);
			}
			boost::filesystem::path _file (
					"cm_id_" + 
					std::to_string(cc_id) + 
					"_objects_" + 
					std::to_string(index2objectId.size()) + 
					".bcm"
					);
			return dir / _file;
		};
		inline boost::filesystem::path getTmpFilePath(){
			boost::filesystem::path dir (tcp.tmp_dir);
			if(!boost::filesystem::is_directory(dir)){
				boost::filesystem::create_directory(dir);
			}
			boost::filesystem::path _file (
					"cm_id_" + 
					std::to_string(cc_id) + 
					".bcm"
					);
			return dir / _file;
		};

		inline unsigned long index(unsigned i,unsigned j) {
			/* row-wise index */
			if(j > i){
			 std::swap(j,i);
			}else if(i == j){
				std::cout << "Error: attempt to index diagonal in TriangularMatrix" << std::endl;
				return 0;
			}
			
			return (((i*(i-1))/2)+j);

			/* column-mojor index */

			//if(j < i){
			//	std::swap(j,i);
			//}else if(i == j){
			//	std::cout << "Error: attempt to index diagonal in TriangularMatrix" << std::endl;
			//	return 0;
			//}
			//return (size()*i - (i+1)*(i)/2 + j-(i+1));
		};

		enum StorageType {MATRIX,MAP};

		unsigned cc_id;
		TCC::TransClustParams tcp;
		//external_cost_vector_type cost_matrix;
		StorageType st = StorageType::MATRIX;
		boost::unordered_map<unsigned,unsigned> objectId2index;
		std::deque<unsigned> index2objectId;
		bool transitive = false;
		long num_positive_edges;
		float maxValue = std::numeric_limits<float>::lowest();
		float minValue = std::numeric_limits<float>::max();
		float max_cost = 0;
		long num_conflicting_edges = 0;
		long num_data_values = 0;

		std::string data_file_path;
		std::string tmp_file_path;
		boost::iostreams::mapped_file_source mm_file;
		bool is_loaded = false;
		float normalization_context_positive;
		float normalization_context_negative;
		
		//enum StorageMathod {INTERNAL_MAP,INTERNAL_MATRIX,EXTERNAL_MAP,EXTERNAL_MATRIX};
		/*
		inline void init_normalization_context(TCC::TransClustParams& tcp)
		{
			if(tcp.normalization == "RELATIVE"){

				float max_val = std::abs(m.getMaxValue());
				if(max_val != 0){
					normalization_context_positive = max_val;	
				}else{
					normalization_context_positive = 1;	
				}
				float min_val = std::abs(m.getMinValue()-threshold);
				if(min_val != 0){
					normalization_context_negative = min_val;	
				}else{
					normalization_context_negative = 1;	
				}

			}else if(tcp.normalization == "ABSOLUTE"){
				float max_val = std::max(
						std::abs(m.getMaxValue()),
						std::abs(m.getMinValue())
						);
				normalization_context_positive = max_val;
				normalization_context_negative = max_val;
			}
		
		}
		float threshold;
		TriangularMatrix m;
		float normalization_context_positive;
		float normalization_context_negative;
		//float cost;

		inline static unsigned getNewId()
		{
			_cc_id++;
			return _cc_id;
		}
		*/
};
#endif
