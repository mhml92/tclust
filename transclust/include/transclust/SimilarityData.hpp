//#ifndef SIMILARITY_DATA_HPP
//#define SIMILARITY_DATA_HPP
//#include <boost/unordered_map.hpp>
//#include "transclust/Common.hpp"
//class SimilarityData
//{
//	public:
//		SimilarityData(
//				const std::string& filename,
//				TCC::TransClustParams& _tcp
//				);
//
//
//		void init(std::deque<std::string>& id2name);
//		bool hasValue(std::pair<std::string,std::string>& key);
//		float getValue(std::pair<std::string,std::string>& key);
//
//		inline unsigned getId(std::string name){return name2id.at(name);}
//		inline boost::unordered_map<std::pair<std::string, std::string>, float>& getValues(){return values;}
//
//	private:
//		//void init_external();
//		void init_internal(std::deque<std::string>& id2name);
//		TCC::TransClustParams tcp;
//		std::string filename;
//
//		// name to id | id to name maps
//
//		float maxValue;
//		float minValue;
//
//		// LOG BOOLS
//		bool has_inf_in_input = false;
//
//		boost::unordered_map<std::string, unsigned> name2id;
//
//		// map for similarity value pair<<object1 name>-<object2 name>> -> <value>
//		boost::unordered_map<std::pair<std::string, std::string>, float> values;
//
//		void buildObjectIdMaps(std::string o,std::deque<std::string>& id2name){
//			if (name2id.find(o) == name2id.end())
//			{
//				id2name.push_back(o);
//				unsigned _id = id2name.size()-1;
//				name2id[o] = _id;
//			}
//		}
//};
//#endif
