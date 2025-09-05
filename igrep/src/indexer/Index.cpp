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
		ofs.write(reinterpret_cast<const char*>(&files_size), sizeof(files_size));
		for(const auto& [id, filepath] : id_to_file){
			ofs.write(reinterpret_cast<const char*>(&id), sizeof(id));

			string string_path = filepath.string();
			uint32_t path_size = string_path.length();
			ofs.write(reinterpret_cast<const char*>(&path_size), sizeof(path_size));
			ofs.write(string_path.data(), path_size);
		}

		uint32_t map_size = words.size();
		ofs.write(reinterpret_cast<char*>(&map_size), sizeof(map_size));

		for (const auto& pair : words) {

			uint32_t key_length = pair.first.length();
			ofs.write(reinterpret_cast<const char*>(&key_length), sizeof(key_length));
			ofs.write(pair.first.data(), key_length);

			uint32_t vector_size = pair.second.size();
			ofs.write(reinterpret_cast<const char*>(&vector_size), sizeof(vector_size));

			for (const Position& position : pair.second) {
				ofs.write(reinterpret_cast<const char*>(&position.file_id), sizeof(position.file_id));
				ofs.write(reinterpret_cast<const char*>(&position.word_index), sizeof(position.word_index));
				ofs.write(reinterpret_cast<const char*>(&position.indent), sizeof(position.indent));
				ofs.write(reinterpret_cast<const char*>(&position.line_number), sizeof(position.line_number));
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
			uint32_t files_size;
			ifs.read(reinterpret_cast<char*>(&files_size), sizeof(files_size));

			for (int i = 0; i < files_size; i++){
				uint32_t file_id;
				ifs.read(reinterpret_cast<char*>(&file_id), sizeof(file_id));

				uint32_t path_size;
				ifs.read(reinterpret_cast<char*>(&path_size), sizeof(path_size));

				string file_string;
				file_string.resize(path_size);
				ifs.read(file_string.data(), path_size);
				
				filesystem::path filepath = file_string;
				file_to_id[filepath] = file_id;
				id_to_file[file_id] = filepath;
			}



			uint32_t map_size;
			ifs.read(reinterpret_cast<char*>(&map_size), sizeof(map_size));

			for (int i = 0; i < map_size; i++) {
				uint32_t key_len;
				ifs.read(reinterpret_cast<char*>(&key_len), sizeof(key_len));

				string key;
				key.resize(key_len);
				ifs.read(key.data(), key_len);

				uint32_t vector_size;
				ifs.read(reinterpret_cast<char*>(&vector_size), sizeof(vector_size));

				vector<Position> positions;
				positions.reserve(vector_size);

				for (int j = 0; j < vector_size; j++) {

					uint32_t file_id;
					uint32_t indent;
					uint32_t line_number;
					uint32_t word_index;
					
					ifs.read(reinterpret_cast<char*>(&file_id), sizeof(line_number));
					ifs.read(reinterpret_cast<char*>(&word_index), sizeof(word_index));
					ifs.read(reinterpret_cast<char*>(&indent), sizeof(indent));
					ifs.read(reinterpret_cast<char*>(&line_number), sizeof(line_number));
					

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


	

}