#pragma once

#include<string>

namespace igrep::utils{
    class StringUtils{

        public:
            static std::string normalize_line(std::string line);
            static std::string to_lower_case_copy(const std::string& line);

        private:
            static std::string remove_extra_spaces(const std::string& line);
    };

    
}