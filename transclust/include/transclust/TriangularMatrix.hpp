#ifndef TRIANGULARMATRIX_HPP
#define TRIANGULARMATRIX_HPP
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include "transclust/Common.hpp"

class TriangularMatrix{
	public:
		// Create connected component based on TriangularMatrix and threshold
		TriangularMatrix(TriangularMatrix &m,const std::vector<unsigned> &objects,unsigned cc_id);

		// Read input similarity file and create similarity matrix
		TriangularMatrix(const std::string &filename,TCC::TransClustParams& tcp,unsigned cc_id);

		// Read 1d similarity matrix
		TriangularMatrix(std::vector<float>& sim_matrix_1d,unsigned _num_o,unsigned cc_id);

		inline float get(unsigned i,unsigned j) {
			if(tcp.external){
				if(!is_loaded || !mm_file.is_open()){
					std::cout << bin_file_path << std::endl;
					std::cout << "ERROR - file notloaded " << is_loaded << std::endl;
				}
				return *((float*)mm_file.data()+index(i,j));
			}else{
				return matrix.at(index(i,j));
			}
		};

		inline void load(){
			if(tcp.external){
				if(num_o > 1){
					mm_file.open(bin_file_path);
				}
				is_loaded = true;
			}
		};
		inline void free(){
			if(tcp.external){
				if(num_o > 1){
					mm_file.close();
				}
				boost::filesystem::remove(bin_file_path);
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
		void parseLegacySimDataFile(
				std::map<std::string, unsigned> &object2index,
				std::map<std::pair<std::string, std::string>, float> & sim_value,
				std::map<std::pair<std::string, std::string>, bool> &hasPartner,
				TCC::TransClustParams& tcp);

		void parseSimpleSimDataFile(
				std::map<std::string, unsigned> &object2index,
				std::map<std::pair<std::string, std::string>, float> & sim_value,
				std::map<std::pair<std::string, std::string>, bool> &hasPartner,
				TCC::TransClustParams& tcp);

		float parseSimpleEdge(
				std::vector<std::pair<unsigned,unsigned>>& positive_inf,
				std::map<std::string, unsigned> &object2index,
				std::map<std::pair<std::string, std::string>, float> & sim_value,
				std::map<std::pair<std::string, std::string>, bool> &hasPartner,
				unsigned i,
				unsigned j
				);

		void readFile(
				const std::string &filename,
				std::map<std::string, unsigned> &object2index,
				std::map<std::pair<std::string, std::string>, float> & sim_value,
				std::map<std::pair<std::string, std::string>, bool> &hasPartner
				);

		void load_external_file(){


			
		};
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
		inline unsigned index(unsigned i,unsigned j) const {
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
		unsigned id;
		bool is_external = false;
		unsigned num_o;
		std::vector<float> matrix;
		float maxValue = 0;
		float minValue = 0;
		std::vector<std::string> index2ObjName;
		std::vector<unsigned> index2ObjId;
		TCC::TransClustParams tcp;

		/* EXTERNAL MEMORY */
		std::string bin_file_path;
		boost::iostreams::mapped_file_source mm_file;
		bool is_loaded = false;
};

#endif
