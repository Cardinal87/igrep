#pragma once

#include"Position.h"
#include<string>
#include<unordered_map>
#include<vector>

namespace include::indexer {
	
	class Index {
	public:
		void serialize(const std::string& path) const;
		void deserialize(const std::string& path);

		void process_line(const std::string& line, const std::string& filename, int line_number);
		const std::vector<Position>& get_positions(const std::string& word) const;


	private:
		std::unordered_map<std::string, std::vector<Position>> words;

		std::string normalize_line(std::string line) const;
		std::string remove_extra_spaces(const std::string& line) const;
	};

}