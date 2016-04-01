#ifndef CONNECTEDCOMPONENT_HPP
#define CONNECTEDCOMPONENT_HPP
#include <string>
#include <vector>
#include "TriangularMatrix.hpp"

static unsigned _cc_id(0);

class ConnectedComponent{
   public:
      ConnectedComponent(const std::string &filename);
      ConnectedComponent(const ConnectedComponent& cc,const std::vector<unsigned>& objects, float th);  
      /*
      ConnectedComponent(const std::vector<std::vector<float>>& pos,float th);
      */
      inline const TriangularMatrix& getMatrix() const {return m;}
      inline const unsigned size()const{return m.getNumObjects();}
      inline const float getMinSimilarity() const {return m.getMinValue();}
      inline const float getMaxSimilarity() const {return m.getMaxValue();}
      inline const std::vector<std::string>& getObjectNames() const {return m.getObjectNames();}
      inline const float at(unsigned i, unsigned j,bool normalized = true) const {
         if(normalized){
            return (m(i,j)-threshold)/normalization_context;
         }else{
            return m(i,j)-threshold;
         }
      }
      inline const float getNormalizationContext() const {return normalization_context;}
      inline const float getThreshold() const {return threshold;}

      inline const unsigned getObjectId(unsigned i) const {return m.getObjectId(i);}
      inline const unsigned getId() const {return id;}

      void dump();

   private:
      TriangularMatrix m;
      float threshold;
      float normalization_context;
      float cost;
      unsigned id;

      inline static unsigned getNewId(){
         _cc_id++;
         return _cc_id;
      }
};
#endif
