#include "utils/StringUtils.h"
#include<unordered_set>
#include<filesystem>
#include<cstdint>
#include<functional>
#include<chrono>

using namespace std;
using namespace std::filesystem;
using namespace std::chrono;

namespace igrep::utils{

    uint32_t StringUtils::get_file_hash(const path& filepath){
		hash<path> path_hasher;
		size_t file_hash = path_hasher(filepath);

		auto now = chrono::system_clock::now();
		uint64_t timestamp = chrono::duration_cast<chrono::microseconds>(now.time_since_epoch()).count();
		hash<uint64_t> ts_hasher;
		size_t ts_hash = ts_hasher(timestamp);

		file_hash ^= ts_hash + 0x9e3779b9 + (file_hash << 6) + (file_hash >> 2);
		return static_cast<uint32_t>(file_hash ^ (file_hash >> 32));
	}
	
	string StringUtils::normalize_line(string line) {
        static const unordered_set<char> punctuation = { ',', '.', '\'', '\"', '\\', ':', '/', ';','[','{', '}', ']', '(',')' , '@', '#', '$', '!', '?', '-', '+', '=', '&', '^', '%', '~', '_'};

		for (auto it = line.begin(); it != line.end(); it++) {
			if (punctuation.count(*it) == 1) {
				*it = ' ';
			}
			else{
				*it = static_cast<char>(tolower(static_cast<unsigned char>(*it)));
			}
		}

		return remove_extra_spaces(line);
    }

	string StringUtils::to_lower_case_copy(const string& line){
		string lower_copy;
		for(auto& ch : line){
			lower_copy += static_cast<char>(tolower(static_cast<unsigned char>(ch)));
		}
		return lower_copy;
	}
    
	string StringUtils::remove_extra_spaces(const string& line ){
        string clean_line = "";
		bool is_prev_space = false;
		bool find_first_char = false;
		for (auto it = line.begin(); it != line.end(); it++) {
			if (*it == ' ' && (is_prev_space || !find_first_char)) {
				continue;
			}
			if (!find_first_char){
				find_first_char = true;
			}
			clean_line += *it;
			is_prev_space = *it == ' ';
		}
		if (is_prev_space){
			clean_line.pop_back();
		}

		return clean_line;
    }	
	
	
}