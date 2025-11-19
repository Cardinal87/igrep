#include "indexer/lsm_tree/SSTableManager.h"
#include "indexer/lsm_tree/SSTableMeta.h"
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

         if (!exists(meta_path)) {
            return; 
        }

        ifstream ifs(meta_path, ios::binary);

        if (!ifs.is_open()){
            throw runtime_error("failed to open file" + meta_path.string());
        }
        
        try{

            uint32_t levels_size = read_varint(ifs);
            _levels.resize(levels_size);
            
            for (uint8_t l = 0; l < levels_size; l++){
                uint32_t meta_size = read_varint(ifs);

                for(uint32_t i = 0; i < meta_size; i++){
                    uint8_t level = static_cast<uint8_t>(read_varint(ifs));

                    uint32_t table_id = read_varint(ifs);

                    uint32_t first_word_size =  read_varint(ifs);
                    string first_word(first_word_size, '\0');
                    ifs.read(first_word.data(), first_word_size);

                    uint32_t last_word_size =  read_varint(ifs);
                    string last_word(last_word_size, '\0');
                    ifs.read(last_word.data(), last_word_size);

                    _levels[level].emplace_back(level, table_id, first_word, last_word);
                }
            }
        }
        catch(exception& ex){
            _levels.clear();
            ifs.close();
            throw;
        }
        ifs.close();
        
    }

    void SSTableManager::save_metadata() const{
        path meta_path = _working_dir / "tables.meta";

        ofstream ofs(meta_path, ios::binary);

        if (!ofs.is_open()){
            throw runtime_error("failed to open file" + meta_path.string());
        }

        try{

            write_varint(ofs, _levels.size());

            for(const auto& level: _levels){
                write_varint(ofs, level.size());
                for(const auto& meta_object: level){
                    write_varint(ofs, meta_object.level);
                    write_varint(ofs, meta_object.sstable_id);

                    write_varint(ofs, meta_object.first_word.length());
                    ofs.write(meta_object.first_word.data(), meta_object.first_word.length());

                    write_varint(ofs, meta_object.last_word.length());
                    ofs.write(meta_object.last_word.data(), meta_object.last_word.length());
                }
            }
        }
        catch(exception& ex){
            remove(meta_path);
            ofs.close();
            throw;
        }
        ofs.close();
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
                    uint32_t word_amount = read_varint(idx);
                    for(int i = 0; i < word_amount; i++){
                        offset = read_varint(idx);

                        uint32_t key_len = read_varint(idx);
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
                    uint32_t vector_size = read_varint(ifs);

                    for(int i = 0; i < vector_size; i++){
                        uint32_t file_id = read_varint(ifs);
                        uint32_t line_number = read_varint(ifs);
                        uint32_t indent = read_varint(ifs);
                        uint32_t word_index = read_varint(ifs);

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
        random_device ran_dev;
        mt19937 generator(ran_dev());
        uniform_int_distribution<uint32_t> distrib;

        uint32_t table_id = distrib(generator);
        path table_path = _working_dir / (to_string(table_id) + ".sstable");
        path index_path = _working_dir / (to_string(table_id) + ".idx");
        ofstream ofs(table_path, ios::binary);
        ofstream idx(index_path, ios::binary);
        if (!ofs.is_open()){
            throw runtime_error("failed to open file" + table_path.string());
        }
        if (!idx.is_open()){
            throw runtime_error("failed to open file" + index_path.string());
        }
        
        try{

            write_varint(idx, sorted_positions.size());

            for(const auto& [word, pos_vector]: sorted_positions){
                uint64_t offset = static_cast<uint64_t>(ofs.tellp());
                write_varint(idx, offset);
                
                uint32_t word_len = word.length();
                write_varint(idx, word_len);
                idx.write(word.data(), word_len);

                uint32_t vector_size = pos_vector.size();
                write_varint(ofs, vector_size);

                for(const auto& position: pos_vector){
                    write_varint(ofs, position.file_id);
                    write_varint(ofs, position.line_number);
                    write_varint(ofs, position.indent);
                    write_varint(ofs, position.word_index);
                }

            }
        }
        catch(exception& ex){
            ofs.close();
            idx.close();
            remove(table_path);
            remove(index_path);
            throw;
        }
        ofs.close();
        idx.close();
        
        if (_levels.empty()){
            _levels.resize(1);
        }

        _levels[0].emplace_back(0, table_id, sorted_positions.front().first, sorted_positions.back().first);
    }


    void SSTableManager::write_varint(ofstream& ofs, uint32_t value) const{
		while(value >= 0x80){
			char ch = value | 0x80;
			ofs.write(&ch, 1);
			value >>= 7;	
		}
		ofs.write(reinterpret_cast<char*>(&value), 1);
	}
	
	uint32_t SSTableManager::read_varint(ifstream& ifs) const {
		unsigned char buffer;
		ifs.read(reinterpret_cast<char*>(&buffer), 1);
		uint32_t value = static_cast<uint32_t>(buffer & 0x7F);
		uint8_t shift = 7;

		while(buffer & 0x80 && shift < 32){
			ifs.read(reinterpret_cast<char*>(&buffer), 1);
			value |= (static_cast<uint32_t>(buffer & 0x7F) << shift);
			shift += 7;
		}

		return value;
	}
}