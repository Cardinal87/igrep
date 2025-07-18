#include "../../include/indexer/Index.h"
#include<sstream>
#include<string>
#include<utility>
#include<fstream>
#include<ios>

using namespace std;

namespace include::indexer {


	void Index::process_line(const string& line, const string& filename, int line_number) {
		istringstream iss(line);
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

}