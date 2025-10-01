#pragma once

#include<vector>
#include "common/IndexBase.h"
#include "common/Position.h"
#include "MemTable.h"
#include "SSTableManager.h"
#include<string>
#include<filesystem>
#include<unordered_map>
#include <cstdint>


namespace igrep::indexer::lsm_tree{
    class LSMIndex : public igrep::indexer::common::IndexBase{
        public:
            ~LSMIndex();
            explicit LSMIndex(std::filesystem::path working_dir);

            void process_line(std::string& line, const std::string& filename, uint32_t& word_index);
            std::vector<igrep::indexer::common::Position> get_positions(std::string& query) const;
            void remove_file(const std::string& filename);

            bool is_file_indexed(const std::filesystem::path& filepath) const;
            std::filesystem::path get_path_by_id(const uint32_t& file_id) const;

            void flush_memtable();
            void compact();

            void serialize() const;

            bool operator==(const LSMIndex& other) const;

        private:
            static inline const std::string type = "lsm-tree";
            const std::filesystem::path _working_dir;

            std::unordered_map<uint32_t, std::filesystem::path> id_to_file;
		    std::unordered_map<std::filesystem::path, uint32_t> file_to_id;

            MemTable _mem_table;
            SSTableManager _table_manager;


            void deserialize();

    };
}