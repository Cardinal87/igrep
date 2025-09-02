#include "indexer/Index.h"
#include "utils/StringUtils.h"
#include<sstream>
#include<string>
#include<utility>
#include<fstream>
#include<filesystem>
#include<ios>
#include<unordered_set>


using namespace std;
using namespace std::filesystem;
using namespace igrep::utils;


namespace igrep::indexer {

	
	void Index::process_line(const string& line, const string& filename, const size_t& line_number, size_t& word_index) {
		path absolute_path = path(absolute(filename));
		absolute_path = canonical(absolute_path);
		indexed_files.insert(absolute_path);
		
		string lower_case_line = StringUtils::to_lower_case_copy(line);
		string normalized =  StringUtils::normalize_line(line);
		istringstream iss(normalized);
		string word;
		size_t indent;
		size_t start_pos = 0;
		while (iss >> word) {
			indent = lower_case_line.find(word, start_pos);
			start_pos = indent + 1;
			words[move(word)].emplace_back(absolute_path, line_number, indent, word_index++);
		}
		
	}

	bool Index::remove_file(const path& filepath){
		path absolute_path = path(absolute(filepath));
		absolute_path = canonical(absolute_path);

		if (!indexed_files.contains(absolute_path)){
			return false;
		}
		
		for(auto it = words.begin(); it != words.end(); ){
			
			for(int i = 0; i < (it->second).size(); i++){
				if ((it->second)[i].filename == absolute_path){
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
		indexed_files.erase(absolute_path);
		return true;
	}

	bool Index::is_file_indexed(const path& filepath) const{
		return indexed_files.find(absolute(filepath)) != indexed_files.end();
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

	void Index::serialize(const string& path) const {
		ofstream ofs(path, ios::binary);

		if (!ofs.is_open()) {
			throw runtime_error("Cannot open file " + path);
		}
		size_t set_size = indexed_files.size();
		ofs.write(reinterpret_cast<const char*>(&set_size), sizeof(set_size));
		for(const auto& el : indexed_files){
			string string_path = el.string();
			size_t path_size = string_path.length();
			ofs.write(reinterpret_cast<const char*>(&path_size), sizeof(path_size));
			ofs.write(string_path.data(), path_size);
		}

		size_t map_size = words.size();
		ofs.write(reinterpret_cast<char*>(&map_size), sizeof(map_size));

		for (const auto& pair : words) {

			size_t key_length = pair.first.length();
			ofs.write(reinterpret_cast<const char*>(&key_length), sizeof(key_length));
			ofs.write(pair.first.data(), key_length);

			size_t vector_size = pair.second.size();
			ofs.write(reinterpret_cast<const char*>(&vector_size), sizeof(vector_size));

			for (const Position& position : pair.second) {

				size_t filename_length = position.filename.length();
				ofs.write(reinterpret_cast<const char*>(&filename_length), sizeof(filename_length));
				ofs.write(position.filename.data(), filename_length);

				ofs.write(reinterpret_cast<const char*>(&position.word_index), sizeof(position.word_index));
				ofs.write(reinterpret_cast<const char*>(&position.indent), sizeof(position.indent));
				ofs.write(reinterpret_cast<const char*>(&position.line_number), sizeof(position.line_number));
			}
		}
		ofs.close();
	}

	void Index::deserialize(const string& path) {
		try {
			ifstream ifs(path, ios::binary);
			if (!ifs.is_open()) {
				throw runtime_error("Cannot open index file" + path);
			}
			size_t set_size;
			ifs.read(reinterpret_cast<char*>(&set_size), sizeof(set_size));

			for (int i = 0; i < set_size; i++){
				size_t path_size;
				ifs.read(reinterpret_cast<char*>(&path_size), sizeof(path_size));
				string file_string;
				file_string.resize(path_size);
				ifs.read(file_string.data(), path_size);
				
				filesystem::path filepath = file_string;
				indexed_files.insert(filepath);
			}



			size_t map_size;
			ifs.read(reinterpret_cast<char*>(&map_size), sizeof(map_size));

			for (int i = 0; i < map_size; i++) {
				size_t key_len;
				ifs.read(reinterpret_cast<char*>(&key_len), sizeof(key_len));

				string key;
				key.resize(key_len);
				ifs.read(key.data(), key_len);

				size_t vector_size;
				ifs.read(reinterpret_cast<char*>(&vector_size), sizeof(vector_size));

				vector<Position> positions;
				positions.reserve(vector_size);

				for (int j = 0; j < vector_size; j++) {
					size_t filename_len;
					ifs.read(reinterpret_cast<char*>(&filename_len), sizeof(filename_len));

					string filename;
					filename.resize(filename_len);
					ifs.read(filename.data(), filename_len);

					size_t indent;
					size_t line_number;
					size_t word_index;

					ifs.read(reinterpret_cast<char*>(&word_index), sizeof(word_index));
					ifs.read(reinterpret_cast<char*>(&indent), sizeof(indent));
					ifs.read(reinterpret_cast<char*>(&line_number), sizeof(line_number));

					positions.emplace_back(move(filename), line_number, indent, word_index);

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