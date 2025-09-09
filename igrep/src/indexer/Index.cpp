#include "indexer/Index.h"
#include "utils/StringUtils.h"
#include<sstream>
#include<string>
#include<utility>
#include<fstream>
#include<filesystem>
#include<ios>
#include<unordered_set>
#include<cstdint>

using namespace std;
using namespace std::filesystem;
using namespace igrep::utils;


namespace igrep::indexer {


	path Index::get_path_by_id(const uint32_t file_id) const{
		path filepath = id_to_file.at(file_id);
		return filepath;
	}

	void Index::process_line(const string& line, const path& filename, const uint32_t& line_number, uint32_t& word_index) {
		path absolute_path = weakly_canonical(absolute(path(filename)));
		
		uint32_t file_id;
		if (file_to_id.contains(filename)){
			file_id = file_to_id[absolute_path];
		}
		else{
			file_id = StringUtils::get_file_hash(filename);
			file_to_id[absolute_path] = file_id;
			id_to_file[file_id] = absolute_path;
		}


		string lower_case_line = StringUtils::to_lower_case_copy(line);
		string normalized =  StringUtils::normalize_line(line);
		istringstream iss(normalized);
		string word;
		uint32_t indent;
		uint32_t start_pos = 0;
		while (iss >> word) {
			indent = lower_case_line.find(word, start_pos);
			start_pos = indent + 1;
			words[move(word)].emplace_back(file_id, line_number, indent, word_index++);
		}
		
	}

	bool Index::remove_file(const path& filepath){
		path absolute_path = weakly_canonical(absolute(path(filepath)));

		if (!file_to_id.contains(absolute_path)){
			return false;
		}
		
		uint32_t file_id = file_to_id[absolute_path];

		for(auto it = words.begin(); it != words.end(); ){
			
			for(int i = 0; i < (it->second).size(); i++){
				if ((it->second)[i].file_id == file_id){
					(it->second).erase((it->second).begin() + i);
					i--;
				}
			}
			if ((it->second).empty()){
				it = words.erase(it);
			} 
			else {
				it++;
			}
		}
		file_to_id.erase(absolute_path);
		id_to_file.erase(file_id);

		return true;
	}

	bool Index::is_file_indexed(const path& filepath) const{
		path absolute_path = weakly_canonical(absolute(path(filepath)));
		return file_to_id.contains(absolute_path);
	}

	const vector<Position>& Index::get_positions(const string& word) const {
		string lower_case_word = StringUtils::to_lower_case_copy(word);
		auto it = words.find(lower_case_word);
		if (it != words.end()) {
			return it->second;
		}


		static const vector<Position> empty;
		return empty;
	}

	void Index::serialize(const string& filepath) const {
		path absolute_path = absolute(path(filepath));
		
		ofstream ofs(absolute_path, ios::binary);

		if (!ofs.is_open()) {
			throw runtime_error("Cannot open file " + absolute_path.string());
		}
		uint32_t files_size = id_to_file.size();
		write_varint(ofs, files_size);
		for(const auto& [id, filepath] : id_to_file){
			write_varint(ofs, id);


			string string_path = filepath.string();
			uint32_t path_size = string_path.length();
			write_varint(ofs, path_size);
			ofs.write(string_path.data(), path_size);
		}

		uint32_t map_size = words.size();
		write_varint(ofs, map_size);

		for (const auto& pair : words) {

			uint32_t key_length = pair.first.length();
			write_varint(ofs, key_length);
			ofs.write(pair.first.data(), key_length);

			uint32_t vector_size = pair.second.size();
			write_varint(ofs, vector_size);

			for (const Position& position : pair.second) {
				write_varint(ofs, position.file_id);
				write_varint(ofs, position.indent);
				write_varint(ofs, position.line_number);
				write_varint(ofs, position.word_index);
			}
		}
		ofs.close();
	}

	void Index::deserialize(const string& filepath) {
		try {
			path absolute_path = absolute(path(filepath));

			ifstream ifs(absolute_path, ios::binary);
			if (!ifs.is_open()) {
				throw runtime_error("Cannot open index file" + absolute_path.string());
			}
			uint32_t files_size = read_varint(ifs);

			for (int i = 0; i < files_size; i++){
				uint32_t file_id = read_varint(ifs);

				uint32_t path_size = read_varint(ifs);

				string file_string;
				file_string.resize(path_size);
				ifs.read(file_string.data(), path_size);
				
				filesystem::path filepath = file_string;
				file_to_id[filepath] = file_id;
				id_to_file[file_id] = filepath;
			}



			uint32_t map_size = read_varint(ifs);

			for (int i = 0; i < map_size; i++) {
				uint32_t key_len = read_varint(ifs);

				string key;
				key.resize(key_len);
				ifs.read(key.data(), key_len);

				uint32_t vector_size = read_varint(ifs);

				vector<Position> positions;
				positions.reserve(vector_size);

				for (int j = 0; j < vector_size; j++) {

					uint32_t file_id = read_varint(ifs);
					uint32_t indent = read_varint(ifs);
					uint32_t line_number = read_varint(ifs);
					uint32_t word_index = read_varint(ifs);
					

					positions.emplace_back(file_id, line_number, indent, word_index);

				}
				words[move(key)] = move(positions);

				if (!ifs) {
					throw runtime_error("Error during deserializing index");
				}


			}
			ifs.close();
		}
		catch (const exception&) {
			words.clear();
			throw;
		}
	}

	void Index::write_varint(ofstream& ofs, uint32_t value) const{
		while(value >= 0x80){
			char ch = value | 0x80;
			ofs.write(&ch, 1);
			value >>= 7;	
		}
		ofs.write(reinterpret_cast<char*>(&value), 1);
	}
	
	uint32_t Index::read_varint(ifstream& ifs) const {
		unsigned char buffer;
		ifs.read(reinterpret_cast<char*>(&buffer), 1);
		uint32_t value = static_cast<uint32_t>(buffer & 0x7F);
		uint8_t shift = 7;

		while(buffer & 0x80 && shift < 32){
			ifs.read(reinterpret_cast<char*>(&buffer), 1);
			value |= (static_cast<uint32_t>(buffer & 0x7F) << shift);
			shift += 7;
		}

		return value;
	}

}