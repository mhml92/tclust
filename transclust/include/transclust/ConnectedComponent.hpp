#ifndef CONNECTEDCOMPONENT_HPP
#define CONNECTEDCOMPONENT_HPP
#include <string>
#include <vector>
#include <cmath>
#include <math.h>
#include "transclust/TriangularMatrix.hpp"

static unsigned _cc_id(0);

class ConnectedComponent
{
	public:
		ConnectedComponent(const std::string &filename,bool use_custom_fallback,double sim_fallback,std::string ft);
		ConnectedComponent(std::vector<double>& sim_matrix_1d,unsigned num_o,bool use_custom_fallback,double sim_fallback);
		ConnectedComponent(const ConnectedComponent& cc,const std::vector<unsigned>& objects, double th);
		/*
			ConnectedComponent(const std::vector<std::vector<double>>& pos,double th);
			*/
		inline const TriangularMatrix& getMatrix() const { return m; }
		inline const unsigned size()const { return m.getNumObjects(); }
		inline const double getMinSimilarity() const{ return m.getMinValue(); }
		inline const double getMaxSimilarity() const{ return m.getMaxValue(); }
		inline const std::vector<std::string>& getObjectNames() const
		{
			return m.getObjectNames();
		}
		inline const std::string getObjectName(unsigned i)const { return m.getObjectName(i);}
		inline const double at(unsigned i, unsigned j, bool normalized = true) const
		{
			if (normalized)
			{
				//double cost = (m(i, j) - threshold) / normalization_context;
				//if(cost < -1 || cost > 1){std::cout << cost << std::endl;}
				return ( std::rint( ( (m(i, j) - threshold ) / normalization_context )*100000)/100000);
			}
			else
			{
				return m(i, j) - threshold;
			}
		}
		inline const double getNormalizationContext() const
		{
			return normalization_context;
		};
		inline const double getThreshold() const
		{
			return threshold;
		};

		inline const unsigned getObjectId(unsigned i) const
		{
			return m.getObjectId(i);
		};
		inline const unsigned getId() const
		{
			return id;
		};
		inline const unsigned getMatrixSize() const
		{
			return m.getMatrixSize();
		};

		void dump();

	private:
		TriangularMatrix m;
		double threshold;
		double normalization_context;
		double cost;
		unsigned id;

		inline static unsigned getNewId()
		{
			_cc_id++;
			return _cc_id;
		}
};
#endif
