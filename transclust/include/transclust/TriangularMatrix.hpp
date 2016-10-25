#ifndef TRIANGULARMATRIX_HPP
#define TRIANGULARMATRIX_HPP
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <plog/Log.h>
#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include "transclust/Common.hpp"

class TriangularMatrix{
	public:
		TriangularMatrix(
				TriangularMatrix &m,
				const std::vector<unsigned> &objects,
				unsigned cc_id,
				float threshold,
				float delta_threshold);

		// Read input similarity file and create similarity matrix
		TriangularMatrix(const std::string &filename,TCC::TransClustParams& tcp,unsigned cc_id);

		// Read 1d similarity matrix
		TriangularMatrix(std::vector<float>& sim_matrix_1d,unsigned _num_o,unsigned cc_id);

		/**
		 * Returns the cost of the edge between i and j
		 * OR std::numeric_limits<float>::lowest() if the edge is not present
		 */
		inline float get(unsigned i,unsigned j) {
			switch(cs){
				case CostStructure::COST_MATRIX:
					switch(sm){
						case StorageMethod::EXTERNAL:
							///////////////////////////////////////////////////////////
							// MATRIX FORMAT
							///////////////////////////////////////////////////////////
							if(!is_loaded || !mm_file.is_open()){
								std::cout << data_file_path << std::endl;
								std::cout << "ERROR - file notloaded " << is_loaded << std::endl;
							}

							return *((float*)mm_file.data()+index(i,j));
							break;
						case StorageMethod::INTERNAL:
							///////////////////////////////////////////////////////////
							// MATRIX FORMAT
							///////////////////////////////////////////////////////////
							return matrix.at(index(i,j));
							break;
					}
					break;
				case CostStructure::COST_MAP:
					switch(sm){
						case StorageMethod::EXTERNAL:
							if(!is_loaded){
								std::cout << data_file_path << std::endl;
								std::cout << "ERROR - file not loaded " << is_loaded << std::endl;
							}
						case StorageMethod::INTERNAL:
							///////////////////////////////////////////////////////////
							// MAP FORMAT
							///////////////////////////////////////////////////////////
							long key = TCC::fuse(i,j);
							if(cost_map.find(key) != cost_map.end())
							{
								return cost_map[key];
							}else{
								return std::numeric_limits<float>::lowest();
							}
							break;
					}
					break;
			}
			std::cout << "ERROR " << __LINE__ << " " << __FILE__ << std::endl;
			return 0;
		};

		inline void load(){
			LOGD << "Loading TriangularMatrix with id " << id;
			if(sm == StorageMethod::EXTERNAL){
				if(num_o > 1){
					switch(cs){
						case CostStructure::COST_MAP:
							{
								std::ifstream fs(data_file_path);
								for (std::string line; std::getline(fs, line); )
								{
									std::istringstream buf(line);
									std::istream_iterator<std::string> beg(buf), end;
									std::vector<std::string> tokens(beg, end);
									
									cost_map.insert( 
											std::make_pair(
												std::stol(tokens.at(0)),
												std::stof(tokens.at(1))
												) 
											);
								}
								fs.close();
							
							}
							break;
						case CostStructure::COST_MATRIX:
							mm_file.open(data_file_path);
							break;
					}
				}
				is_loaded = true;
			}
		};
		inline void free(){
			if(sm == StorageMethod::EXTERNAL){
				if(num_o > 1){
					switch(cs){
						case CostStructure::COST_MAP:
							break;
						case CostStructure::COST_MATRIX:
							mm_file.close();
							break;
					}
				}
				LOGD << "Freeing TriangularMatrix with id " << id;

				// delete old data file
				boost::filesystem::remove(data_file_path);
				is_loaded = false;
			}
		};

		// convineance functions
		inline float getMaxValue() const {return maxValue;}
		inline float getMinValue() const {return minValue;}
		inline const std::string getObjectName(unsigned i) const {return index2ObjName.at(i);};
		inline const std::vector<std::string>& getObjectNames() const {return index2ObjName;};
		inline const unsigned getNumObjects() const {return num_o;};
		inline const unsigned getObjectId(unsigned i) const {return index2ObjId.at(i);};
		inline const unsigned getMatrixSize() const {return matrix.size();};
		inline const TCC::TransClustParams& getTcp() const {return tcp;}

	private:
		// enum for file type. used in parsing inputfile. since there can be 
		// billions of edges we want to reduce computation as much as possible and
		// string comparisons can be relativly costly
		enum FileType {SIMPLE,LEGACY};

		enum StorageMethod {INTERNAL,EXTERNAL};

		enum CostStructure {COST_MATRIX,COST_MAP};

		unsigned id;

		FileType ft;

		StorageMethod sm;

		CostStructure cs;

		// number of obejcts
		unsigned num_o;

		// number of edges
		unsigned long msize;

		// matrix (1d vector) storing cost values
		std::vector<float> matrix;

		// map for storing cost values
		boost::unordered_map<long,float> cost_map;


		float maxValue = 0;
		float minValue = 0;
		std::vector<std::string> index2ObjName;
		std::vector<unsigned> index2ObjId;
		TCC::TransClustParams tcp;


		/* EXTERNAL MEMORY */
		std::string data_file_path;
		boost::iostreams::mapped_file_source mm_file;
		bool is_loaded = false;

		void writeToFile(
				std::map<std::string, unsigned>& object2index,
				std::map<std::pair<std::string, std::string>, float> & sim_value,
				std::map<std::pair<std::string, std::string>, bool> &hasPartner
				);

		void writeToMemory(
				std::map<std::string, unsigned>& object2index,
				std::map<std::pair<std::string, std::string>, float> & sim_value,
				std::map<std::pair<std::string, std::string>, bool> &hasPartner
				);

		float parseLegacyEdge(
				std::map<std::string, unsigned>& object2index,
				std::map<std::pair<std::string, std::string>, float> & sim_value,
				std::map<std::pair<std::string, std::string>, bool> &hasPartner,
				unsigned i,
				unsigned j);

		float parseSimpleEdge(
				std::vector<std::pair<unsigned,unsigned>>& positive_inf,
				std::map<std::string, unsigned> &object2index,
				std::map<std::pair<std::string, std::string>, float> & sim_value,
				std::map<std::pair<std::string, std::string>, bool> &hasPartner,
				unsigned i,
				unsigned j);

		void readFile(
				const std::string &filename,
				std::map<std::string, unsigned> &object2index,
				std::map<std::pair<std::string, std::string>, float> & sim_value,
				std::map<std::pair<std::string, std::string>, bool> &hasPartner
				);

		inline void setCostStructure(){
			if(msize > (unsigned long)matrix.max_size()){
				cs = CostStructure::COST_MAP;
			}else{
				cs = CostStructure::COST_MATRIX;
			}
			//debug
			//cs = CostStructure::COST_MAP;
		};

		inline void writePrecision(
				std::ofstream& ofs,
				unsigned i, 
				unsigned j, 
				float val)
		{
			ofs << TCC::fuse(i,j) <<"\t"<< std::fixed << std::setprecision(8) << val<< "\n";
		};

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
					std::to_string(id) + 
					"_objects_" + 
					std::to_string(num_o) + 
					".bcm"
					);
			return dir / _file;
		};

		// indexing the symetric matrix (column-major)
		inline unsigned long index(unsigned i,unsigned j) const {
			/* row-wise index */
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
				std::cout << "Error: attempt to index diagonal in TriangularMatrix" << std::endl;
				return 0;
			}
			return (num_o*i - (i+1)*(i)/2 + j-(i+1));
		};
};

#endif
