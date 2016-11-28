#ifndef INPUT_PARSER_HPP
#define INPUT_PARSER_HPP
#include <deque>
#include <algorithm>
#include <stxxl/sorter>
#include <stxxl/stats>
#include <boost/unordered_map.hpp>
#include "transclust/ConnectedComponent.hpp"
#include "transclust/Common.hpp"
#include "transclust/DynamicUnionFind.hpp"
#include "transclust/ClusteringResult.hpp"

class InputParser
{
	public:
		InputParser(
				TCC::TransClustParams& _tcp
				);

		void getConnectedComponents(
				std::deque<ConnectedComponent>& ccs,
				std::deque<std::string>& id2name
				);

	private:
		struct similarity
		{
			uint64_t id;
			float sim;
			similarity()
			{ }
			similarity(uint64_t _id, float _sim)
				: id(_id), sim(_sim)
			{ }
		};
		struct similarity_comparator
		{
			bool operator () (const similarity& a, const similarity& b) const
			{
				if(a.id == b.id){
					return (a.sim < b.sim);
				}else{
					return (a.id < b.id);
				}
			}
			similarity min_value() const
			{
				return similarity(std::numeric_limits<uint64_t>::lowest(),std::numeric_limits<float>::lowest());
			}
			similarity max_value() const
			{
				return similarity(std::numeric_limits<uint64_t>::max(),std::numeric_limits<float>::max());
			}
		};

		typedef stxxl::sorter<similarity,similarity_comparator> external_sorter;

		void buildObjectIdMaps(
				std::string o,
				boost::unordered_map<std::string, unsigned>& name2id,
				std::deque<std::string>& id2name
				)
		{
			if (name2id.find(o) == name2id.end())
			{
				unsigned _id = id2name.size();
				id2name.push_back(o);
				name2id[o] = _id;
			}
		}

		TCC::TransClustParams tcp;
		std::string filename;
		float maxValue = std::numeric_limits<float>::lowest();
		float minValue = std::numeric_limits<float>::max();
		DynamicUnionFind duf;

		boost::unordered_map<unsigned,unsigned> rootId2cc;
		// LOG BOOLS
		bool has_inf_in_input = false;
};
#endif
