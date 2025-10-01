#pragma once

#include<vector>
#include "common/Position.h"
#include "SSTableMeta.h"
#include<string>
#include<filesystem> 
#include<cstdint>
#include<iostream>   
#include <fstream>

namespace igrep::indexer::lsm_tree
{
    class SSTableManager{
        public:
            ~SSTableManager() = default;
            explicit SSTableManager(std::filesystem::path working_dir);

            void write(const std::vector<std::pair<std::string, std::vector<igrep::indexer::common::Position>>>& positions);
            std::vector<igrep::indexer::common::Position> find_word(const std::string& word) const;

            std::vector<igrep::indexer::lsm_tree::SSTableMeta> get_metadata() const;

        private:
            const std::filesystem::path _working_dir;
            std::vector<igrep::indexer::lsm_tree::SSTableMeta> _sstables_metadata;

            void write_varint(std::ofstream& ofs, uint32_t value) const;
		    uint32_t read_varint(std::ifstream& ifs) const;

            void load_metadata();

    };
} // namespace igrep::indexer::lsm_tree
