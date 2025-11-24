#pragma once

#include<cstdint>
#include<filesystem>
#include<vector>
#include "indexer/common/Position.h"
#include "SSTableMeta.h"

namespace igrep::indexer::lsm_tree{

    class SSTableIO{

        public:
            static SSTableMeta write_table(const std::vector<std::pair<std::string, std::vector<igrep::indexer::common::Position>>>& sorted_positions, const std::filesystem::path& working_dir, uint8_t level);

            static void write_varint(std::ofstream& ofs, uint64_t value);
		    static uint64_t read_varint(std::ifstream& ifs);

        private:
            SSTableIO() = delete;
    };
}