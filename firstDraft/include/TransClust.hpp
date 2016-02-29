#ifndef TRANSCLUST_HPP
#define TRANSCLUST_HPP
#include <string>
#include <vector>
#include <TriangularMatrix.hpp>

class TransClust{
   public:
      TransClust(const std::string &filename);
      int cluster(TriangularMatrix &costMatrix);
 //  private:
 //     std::vector<TriangularMatrix> findConnectedComponents(TriangularMatrix &m,double threshold);
 //     std::vector<std::vector<int>> findMembershipVector(TriangularMatrix &m,double threshold);
};
#endif
