#pragma once

#include "indexer/lsm_tree/SSTableManager.h"
#include<filesystem>

namespace igrep::indexer::lsm_tree
{
    class ComapactionManager{
        public:
            explicit ComapactionManager(std::filesystem::path working_dir);
            ~ComapactionManager() = default;

            void compact(std::vector<std::vector<SSTableMeta>>& tables_meta) const;


        private:
            const std::filesystem::path _working_dir;

            int8_t pick_level_for_compact(std::vector<std::vector<SSTableMeta>>& levels) const;
            std::vector<SSTableMeta> pick_candidates(uint8_t level, const std::vector<std::vector<SSTableMeta>>& levels) const;

            void merge(std::vector<SSTableMeta> candidates) const;

            const uint8_t MAX_L0_SIZE = 6; 
            const uint8_t FANOUT = 10;
            const uint32_t TABLE_SIZE = 64 * 1024 * 1024;
    };
} 
