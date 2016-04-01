#include "Result.hpp"
#include <limits>
#include <algorithm>
#include <map>
#include <iomanip>

Result::Result(unsigned rs):resultSize(rs)
{ }

void Result::add(ConnectedComponent& cc, ClusteringResult& cr)
{
   if(thresholdIndex.find(cc.getThreshold()) == thresholdIndex.end())
   {
      // if first cc of threshold
      results.push_back(std::vector<unsigned>(resultSize,std::numeric_limits<unsigned>::max()));
      thresholdClusterIndex.push_back(0);
      thresholdClusterCost.push_back(0);
      thresholdIndex.insert(std::pair<float,unsigned>(cc.getThreshold(),results.size()-1));
   } 

   unsigned index = thresholdIndex.at(cc.getThreshold());
   unsigned cid = thresholdClusterIndex.at(index);

   thresholdClusterCost.at(index) += cr.getCost();
   for(unsigned i = 0; i < cr.getClusters().size();i++)
   {
      results.at(index).at(cc.getObjectId(i)) = cr.getClusters().at(i)+cid;
   }
   thresholdClusterIndex.at(index) += mydistinct(cr.getClusters());

   /*
   for(auto& clustear:cr.getClusters()){
      unsigned cid = thresholdClusterIndex.at(index);
      for(unsigned j = 0; j < cluster.size();j++){
         results.at(index).at(cc.getObjectId(j)) = cid;
      }
      thresholdClusterIndex.at(index) = cid+1;
   }
   */
}



void Result::dump()
{
   for(auto const& th:thresholdIndex)
   {
      unsigned index = th.second;
      std::cout << th.first << "\t" << " - " << "\t";
      for(unsigned i = 0; i < resultSize;i++)
      {
         std::cout << results.at(index).at(i) << ", ";
      }
      std::cout << std::endl;
   }
   
}
