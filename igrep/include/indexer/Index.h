#pragma once

#include"Position.h"
#include<string>
#include<unordered_map>

namespace include::indexer {
	
	class Index {
	public:
		void serialize(const std::string& path) const;
		void deserialize(const std::string& path);

		void process_line(const std::string& line, const std::string& filename, int line_number);
		const std::vector<Position>& get_positions(const std::string& word) const;


	private:
		std::unordered_map<std::string, std::vector<Position>> words;
	};

}