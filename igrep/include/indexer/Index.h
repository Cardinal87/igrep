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

		void process_line(const std::string& line, const std::filesystem::path& filepath, const uint32_t& line_number, uint32_t& word_index);
		bool remove_file(const std::filesystem::path& filepath);
		bool is_file_indexed(const std::filesystem::path& filepath) const;

		const std::vector<Position>& get_positions(const std::string& word) const;
		std::filesystem::path get_path_by_id(const uint32_t file_id) const;

		bool operator==(const Index& other) const{
			return words == other.words &&
				   file_to_id == other.file_to_id;
		}


	private:
		std::unordered_map<std::string, std::vector<Position>> words;
		std::unordered_map<uint32_t, std::filesystem::path> id_to_file;
		std::unordered_map<std::filesystem::path, uint32_t> file_to_id;
		

		void write_varint(std::ofstream& ofs, uint32_t value) const;
		uint32_t read_varint(std::ifstream& ifs) const;
		
		
	};

}