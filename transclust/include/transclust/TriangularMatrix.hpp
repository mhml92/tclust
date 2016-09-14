#ifndef TRIANGULARMATRIX_HPP
#define TRIANGULARMATRIX_HPP
#include <string>
#include <map>
#include <vector>
#include <iostream>
class TriangularMatrix{
	public:
		// Create connected component based on TriangularMatrix and threshold
		TriangularMatrix(const TriangularMatrix &m,const std::vector<unsigned> &objects);  

		// Read input similarity file and create similarity matrix
		TriangularMatrix(const std::string &filename,bool use_custom_fallback, double sim_fallback);  

		// read from x_1,x_2,...,x_n coordinates
		TriangularMatrix(const std::vector<std::vector<double> > &pos);

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
		// indexing the symetric matrix
		inline unsigned index(unsigned i,unsigned j) const {
			if(j > i){
				unsigned tmp = i;
				i = j;
				j = tmp;
			}else if(i == j){
				std::cout << "Error: attempt to index diagonal in TriangularMatrix" << std::endl;
				return 0;
			}
			return (((i*(i-1))/2)+j);
		};
		std::vector<double> matrix;
		double maxValue = 0;
		double minValue = 0;
		std::vector<std::string> index2ObjName;
		std::vector<unsigned> index2ObjId;
};

#endif
