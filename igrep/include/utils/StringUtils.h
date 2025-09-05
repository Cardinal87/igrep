#pragma once

#include<string>
#include<cstdint>
#include<filesystem>

namespace igrep::utils{
    class StringUtils{

        public:
            static std::string normalize_line(std::string line);
            static std::string to_lower_case_copy(const std::string& line);
            static std::uint32_t get_file_hash(const std::filesystem::path& filepath);

        private:
            static std::string remove_extra_spaces(const std::string& line);
    };

    
}