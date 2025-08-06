#include "../../include/indexer/Index.h"
#include "../../include/utils/StringUtils.h"
#include<sstream>
#include<string>
#include<utility>
#include<fstream>
#include<ios>
#include<unordered_set>


using namespace std;
using namespace igrep::utils;


namespace igrep::indexer {

	
	void Index::process_line(const string& line, const string& filename, int line_number) {
		string normalized =  StringUtils::normalize_line(line);
		istringstream iss(normalized);
		string word;
		size_t indent;
		size_t start_pos = 0;
		while (iss >> word) {
			indent = line.find(word, start_pos);
			start_pos += word.length();
			words[move(word)].emplace_back(filename, line_number, indent);
		}
		
	}

	const vector<Position>& Index::get_positions(const string& word) const {
		auto it = words.find(word);
		if (it != words.end()) {
			return it->second;
		}


		static const vector<Position> empty;
		return empty;
	}

	void Index::serialize(const string& path) const {
		ofstream ofs(path, ios::binary);

		if (!ofs.is_open()) {
			throw runtime_error("Cannot open file" + path);
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

					int indent;
					int line_number;

					ifs.read(reinterpret_cast<char*>(&indent), sizeof(indent));
					ifs.read(reinterpret_cast<char*>(&line_number), sizeof(line_number));

					positions.emplace_back(move(filename), indent, line_number);

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