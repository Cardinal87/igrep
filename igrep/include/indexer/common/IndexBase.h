#pragma once

#include "Position.h"
#include<string>
#include<cstdint>
#include<vector>
#include<unordered_set>
#include<filesystem>

namespace igrep::indexer::common{
    class IndexBase{
        public:
            virtual ~IndexBase() = default;

            virtual void process_file(const std::filesystem::path& filepath) = 0;
            virtual void process_directory(const std::filesystem::path& dirpath) = 0;
            virtual std::vector<Position> get_positions(std::string& query) const = 0;
            virtual void remove_file(const std::filesystem::path& filename) = 0;
            virtual bool is_file_indexed(const std::filesystem::path& filepath) const = 0;

        protected:
            static inline const std::unordered_set<std::string> extensions = {".txt", ".log", ".json", ".csv", ".ini"};
    };
}