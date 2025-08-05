#pragma once

#include<string>

namespace include::utils{
    class StringUtils{

        public:
            static std::string normalize_line(std::string line);


        private:
            static std::string remove_extra_spaces(const std::string& line);
    };

    
}