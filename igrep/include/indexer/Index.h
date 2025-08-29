#pragma once

#include"Position.h"
#include<string>
#include<unordered_map>
#include<vector>
#include<filesystem>
#include<unordered_set>

namespace igrep::indexer {
	
	class Index {
	public:
		Index() = default;

		void serialize(const std::string& path) const;
		void deserialize(const std::string& path);

		void process_line(const std::string& line, const std::string& filename, const size_t& line_number, size_t& word_index);
		void remove_file(const std::filesystem::path& filepath);
		const std::vector<Position>& get_positions(const std::string& word) const;

		bool operator==(const Index& other) const{
			return words == other.words &&
				   indexed_files == other.indexed_files;
		}


	private:
		std::unordered_map<std::string, std::vector<Position>> words;
		std::unordered_set<std::filesystem::path> indexed_files;
		
	};

}