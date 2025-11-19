#pragma once

#include<vector>
#include "indexer/common/Position.h"
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

            void write(const std::vector<std::pair<std::string, std::vector<igrep::indexer::common::Position>>>& sorted_positions);
            std::vector<igrep::indexer::common::Position> find_word(const std::string& word) const;

            void save_metadata() const;
            void load_metadata();
            std::vector<std::vector<SSTableMeta>>& get_metadata() {return _levels;};

        private:
            const std::filesystem::path _working_dir;
            std::vector<std::vector<SSTableMeta>> _levels;

            void write_varint(std::ofstream& ofs, uint32_t value) const;
		    uint32_t read_varint(std::ifstream& ifs) const;

            

            

    };
} // namespace igrep::indexer::lsm_tree
