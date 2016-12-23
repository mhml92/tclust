#ifndef CONNECTEDCOMPONENT_HPP
#define CONNECTEDCOMPONENT_HPP
#include <string>
#include <deque>
#include <vector>
#include <cmath>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <plog/Log.h>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/algorithm/clamp.hpp>
#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include "transclust/Common.hpp"

class ConnectedComponent
{
	public:
		struct Cost {
			uint64_t id;
			float cost;
			Cost(){}
			Cost(uint64_t _id, float _cost)
				:id(_id), cost(_cost)
			{ }
			
		};
		ConnectedComponent();
		ConnectedComponent(unsigned id,long _num_positive_edges ,TCC::TransClustParams& _tcp);

		inline void addNode(unsigned id)
		{
			globalId2localId.insert(std::make_pair(id,localId2globalId.size()));
			localId2globalId.push_back(id);
		}

		inline unsigned getId(){ return cc_id; }
		inline unsigned size(){ return localId2globalId.size(); }
		//inline long getNumEdges(){return num_positive_edges;}
		inline bool isTransitive(){ return transitive; }
		//inline float getMaxCost(){ return max_cost; }
		inline std::deque<unsigned> getIndex2ObjectId(){ return localId2globalId; }
		//inline long getNumConflictingEdges(){ return num_conflicting_edges; }
		inline unsigned getObjectId(unsigned i){ return localId2globalId.at(i); }

		void getBufferedCost(
				std::vector<float>& buffer,
				unsigned i,
				unsigned j,
				bool normalized = true);

		float getCost(unsigned i,unsigned j,bool normalized = true);
		void addCost(uint64_t id, float cost);
		bool getFitInMemory();


		// the connected component has now been given all known information
		// and now a decision can be made as to how this
		// information can be stored most efficiantly 
		void commit();

		void load(TCC::TransClustParams& _tcp);

		inline void free(bool permanent = false)
		{
			if(mm_file.is_open())
			{
				mm_file.close();
			}

			flat_map.clear();
			flat_map_index.clear();

			if(permanent)
			{
				localId2globalId.clear();
				is_external_file_configured = false;

				if(boost::filesystem::exists(matrix_file_path)){
					boost::filesystem::remove(matrix_file_path);
				}
				if(boost::filesystem::exists(flat_file_path)){
					boost::filesystem::remove(flat_file_path);
				}
			}
			is_mmf_configured = false;
		}

	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar,const unsigned int version)
		{
			ar & cc_id;
			ar & localId2globalId;
			ar & max_value;
			ar & min_value;
			ar & cff;
			ar & num_cost_values;
			ar & flat_file_path;
			ar & normalization_factor;
			ar & transitive;
		}
		void initMatrixFile();
		void initFlatFile();
		void flushCostBuffer();

		void getInMemoryBufferedCostMatrix(
				std::vector<float>& buffer,
				unsigned i,
				unsigned j);

		void getInMemoryBufferedCostFlat(
				std::vector<float>& buffer,
				unsigned i,
				unsigned j);

		void getExternalBufferedCostFlat(
				std::vector<float>& buffer,
				unsigned i,
				unsigned j);

		void getExternalBufferedCostMatrix(
				std::vector<float>& buffer,
				unsigned i,
				unsigned j);
		void loadMatrixBuffer();

		/**
		 * Contructs a filename and creates path if neccesary
		 */
		boost::filesystem::path getMatrixFilePath();
		boost::filesystem::path getFlatFilePath();

		unsigned long index(unsigned i,unsigned j); 

		unsigned cc_id;
		TCC::TransClustParams tcp;
		boost::unordered_map<unsigned,unsigned> globalId2localId;
		std::deque<unsigned> localId2globalId;
		bool transitive = false;
		long num_positive_edges;
		float max_value = std::numeric_limits<float>::lowest();
		float min_value = std::numeric_limits<float>::max();
		//float max_cost = 0;
		//long num_conflicting_edges = 0;
		long num_cost_values = 0;
		enum CostFileFormat {MATRIX,FLAT};
		CostFileFormat cff;

		std::string matrix_file_path;
		std::string flat_file_path;

		std::vector<Cost> cost_buffer;

		boost::unordered_map<unsigned long,float> flat_map;
		std::map<unsigned long,unsigned> flat_map_index;
		std::deque<float> matrix_buffer;

		boost::iostreams::mapped_file_source mm_file;
		bool is_mmf_configured = false;
		bool is_external_file_configured = false;
		bool fit_in_memory = false;
		float normalization_factor;
};
#endif
