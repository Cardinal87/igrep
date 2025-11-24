#pragma once

#include "indexer/lsm_tree/SSTableManager.h"
#include<filesystem>

namespace igrep::indexer::lsm_tree
{
    class CompactionManager{
        public:
            explicit CompactionManager(std::filesystem::path working_dir);
            ~CompactionManager() = default;

            void compact(std::vector<std::vector<SSTableMeta>>& tables_meta) const;


        private:
            const std::filesystem::path _working_dir;

            int8_t pick_level_for_compact(std::vector<std::vector<SSTableMeta>>& levels) const;
            std::vector<SSTableMeta> pick_candidates(uint8_t level, const std::vector<std::vector<SSTableMeta>>& levels) const;

            void merge(const std::vector<SSTableMeta>& candidates, uint8_t write_level) const;

            bool read_next_word(std::ifstream& idx, std::ifstream& table, std::string& word, std::vector<common::Position>& positions) const;

            const uint8_t MAX_L0_COUNT = 6; 
            const uint8_t FANOUT = 10;
            const uint16_t MAX_CHUNK_COUNT = 10000;
    };
} 
