#include <TriangularMatrix.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>
#include <limits>  

TriangularMatrix::TriangularMatrix(
      const TriangularMatrix &m,
      const std::vector<unsigned> &objects,
      double th){

   threshold = th;
   std::map<std::string, int> object2index;
   std::map<std::string,double> sim_value;
   maxValue = std::numeric_limits<double>::min();
   minValue = std::numeric_limits<double>::max();

   unsigned num_o= objects.size();
   unsigned msize = ((num_o*num_o)-num_o)/2;
   index2object.push_back(m.getObjectName(objects.at(0)));
   matrix.resize(msize); 
   for(unsigned i = 1; i < num_o;i++){
      index2object.push_back(m.getObjectName(objects.at(i)));
      for(unsigned j = 0;j < i;j++){
         double val = m(objects.at(i),objects.at(j))-th;
         if(maxValue < val){
            maxValue = val;
         }
         if(minValue > val){
            minValue = val;
         }
         matrix.at(index(i,j)) = val;
      }
   }
}

TriangularMatrix::TriangularMatrix(const std::string &filename){

   std::map<std::string, int> object2index;
   std::map<std::string,double> sim_value;


   maxValue = std::numeric_limits<double>::min();
   minValue = std::numeric_limits<double>::max();
   /*********************************
    * Read the input similarity file
    ********************************/
   std::ifstream fs(filename);
   for(std::string line; std::getline(fs, line); ){
      // split line from similarity file <o1,o2,sim val>
      std::istringstream buf(line);
      std::istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> tokens(beg, end);

      // build objects hashmaps
      if(object2index.empty()){
         index2object.push_back(tokens.at(0)); 
         object2index[tokens.at(0)] = 0;
      }else{
         if(index2object.back() != tokens.at(0)){
            object2index[tokens.at(0)] = index2object.size();
            index2object.push_back(tokens.at(0)); 
         }
      }
      // if o1,o2 || o2,o1 exists in sim_value
      double current_sval = -1;
      double sval = atof(tokens.at(2).c_str());

      if(sval > maxSimilarityValue){
         maxSimilarityValue = sval;
      }
      if(sim_value[tokens.at(0)+tokens.at(1)]){
         current_sval = sim_value[tokens.at(0)+tokens.at(1)];
      }else if(sim_value[tokens.at(1)+tokens.at(0)]){
         current_sval = sim_value[tokens.at(1)+tokens.at(0)];
      }         

      if(current_sval == -1 || current_sval > sval){
         sim_value[tokens.at(0)+tokens.at(1)] = sval;
      }
   }
   unsigned num_o = index2object.size();
   unsigned msize = ((num_o*num_o)-num_o)/2;
   matrix.resize(msize); 

   for(unsigned i = 1; i < num_o;i++){
      for(unsigned j = 0;j < i;j++){
         double val;
         // find the object pair (might be o1o2 or o2o1)
         if(sim_value[index2object[i]+index2object[j]]){
            val = sim_value[index2object[i]+index2object[j]];
         }else if(sim_value[index2object[j]+index2object[i]]){
            val = sim_value[index2object[j]+index2object[i]];
         }else{
            //if it is not present then it is 0
            val = 0;
         }        
         if(maxValue < val){
            maxValue = val;
         }
         if(minValue > val){
            minValue = val;
         }
         matrix.at(index(i,j)) = val; 
      }
   }

}
void TriangularMatrix::dump(){
   unsigned num_o = index2object.size();
   std::cout << num_o << std::endl;
   for(auto &name:index2object){
      std::cout << name << std::endl;
   }
   for(unsigned i = 0; i < num_o;i++){
      for(unsigned j = i+1;j < num_o;j++){
         std::cout << matrix.at(index(i,j))-threshold << "\t";
      }
      std::cout << std::endl;
   }
   std::cout << std::endl;
}
