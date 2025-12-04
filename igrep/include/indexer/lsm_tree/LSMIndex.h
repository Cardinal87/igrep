#pragma once

#include<vector>
#include "indexer/common/IndexBase.h"
#include "indexer/common/Position.h"
#include "MemTable.h"
#include "SSTableManager.h"
#include "CompactionManager.h"
#include<string>
#include<filesystem>
#include<unordered_map>
#include <cstdint>


namespace igrep::indexer::lsm_tree{
    class LSMIndex : public igrep::indexer::common::IndexBase{
        public:
            ~LSMIndex() = default;
            explicit LSMIndex(std::filesystem::path working_dir);

            void process_file(const std::filesystem::path& filepath);
            void process_directory();
            void remove_file(const std::string& filepath);

            std::vector<igrep::indexer::common::Position> get_positions(std::string& query) const;

            bool is_file_indexed(const std::filesystem::path& filepath) const;
            std::filesystem::path get_path_by_id(const uint32_t& file_id) const;

            void flush_memtable();
            void compact();

            void serialize() const;
            void deserialize();

            bool operator==(const LSMIndex& other) const;

        private:
            void index_line(std::string& line, const uint32_t& file_id, const uint32_t& line_number, uint32_t& word_index);
            void index_file(const std::filesystem::path& filename);

            static inline const std::string type = "lsm-tree";
            static inline const std::unordered_set<std::string> _extensions = {".txt", ".log", ".json", ".csv", ".ini"};
            const std::filesystem::path _working_dir;

            std::unordered_map<uint32_t, std::filesystem::path> id_to_file;
		    std::unordered_map<std::filesystem::path, uint32_t> file_to_id;

            MemTable _mem_table;
            SSTableManager _table_manager;
            CompactionManager _compaction_manager;


    };
}