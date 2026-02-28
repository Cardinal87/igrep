#include "indexer/lsm_tree/CompactionManager.h"
#include "indexer/common/Position.h"
#include "indexer/lsm_tree/SSTableIO.h"
#include<filesystem>
#include<cstdint>
#include<vector>
#include<cmath>
#include<ranges>
#include<algorithm>
#include <queue>
#include<unordered_map>
#include <functional>

using namespace std;
using namespace std::filesystem;
using namespace igrep::indexer::lsm_tree;
using namespace igrep::indexer::common;

namespace igrep::indexer::lsm_tree{
    CompactionManager::CompactionManager(path working_dir): _working_dir(working_dir){}
    
    

    void CompactionManager::compact(vector<vector<SSTableMeta>>& tables_meta, const unordered_map<uint32_t, path>& files) const{

        int8_t level_for_compact = pick_level_for_compact(tables_meta);
        
        vector<SSTableMeta> candidates = pick_candidates(level_for_compact, tables_meta);

        uint8_t next_level = level_for_compact + 1;

        auto exist_condition = [&files](uint32_t file_id) -> bool {
            return files.contains(file_id);
        };
        vector<SSTableMeta> new_tables_meta = merge(candidates, level_for_compact + 1, exist_condition);
        remove_old_tables(candidates, tables_meta, level_for_compact);
        if (!new_tables_meta.empty()){
            if (next_level >= tables_meta.size()) tables_meta.resize(next_level + 1);
            tables_meta[next_level].insert(tables_meta[next_level].end(), new_tables_meta.begin(), new_tables_meta.end());
            sort(tables_meta[next_level].begin(), tables_meta[next_level].end(), [](const SSTableMeta& a, const SSTableMeta& b) {
                return a.first_word < b.first_word;
            });
        }

    }


    int8_t CompactionManager::pick_level_for_compact(vector<vector<SSTableMeta>>& levels) const {
        for (int8_t lv = 0; lv < levels.size(); lv++){
            int8_t level_size = levels[lv].size();

            if (level_size >= MAX_L0_COUNT * pow(FANOUT, lv)){
                return lv;
            }
        }
        return -1;
    }

    vector<SSTableMeta> CompactionManager::pick_candidates(uint8_t level, const vector<vector<SSTableMeta>>& levels) const {
        if (levels[level].empty()){
            return {};
        }
        
        vector<SSTableMeta> candidates;
        uint16_t check_until = 0;
        if (level == 0){
            candidates = levels[0];
            check_until = levels[0].size() - 1;
        }
        else {
            candidates = {levels[level][0]};
        }

        level++;
        if (levels.size() == level){
            return candidates;
        }

        for (const auto& meta: levels[level]){
            bool has_overlap = false;

            for(uint16_t index = 0; index <= check_until; index++){
                if (candidates[index].first_word <= meta.last_word && candidates[index].last_word >= meta.first_word){
                    has_overlap = true;
                    break;
                }
            }

            if(has_overlap){
                candidates.push_back(meta);
            }
        }

        return candidates;
    }



