#pragma once

#include<string>
#include<cstdint>

namespace igrep::indexer::lsm_tree
{
    struct SSTableMeta{
        std::uint32_t sstable_id;
        std::string first_word;
        std::string last_word;

        bool operator==(const SSTableMeta& other) const{
            return sstable_id == other.sstable_id && 
                   first_word == other.first_word && 
                   last_word == other.last_word;
        }
    };
} // namespace igrep::indexer::lsm_tree
