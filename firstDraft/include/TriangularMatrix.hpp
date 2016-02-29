#ifndef TRIANGULARMATRIX_HPP
#define TRIANGULARMATRIX_HPP
#include <string>
#include <map>
#include <vector>
#include <iostream>
class TriangularMatrix{
   public:
      // Create connected component based on TriangularMatrix and threshold
      TriangularMatrix(const TriangularMatrix &m,const std::vector<unsigned> &objects, double th);  
      // Read input similarity file and create similarity matrix
      TriangularMatrix(const std::string &filename);  


      // accessing values of the matrix
      double &operator()(unsigned i,unsigned j) {return matrix.at(index(i,j));};
      const double& operator()(unsigned i,unsigned j)const {return matrix.at(index(i,j));};


      // convineance functions
      double getMaxValue(){return maxValue;}
      double getMinValue(){return minValue;}
      double getThreshold(){return threshold;}
      const std::string getObjectName(int i)const {return index2object.at(i);};
      unsigned getNumObjects(){return index2object.size();}


      // debug print matrix in 'cost matrix' format
      void dump();
   private:
      // indexing the symetric matrix
      unsigned index(unsigned i,unsigned j)const{
         if(j > i){
            unsigned tmp = i;
            i = j;j = tmp;
         }else if(i == j){
            std::cout << "Error: attempt to index diagonal in TriangularMatrix" << std::endl;
            return 0;
         }
         return ((i*(i-1))/2)+j;
      };
      double threshold = 0.0;
      std::vector<double> matrix;
      double maxSimilarityValue = 0;
      double maxValue = 0;
      double minValue = 0;
      std::vector<std::string> index2object;
};
#endif
