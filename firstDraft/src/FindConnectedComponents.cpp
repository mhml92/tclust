#include <FindConnectedComponents.hpp>
#include <ConnectedComponent.hpp>
#include <math.h> 
#include <iomanip>

#include <limits>

namespace fcc{
   /*******************************************************************************
    * FIND CCs IN CC W. THRESHOLD
    ******************************************************************************/
   void findConnectedComponents(
         const ConnectedComponent &cc, 
         std::queue<ConnectedComponent> &ccs,
         const float threshold) 
   {
      std::vector<std::vector<unsigned>> membership = findMembershipVector(cc,threshold);

      //std::cout << cc.getThreshold() << "\t\t\t" << membership.size() <<"\t\t\t"<< cc.size() << std::endl;
      for(auto &ccv:membership)
      {
         ccs.push(ConnectedComponent(cc,ccv,threshold));
      }
   }

   /*******************************************************************************
    * DETERMINE MEMBERSHIP IN CC 
    ******************************************************************************/
   std::vector<std::vector<unsigned>> findMembershipVector(
         const ConnectedComponent &cc, 
         const float threshold)
   {

      std::vector<unsigned> membership(cc.size(),std::numeric_limits<unsigned>::max());
      std::vector<std::vector<unsigned>> result;
      result.push_back(std::vector<unsigned>());
      std::queue<unsigned> Q;

      unsigned delta_threshold = threshold-cc.getThreshold();
      unsigned componentId = 0;
      Q.push(0);
      membership.at(0) = componentId;
      result.at(componentId).push_back(0);
      while(true){

         unsigned i = Q.front();
         Q.pop();

         for(unsigned j = 0; j < cc.size();j++)
         {
            if(membership.at(j) == std::numeric_limits<unsigned>::max())
            {
               float cost = cc.at(i,j,false)-delta_threshold;
               if(cost > 0)
               {
                  Q.push(j);
                  membership.at(j) = componentId;
                  result.at(componentId).push_back(j);
               }
            }
         }

         if(Q.empty())
         {
            componentId++;
            // could be done smarter by by saving 'last known nonmember'
            for(unsigned s = 0; s < membership.size();s++)
            {
               if(membership.at(s) == std::numeric_limits<unsigned>::max())
               {
                  Q.push(s);
                  membership.at(s) = componentId;
                  result.push_back(std::vector<unsigned>());
                  result.at(componentId).push_back(s);
                  break;
               }
            }
            if(Q.empty()){
               break;
            }
         }
      }
      /*
      std::cout << "membership:" << std::endl;
      for(unsigned i = 0; i < membership.size(); i++){
         std::cout << std::setw(5)<< i << " " << membership.at(i) << std::endl;
      }
      */
      return result;
   }

   










/*

   float dist(const std::vector<std::vector<float>>& pos,unsigned i, unsigned j)
   {
      float dist = 0;
      for(unsigned d = 0; d < pos[0].size(); d++){
         float side = pos[i][d] - pos[j][d]; 
         dist += pow(side,2);
      }
      return sqrt(dist);
   }

   // *******************************************************************************
    * FIND CCs IN POS W. Distance
    ****************************************************************************** //
   void findConnectedComponents(
         const ConnectedComponent& cc,
         const std::vector<std::vector<float>>& pos,
         std::vector<ConnectedComponent>& ccs,
         const float maxDist)
   { 
      std::vector<std::vector<unsigned>> membership = findMembershipVector(cc,pos,ccs,maxDist);


      for(auto &ccv:membership){
         ccs.push_back(ConnectedComponent(cc,ccv,cc.getThreshold()));
      }

   }

   // *******************************************************************************
    * DETERMINE MEMBERSHIP IN POS 
    ****************************************************************************** //
   std::vector<std::vector<unsigned>> findMembershipVector(
         const ConnectedComponent& cc,
         const std::vector<std::vector<float>>& pos,
         std::vector<ConnectedComponent>& ccs,
         const float maxDist)
   {
      std::vector<unsigned> membership(cc.size(),std::numeric_limits<unsigned>::max());

      std::vector<std::vector<unsigned>> result;
      result.push_back(std::vector<unsigned>());

      if(ccs.empty()){

         std::queue<unsigned> Q;
         unsigned componentId = 0;
         Q.push(0);
         while(true){

            unsigned i = Q.front();
            Q.pop();

            // element at membership i is in component 'componentId'
            membership.at(i) = componentId;
            result.at(componentId).push_back(i);
            for(unsigned j = 0; j < cc.size();j++){
               if(j == i){
                  continue;
               }
               if(membership.at(j) == std::numeric_limits<unsigned>::max()){
                  if(fcc::dist(pos,i,j) < maxDist){
                     Q.push(j);
                     membership.at(j) = componentId;
                  }
               }
            }

            if(Q.empty()){
               // could be done smarter by by saving 'last known nonmember'
               for(unsigned s = 0; s < membership.size();s++){
                  if(membership.at(s) == std::numeric_limits<unsigned>::max()){
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
      }else{
         for(auto& c:ccs){
            std::queue<unsigned> Q;
            unsigned componentId = 0;
            Q.push(0);
            while(true){

               unsigned i = Q.front();
               Q.pop();

               // element at membership i is in component 'componentId'
               membership.at(i) = componentId;
               result.at(componentId).push_back(i);
               for(unsigned j = 0; j < cc.size();j++){
                  if(j == i){
                     continue;
                  }
                  if(membership.at(j) == std::numeric_limits<unsigned>::max()){
                     if(fcc::dist(pos,c.getObjectId(i),c.getObjectId(j)) < maxDist){
                        Q.push(j);
                        membership.at(j) = componentId;
                     }
                  }
               }

               if(Q.empty()){
                  // could be done smarter by by saving 'last known nonmember'
                  for(unsigned s = 0; s < membership.size();s++){
                     if(membership.at(s) == std::numeric_limits<unsigned>::max()){
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
         }


      }
      return result;
      */
      /*

      std::vector<unsigned> membership(cc.size(),std::numeric_limits<unsigned>::max());
      std::vector<std::vector<unsigned>> result;

      unsigned componentId = 0;

      for(unsigned i = 0; i < cc.size()-1; i++){
         if(membership.at(i) == std::numeric_limits<unsigned>::max()){

            result.push_back(std::vector<unsigned>());
            result.at(componentId).push_back(i);
            membership.at(i) = componentId; 

            for(unsigned j = i+1;j < cc.size(); j++){
               if(fcc::dist(pos,i,j) < maxDist){
                  if(membership.at(j) == std::numeric_limits<unsigned>::max()){
                     result.at(componentId).push_back(j);
                     membership.at(j) = componentId; 
                  }else{

                  }
               }
            }
         }
         componentId++;
      }

      return result;
      */
  // }

}
