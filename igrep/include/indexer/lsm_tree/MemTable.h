#pragma once

#include "common/Position.h"
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
            std::vector<std::pair<std::string, std::vector<igrep::indexer::common::Position>>> flush_to_sstable();


        private:
            static inline const std::uint32_t maxSize = 50 * 1024 * 1024;

            std::unordered_map<std::string, std::vector<igrep::indexer::common::Position>> id_to_positions;

            void clear();
            

    };
}