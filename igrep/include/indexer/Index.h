#pragma once

#include"Position.h"
#include<string>
#include<unordered_map>
#include<vector>

namespace igrep::indexer {
	
	class Index {
	public:
		Index() = default;

		void serialize(const std::string& path) const;
		void deserialize(const std::string& path);

		void process_line(const std::string& line, const std::string& filename, const int line_number, int& word_index);
		const std::vector<Position>& get_positions(const std::string& word) const;


	private:
		std::unordered_map<std::string, std::vector<Position>> words;

		
	};

}