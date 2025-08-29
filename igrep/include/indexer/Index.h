#pragma once

#include"Position.h"
#include<string>
#include<unordered_map>
#include<vector>
#include<filesystem>

namespace igrep::indexer {
	
	class Index {
	public:
		Index() = default;

		void serialize(const std::string& path) const;
		void deserialize(const std::string& path);

		void process_line(const std::string& line, const std::string& filename, const int line_number, int& word_index);
		void remove_file(const std::filesystem::path& filepath);
		const std::vector<Position>& get_positions(const std::string& word) const;

		bool operator==(const Index& other) const{
			return words == other.words;
		}


	private:
		std::unordered_map<std::string, std::vector<Position>> words;

		
	};

}