    vector<SSTableMeta> CompactionManager::merge(const vector<SSTableMeta>& candidates, uint8_t write_level, const function<bool(uint32_t)>& exist_condition) const{
        
        if (candidates.empty()){
            return {};
        }

        struct TableStream{
            ifstream idx;
            ifstream table;

            string current_word;
            vector<Position> current_positions;
        };
        vector<SSTableMeta> new_table_meta;


        auto comparator = [](const TableStream* a, const TableStream* b) {return a->current_word > b->current_word;};
        priority_queue<TableStream*, vector<TableStream*>, decltype(comparator)> pq(comparator); 

        vector<TableStream> table_streams;
        table_streams.reserve(candidates.size());
        for(const auto& candidate: candidates){
            TableStream table_stream;

            table_stream.idx.open(_working_dir / (to_string(candidate.sstable_id) + ".idx"));
            table_stream.table.open(_working_dir / (to_string(candidate.sstable_id) + ".sstable"));

            if (!table_stream.idx.is_open() || !table_stream.table.is_open()){
                table_stream.idx.close();
                table_stream.table.close();
                throw runtime_error("unable to open ss-table with id: " + to_string(candidate.sstable_id));
            }

            SSTableIO::read_varint(table_stream.idx); // skip word amount

            if (read_next_word(table_stream.idx, table_stream.table, table_stream.current_word, table_stream.current_positions, exist_condition)){
                table_streams.push_back(move(table_stream));
                pq.push(&table_streams.back());
            }   
            else{
                table_stream.idx.close();
                table_stream.table.close();
            }

        }


        vector<pair<string, vector<Position>>> merged_chunk;
        uint16_t current_count = 0;

        string prev_word;
        vector<Position> accumulated;

        while (!pq.empty()){
            TableStream* table_stream = pq.top();
            pq.pop();

            if (!prev_word.empty() && table_stream->current_word != prev_word) {
                if (!accumulated.empty()){
                    merged_chunk.emplace_back(std::move(prev_word), std::move(accumulated));
                    current_count += accumulated.size();

                    if (current_count > MAX_CHUNK_COUNT) {
                        SSTableMeta table_meta = SSTableIO::write_table(merged_chunk, _working_dir, write_level);
                        new_table_meta.push_back(table_meta);

                        merged_chunk.clear();
                        current_count = 0;
                    }
                }

                accumulated.clear();
            }
            prev_word = table_stream->current_word;
            accumulated.insert(accumulated.end(), 
                        make_move_iterator(table_stream->current_positions.begin()), 
                        make_move_iterator(table_stream->current_positions.end()));

            if (read_next_word(table_stream->idx, table_stream->table, table_stream->current_word, table_stream->current_positions, exist_condition)){
                pq.push(table_stream);
            }
            else{
                table_stream->idx.close();
                table_stream->table.close();
            }
        }

        if (!prev_word.empty() && !accumulated.empty()) {
            merged_chunk.emplace_back(std::move(prev_word), std::move(accumulated));
            current_count += accumulated.size();
            accumulated.clear();
        }

        if (!merged_chunk.empty()){
            SSTableMeta table_meta = SSTableIO::write_table(merged_chunk, _working_dir, write_level);
            new_table_meta.push_back(table_meta);
            merged_chunk.clear();
            current_count = 0;
        }

        return new_table_meta;
    }


    void CompactionManager::remove_old_tables(const vector<SSTableMeta>& candidates, vector<vector<SSTableMeta>>& levels, uint8_t level) const{
        for (const auto& meta: candidates){
            path sstable_path = _working_dir / (to_string(meta.sstable_id) + ".sstable");
            path index_path = _working_dir / (to_string(meta.sstable_id) + ".idx");
            remove(sstable_path);
            remove(index_path);
        }

        auto remove_old = [&](const SSTableMeta& m){
            return find(candidates.begin(), candidates.end(), m) != candidates.end();
        };

        levels[level].erase(remove_if(levels[level].begin(), levels[level].end(), remove_old), levels[level].end());

        uint8_t next_level = level + 1;
        if (next_level < levels.size()){
            levels[next_level].erase(remove_if(levels[next_level].begin(), levels[next_level].end(), remove_old), levels[next_level].end());
        }


    }


    bool CompactionManager::read_next_word(ifstream& idx, ifstream& table, string& word, vector<common::Position>& positions, const function<bool(uint32_t)>& exist_condition) const{
        if (idx.eof()){
            return false;
        }

        SSTableIO::read_varint(idx); // skip offset

        uint32_t key_len = SSTableIO::read_varint(idx);
        word.clear();
        word.resize(key_len, '\0');
        idx.read(word.data(), key_len);


        uint32_t vector_size = SSTableIO::read_varint(table);
        positions.clear();
        positions.reserve(vector_size);

        for(int i = 0; i < vector_size; i++){
            uint32_t file_id = SSTableIO::read_varint(table);
            uint32_t line_number = SSTableIO::read_varint(table);
            uint32_t indent = SSTableIO::read_varint(table);
            uint32_t word_index = SSTableIO::read_varint(table);
            
            if (!exist_condition(file_id)){
                continue;
            }
            positions.emplace_back(file_id, line_number, indent, word_index);
        } 

        return true;
    }

}