#ifndef RESULT_HPP
#define RESULT_HPP
#include <vector>
#include <set>
#include "ConnectedComponent.hpp"
#include "ClusteringResult.hpp"


class Result{
   public:
      Result(unsigned rs);
      void add(ConnectedComponent& cc,ClusteringResult& cr);
      void dump();
   private:

      inline unsigned mydistinct(const std::vector<unsigned>& v)
      {
         std::set<unsigned> distinct_container;

         for(auto curr_int = v.begin(), end = v.end(); // no need to call v.end() multiple times
               curr_int != end;
               ++curr_int)
         {
            // std::set only allows single entries
            // since that is what we want, we don't care that this fails 
            // if the second (or more) of the same value is attempted to 
            // be inserted.
            distinct_container.insert(*curr_int);
         }

         return distinct_container.size();
      }
      unsigned resultSize;
      std::vector<std::vector<unsigned>> results;
      std::vector<unsigned> thresholdClusterIndex;
      std::vector<float> thresholdClusterCost;
      std::map<float,int> thresholdIndex;

};


#endif
