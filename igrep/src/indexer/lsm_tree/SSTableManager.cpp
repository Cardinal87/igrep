#include "indexer/lsm_tree/SSTableManager.h"
#include "indexer/lsm_tree/SSTableMeta.h"
#include "indexer/lsm_tree/SSTableIO.h"
#include<string>
#include<fstream>
#include<filesystem>
#include<vector>
#include<cstdint>
#include<random>

using namespace std::filesystem;
using namespace std;
using namespace igrep::indexer::lsm_tree;
using namespace igrep::indexer::common;

namespace igrep::indexer::lsm_tree{
    SSTableManager::SSTableManager(path working_dir): _working_dir(working_dir){}


    void SSTableManager::load_metadata(){
        path meta_path = _working_dir / "tables.meta";
        ifstream ifs(meta_path, ios::binary);

        if (!ifs.is_open()){
            throw runtime_error("failed to open file" + meta_path.string());
        }
        
        try{

            uint32_t levels_size = SSTableIO::read_varint(ifs);
            _levels.resize(levels_size);
            
            for (uint8_t l = 0; l < levels_size; l++){
                uint32_t meta_size = SSTableIO::read_varint(ifs);

                for(uint32_t i = 0; i < meta_size; i++){
                    uint8_t level = static_cast<uint8_t>(SSTableIO::read_varint(ifs));

                    uint32_t table_id = SSTableIO::read_varint(ifs);

                    uint32_t first_word_size =  SSTableIO::read_varint(ifs);
                    string first_word(first_word_size, '\0');
                    ifs.read(first_word.data(), first_word_size);

                    uint32_t last_word_size =  SSTableIO::read_varint(ifs);
                    string last_word(last_word_size, '\0');
                    ifs.read(last_word.data(), last_word_size);

                    _levels[level].emplace_back(level, table_id, first_word, last_word);
                }
            }

            ifs.close();
        }
        catch(exception& ex){
            _levels.clear();
            ifs.close();
            throw;
        }
        
    }

    void SSTableManager::save_metadata() const{
        path meta_path = _working_dir / "tables.meta";

        ofstream ofs(meta_path, ios::binary);

        if (!ofs.is_open()){
            throw runtime_error("failed to open file" + meta_path.string());
        }

        try{

            SSTableIO::write_varint(ofs, _levels.size());

            for(const auto& level: _levels){
                SSTableIO::write_varint(ofs, level.size());
                for(const auto& meta_object: level){
                    SSTableIO::write_varint(ofs, meta_object.level);
                    SSTableIO::write_varint(ofs, meta_object.sstable_id);

                    SSTableIO::write_varint(ofs, meta_object.first_word.length());
                    ofs.write(meta_object.first_word.data(), meta_object.first_word.length());

                    SSTableIO::write_varint(ofs, meta_object.last_word.length());
                    ofs.write(meta_object.last_word.data(), meta_object.last_word.length());
                }
            }

            ofs.close();
        }
        catch(exception& ex){
            remove(meta_path);
            ofs.close();
            throw;
        }
    }

    vector<Position> SSTableManager::find_word(const string& word) const {
        
        vector<Position> result;
        for(const auto& level: _levels){

            for(const auto& meta : level){
                bool after_start = (word >= meta.first_word);
                bool before_end = (word <= meta.last_word);                          
                
                if(after_start && before_end){
                    path table_path = _working_dir / (to_string(meta.sstable_id) + ".sstable");
                    path index_path = _working_dir / (to_string(meta.sstable_id) + ".idx");
                    ifstream idx(index_path, ios::binary);
                    if(!idx.is_open()){
                        throw runtime_error("failed to open file" + table_path.string());
                    }
                    uint64_t offset;
                    bool is_found = false;
                    uint32_t word_amount = SSTableIO::read_varint(idx);
                    for(int i = 0; i < word_amount; i++){
                        offset = SSTableIO::read_varint(idx);

                        uint32_t key_len = SSTableIO::read_varint(idx);
                        string key(key_len, '\0');
                        idx.read(key.data(), key_len);
                        if(word == key){
                            is_found = true;
                            break;
                        }

                    }
                    idx.close();
                    if(!is_found){
                        continue;
                    }


                    ifstream ifs(table_path, ios::binary);
                    if(!ifs.is_open()){
                        throw runtime_error("failed to open file" + table_path.string());
                    }
                    

                    ifs.seekg(offset);
                    uint32_t vector_size = SSTableIO::read_varint(ifs);

                    for(int i = 0; i < vector_size; i++){
                        uint32_t file_id = SSTableIO::read_varint(ifs);
                        uint32_t line_number = SSTableIO::read_varint(ifs);
                        uint32_t indent = SSTableIO::read_varint(ifs);
                        uint32_t word_index = SSTableIO::read_varint(ifs);

                        result.emplace_back(file_id, line_number, indent, word_index);
                    } 
                    ifs.close();

                    if (meta.level > 0){
                        break;
                    }
                    
                } 
                
            }
        }
        return result;
    }

    void SSTableManager::write(const vector<pair<string,vector<Position>>>& sorted_positions){
        SSTableMeta table_meta = SSTableIO::write_table(sorted_positions, _working_dir, 0);

        if (_levels.empty()){
            _levels.resize(1);
        }

        _levels[0].push_back(table_meta);
    }

}