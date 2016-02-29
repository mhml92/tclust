#include <FindConnectedComponents.hpp>
#include <limits.h>
std::vector<TriangularMatrix> findConnectedComponents(TriangularMatrix &m, double threshold){
  
   std::vector<std::vector<unsigned>> membership = findMembershipVector(m,threshold);
   
   std::vector<TriangularMatrix> ccs;
   for(auto &cc:membership){
      ccs.push_back(TriangularMatrix(m,cc,threshold));
   }
   return ccs;

}

/*
 * Current Stupid lameass slow implementation to find cc
 * Gets the triangularmatrix representing the similarity matrix
 * returns a vector of memberships
 */
std::vector<std::vector<unsigned>> findMembershipVector(TriangularMatrix &m, double threshold){
  
   std::vector<unsigned> membership(m.getNumObjects(),UINT_MAX);
   std::vector<std::vector<unsigned>> result;
   result.push_back(std::vector<unsigned>());
   std::queue<unsigned> Q;
   unsigned componentId = 0;
   Q.push(0);
   while(true){

      unsigned i = Q.front();
      Q.pop();

      // element at membership i is in component 'componentId'
      membership.at(i) = componentId;
      result.at(componentId).push_back(i);
      for(unsigned j = 0; j < m.getNumObjects();j++){
         if(j == i){
            continue;
         }
         if(membership.at(j) == UINT_MAX){
            double cost = m(i,j)-threshold;
            if(cost > 0){
               Q.push(j);
               //result.at(componentId).push_back(j);
               membership.at(j) = componentId;
            }
         }
      }
      
      if(Q.empty()){
         for(unsigned s = 0; s < membership.size();s++){
            if(membership.at(s) == UINT_MAX){
               Q.push(s);
               break;
            }
         }
         if(Q.empty()){
            break;
         }
         componentId++;
         result.push_back(std::vector<unsigned>());
      }
   }
   return result;
}
