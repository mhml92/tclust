#ifndef FINDCONNECTEDCOMPONENTS_HPP
#define FINDCONNECTEDCOMPONENTS_HPP
#include <TriangularMatrix.hpp>
#include <vector>
#include <queue> 

std::vector<TriangularMatrix> findConnectedComponents(TriangularMatrix &m, double threshold);
std::vector<std::vector<unsigned>> findMembershipVector(TriangularMatrix &m, double threshold);

#endif
