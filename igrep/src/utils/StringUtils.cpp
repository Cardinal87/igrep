#include "utils/StringUtils.h"
#include<unordered_set>


using namespace std;

namespace igrep::utils{

    string StringUtils::normalize_line(string line){
        static const unordered_set<char> punctuation = { ',', '.', '\'', '\"', '\\', ':', '/', ';','[','{', '}', ']', '@', '#', '$', '!', '?', '-', '+', '=', '&', '^', '%', '~', '_'};

		for (auto it = line.begin(); it != line.end(); it++) {
			if (punctuation.count(*it) == 1) {
				*it = ' ';
			}
		}

		return remove_extra_spaces(line);
    }


    
	string StringUtils::remove_extra_spaces(const string& line ){
        string clean_line = "";
		bool is_prev_space = false;
		for (auto it = line.begin(); it != line.end(); it++) {
			if (*it == ' ' && is_prev_space) {
				continue;
			}

			clean_line += *it;
			is_prev_space = *it == ' ';
		}

		return clean_line;
    }	
	
}