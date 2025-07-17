#include "../../include/indexer/Index.h"
#include<sstream>
#include<string>
#include<utility>


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


}