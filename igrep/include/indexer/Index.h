#pragma once

#include"Position.h"
#include<string>
#include<unordered_map>

namespace include::indexer {
	
	class Index {
	public:
		void serialize() const;
		void deserialize();

		void process_line(std::string line, std::string filename, int line_number);
		std::vector<Position> get_positions(std::string word) const;


	private:
		std::unordered_map<std::string, Position> word;
		
	};


}