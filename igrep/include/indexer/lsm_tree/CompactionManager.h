#pragma once

#include "indexer/lsm_tree/SSTableManager.h"
#include<filesystem>
#include<unordered_map>
#include <functional>

namespace igrep::indexer::lsm_tree
{
    class CompactionManager{
        public:
            explicit CompactionManager(std::filesystem::path working_dir);
            ~CompactionManager() = default;

            void compact(std::vector<std::vector<SSTableMeta>>& tables_meta, 
                         const std::unordered_map<uint32_t, std::filesystem::path>& files) const;


        private:
            const std::filesystem::path _working_dir;

            int8_t pick_level_for_compact(std::vector<std::vector<SSTableMeta>>& levels) const;
            std::vector<SSTableMeta> pick_candidates(uint8_t level, const std::vector<std::vector<SSTableMeta>>& levels) const;

            std::vector<SSTableMeta> merge(const std::vector<SSTableMeta>& candidates, uint8_t write_level,
                                           const std::function<bool(uint32_t)>& exist_condition) const;

            bool read_next_word(std::ifstream& idx, std::ifstream& table, std::string& word, std::vector<common::Position>& positions, 
                                const std::function<bool(uint32_t)>& exist_condition) const;
            void remove_old_tables(const std::vector<SSTableMeta>& candidates, std::vector<std::vector<SSTableMeta>>& levels, uint8_t level) const;

            const uint8_t MAX_L0_COUNT = 6; 
            const uint8_t FANOUT = 10;
            const uint16_t MAX_CHUNK_COUNT = 10000;
    };
} 
