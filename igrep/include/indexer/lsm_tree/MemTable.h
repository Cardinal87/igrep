#pragma once

#include "indexer/common/Position.h"
#include<unordered_map>
#include<string>
#include<cstdint>
#include<vector>
#include<filesystem>

namespace igrep::indexer::lsm_tree{
    class MemTable{
        public:
            MemTable() = default;
            ~MemTable() = default;

            void insert(const std::string& word, const igrep::indexer::common::Position& position);

            bool is_full() const;
            bool is_empty() const;
            std::vector<std::pair<std::string, std::vector<igrep::indexer::common::Position>>> flush_to_sstable();


        private:
            static inline const uint16_t MAX_COUNT = 10000;
            uint16_t current_count = 0;

            std::unordered_map<std::string, std::vector<igrep::indexer::common::Position>> words;

    };
}