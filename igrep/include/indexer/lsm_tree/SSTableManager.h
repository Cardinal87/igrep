#pragma once

#include<vector>
#include "indexer/common/Position.h"
#include "SSTableMeta.h"
#include<string>
#include<filesystem> 
#include<cstdint>
#include<iostream>   
#include <fstream>
#include<unordered_map>

namespace igrep::indexer::lsm_tree
{
    class SSTableManager{
        public:
            ~SSTableManager() = default;
            explicit SSTableManager(std::filesystem::path working_dir);

            std::vector<igrep::indexer::common::Position> find_word(const std::string& word, const std::unordered_map<uint32_t, std::filesystem::path>& files) const;
            void write(const std::vector<std::pair<std::string, std::vector<common::Position>>>& sorted_positions);

            void save_metadata() const;
            void load_metadata();
            std::vector<std::vector<SSTableMeta>>& get_metadata() {return _levels;};

        private:
            const std::filesystem::path _working_dir;
            std::vector<std::vector<SSTableMeta>> _levels;

    };
} // namespace igrep::indexer::lsm_tree
