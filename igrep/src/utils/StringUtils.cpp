#include "utils/StringUtils.h"
#include<unordered_set>


using namespace std;

namespace igrep::utils{

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