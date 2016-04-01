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
      TriangularMatrix(const std::string &filename);  

      // read from x_1,x_2,...,x_n coordinates
      TriangularMatrix(const std::vector<std::vector<float>> &pos);

      // accessing values of the matrix
      inline float &operator()(unsigned i,unsigned j) {return matrix.at(index(i,j));};
      inline const float& operator()(unsigned i,unsigned j)const {return matrix.at(index(i,j));};


      // convineance functions
      inline float getMaxValue() const {return maxValue;}
      inline float getMinValue() const {return minValue;}
      inline const std::string getObjectName(unsigned i) const {return index2ObjName.at(i);};
      inline const std::vector<std::string>& getObjectNames() const {return index2ObjName;};
      inline const unsigned getNumObjects() const {return index2ObjName.size();}
      inline const unsigned getObjectId(unsigned i) const {return index2ObjId.at(i);}

   private:
      // indexing the symetric matrix
      inline unsigned index(unsigned i,unsigned j) const {
         if(j > i){
            unsigned tmp = i;
            i = j;j = tmp;
         }else if(i == j){
            std::cout << "Error: attempt to index diagonal in TriangularMatrix" << std::endl;
            return 0;
         }
         return (((i*(i-1))/2)+j);
      };
      std::vector<float> matrix;
      float maxValue = 0;
      float minValue = 0;
      std::vector<std::string> index2ObjName;
      std::vector<unsigned> index2ObjId;
};

#endif
