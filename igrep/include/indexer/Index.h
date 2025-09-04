#pragma once

#include"Position.h"
#include<string>
#include<unordered_map>
#include<vector>
#include<filesystem>
#include<unordered_set>
#include<cstdint>

namespace igrep::indexer {
	
	class Index {
	public:
		static inline const std::unordered_set<std::string> extenstions = {".txt", ".log", ".json", ".csv", ".ini"};
		Index() = default;

		void serialize(const std::string& path) const;
		void deserialize(const std::string& path);

		void process_line(const std::string& line, const std::string& filename, const uint32_t& line_number, uint32_t& word_index);
		bool remove_file(const std::filesystem::path& filepath);
		bool is_file_indexed(const std::filesystem::path& filepath) const;
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