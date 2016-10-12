#ifndef CONNECTEDCOMPONENT_HPP
#define CONNECTEDCOMPONENT_HPP
#include <string>
#include <vector>
#include <cmath>
#include <math.h>
#include "transclust/Common.hpp"
#include "transclust/TriangularMatrix.hpp"

static unsigned _cc_id(0);

class ConnectedComponent
{
	public:
		//ConnectedComponent(const std::string &filename,bool use_custom_fallback,double sim_fallback,std::string ft);
		ConnectedComponent(const std::string &filename,TCC::TransClustParams& tcp);
		//ConnectedComponent(std::vector<double>& sim_matrix_1d,unsigned num_o,bool use_custom_fallback,double sim_fallback);
		ConnectedComponent(std::vector<double>& sim_matrix_1d,unsigned num_o,TCC::TransClustParams& tcp);//,bool use_custom_fallback,double sim_fallback);
		ConnectedComponent(const ConnectedComponent& cc,const std::vector<unsigned>& objects, double th,TCC::TransClustParams& tcp);
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

		// get cost for some threshold
		inline const double getCost(unsigned i, unsigned j, double t) const { return TCC::round(m(i, j) - t);}

		// get the value of the edge in the cc 
		inline const double at(unsigned i, unsigned j, bool normalized = true) const
		{
			double sim = m(i, j) - threshold;
			if (normalized)
			{

				if( sim > 0){
					return TCC::round(sim / normalization_context_positive);
				}else{
					return TCC::round(sim / normalization_context_negative);
				}
			}
			else
			{
				return TCC::round(sim);
			}
		}
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
		inline void init_normalization_context(TCC::TransClustParams& tcp)
		{
			if(tcp.normalization == "RELATIVE"){
				double max_val = TCC::round(std::abs(m.getMaxValue()-threshold));
				if(max_val != 0){
					normalization_context_positive = max_val;	
				}else{
					normalization_context_positive = 1;	
				}
				double min_val = TCC::round(std::abs(m.getMinValue()-threshold));
				if(min_val != 0){
					normalization_context_negative = min_val;	
				}else{
					normalization_context_negative = 1;	
				}
			}else if(tcp.normalization == "ABSOLUTE"){
				double max_val = std::max(
						TCC::round(std::abs(m.getMaxValue()-threshold)),
						TCC::round(std::abs(m.getMinValue()-threshold))
						);
				normalization_context_positive = max_val;
				normalization_context_negative = max_val;
			}
		
		}
		unsigned id;
		TriangularMatrix m;
		double threshold;
		double normalization_context_positive;
		double normalization_context_negative;
		double cost;

		inline static unsigned getNewId()
		{
			_cc_id++;
			return _cc_id;
		}
};
#endif
