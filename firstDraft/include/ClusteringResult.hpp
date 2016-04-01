#ifndef CLUSTERINGRESULT_HPP
#define CLUSTERINGRESULT_HPP
#include <vector>
#include "ConnectedComponent.hpp"


class ClusteringResult{
   public:

      ClusteringResult(
            std::vector<unsigned>& best_cluster, 
            float best_cost);

      inline float getCost(){return cost;}
      inline std::vector<unsigned>& getClusters(){return clustering;}
      inline void dump(){
         std::cout << "Cost: " << cost << std::endl;
         for(auto &e:clustering){
            std::cout << e << ",";
         }
         std::cout << std::endl;

      }
   private:
      std::vector<unsigned> clustering;
      float cost;

};
#endif
