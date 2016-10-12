#ifndef TRIANGULARMATRIX_HPP
#define TRIANGULARMATRIX_HPP
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "transclust/Common.hpp"

class TriangularMatrix{
	public:
		// Create connected component based on TriangularMatrix and threshold
		TriangularMatrix(const TriangularMatrix &m,const std::vector<unsigned> &objects,unsigned cc_id);

		// Read input similarity file and create similarity matrix
		TriangularMatrix(const std::string &filename,TCC::TransClustParams& tcp,unsigned cc_id);

		// Read 1d similarity matrix
		TriangularMatrix(std::vector<double>& sim_matrix_1d,unsigned _num_o,unsigned cc_id);//,bool use_custom_fallback, double sim_fallback);

		// accessing values of the matrix
		inline double &operator()(unsigned i,unsigned j) {return matrix.at(index(i,j));};
		inline const double& operator()(unsigned i,unsigned j)const {return matrix.at(index(i,j));};


		// convineance functions
		inline double getMaxValue() const {return maxValue;}
		inline double getMinValue() const {return minValue;}
		inline const std::string getObjectName(unsigned i) const {return index2ObjName.at(i);};
		inline const std::vector<std::string>& getObjectNames() const {return index2ObjName;};
		inline const unsigned getNumObjects() const {return index2ObjName.size();};
		inline const unsigned getObjectId(unsigned i) const {return index2ObjId.at(i);};
		inline const unsigned getMatrixSize() const {return matrix.size();};

	private:
		void parseLegacySimDataFile(
				std::map<std::string, unsigned> &object2index,
				std::map<std::pair<std::string, std::string>, double> & sim_value,
				std::map<std::pair<std::string, std::string>, bool> &hasPartner,
				TCC::TransClustParams& tcp);

		void parseSimpleSimDataFile(
				std::map<std::string, unsigned> &object2index,
				std::map<std::pair<std::string, std::string>, double> & sim_value,
				std::map<std::pair<std::string, std::string>, bool> &hasPartner,
				TCC::TransClustParams& tcp);

		double parseSimpleEdge(
				std::vector<std::pair<unsigned,unsigned>>& positive_inf,
				std::map<std::string, unsigned> &object2index,
				std::map<std::pair<std::string, std::string>, double> & sim_value,
				std::map<std::pair<std::string, std::string>, bool> &hasPartner,
				unsigned i,
				unsigned j,
				TCC::TransClustParams& tcp
				);

		void readFile(
				const std::string &filename,
				std::map<std::string, unsigned> &object2index,
				std::map<std::pair<std::string, std::string>, double> & sim_value,
				std::map<std::pair<std::string, std::string>, bool> &hasPartner
				);

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
		unsigned num_o;
		std::vector<double> matrix;
		double maxValue = 0;
		double minValue = 0;
		std::vector<std::string> index2ObjName;
		std::vector<unsigned> index2ObjId;
};

#endif
