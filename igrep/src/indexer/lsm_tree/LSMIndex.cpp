#include"indexer/lsm_tree/LSMIndex.h"
#include"indexer/lsm_tree/SSTableManager.h"
#include"indexer/lsm_tree/CompactionManager.h"
#include"indexer/lsm_tree/MemTable.h"
#include"indexer/common/IndexBase.h"
#include"utils/StringUtils.h"
#include"indexer/common/Position.h"
#include"indexer/lsm_tree/SSTableIO.h"

#include<string>
#include<filesystem>
#include<vector>
#include<unordered_map>
#include<cstdint>
#include<algorithm>
#include<utility>
#include<format>
#include<fstream>

using namespace std;
using namespace igrep::indexer::lsm_tree;
using namespace igrep::indexer::common;
using namespace std::filesystem;
using namespace igrep::utils;

namespace igrep::indexer::lsm_tree{
    LSMIndex::LSMIndex(path working_dir): 
        _working_dir(working_dir),
        _compaction_manager(working_dir),
        _table_manager(working_dir){};



    void LSMIndex::process_file(const path& filepath){
        if (is_file_indexed(filepath)){
            return;
        }

        if (!extensions.contains(filepath.extension().string())){
			throw runtime_error(format("file {} extenstion is not supported", filepath.filename().string()));
		}

        index_file(filepath);

        if (!_mem_table.is_empty()){
            flush_memtable();
        }
    }

    void LSMIndex::process_directory(const path& dirpath){
        if (!exists(dirpath)){
			throw runtime_error(format("File or directory {} does not exists", dirpath.string()));
		}
        if (!is_directory(dirpath)){
			throw runtime_error(format("{} is not directory", dirpath.string()));
		}

        for (const auto& entry : recursive_directory_iterator(dirpath)) {

			if (is_regular_file(entry.path()) && extensions.contains(entry.path().extension().string())) {
				index_file(entry.path());
			}
		}

        if (!_mem_table.is_empty()){
            flush_memtable();
        }
    }

    void LSMIndex::remove_file(const path& filepath){
        //not implemented yet
    }

    vector<Position> LSMIndex::get_positions(std::string& query) const{
        auto positions = _table_manager.find_word(query);
        return positions;
    }

    bool LSMIndex::is_file_indexed(const path& filepath) const{
        path absolute_path = weakly_canonical(absolute(path(filepath)));
		return file_to_id.contains(absolute_path);
    }

    void LSMIndex::flush_memtable(){
        auto sorted_pairs = _mem_table.flush_to_sstable();
        auto metadata_link = _table_manager.get_metadata();
        
        _compaction_manager.compact(metadata_link);
    }

    path LSMIndex::get_path_by_id(const uint32_t& id) const{
        const auto it = id_to_file.find(id);
        if (it == id_to_file.end()){
            return {};
        }
        return it -> second;
    }

    void LSMIndex::compact(){
        auto& talbes_meta = _table_manager.get_metadata();
        _compaction_manager.compact(talbes_meta);
    }


    void LSMIndex::index_line(string& line, const uint32_t& file_id, const uint32_t& line_number, uint32_t& word_index){
		string lower_case_line = StringUtils::to_lower_case_copy(line);
		string normalized =  StringUtils::normalize_line(line);
		istringstream iss(normalized);
		string word;
		uint32_t indent;
		uint32_t start_pos = 0;
		while (iss >> word) {
			indent = lower_case_line.find(word, start_pos);
			start_pos = indent + 1;

            Position pos(file_id, line_number, indent, word_index++);
			_mem_table.insert(word, move(pos));
		}
    }


    void LSMIndex::index_file(const path& filepath){
        ifstream ifs(filepath);
		uint32_t word_index = 1;

        path absolute_path = weakly_canonical(absolute(path(filepath)));
        uint32_t file_id;
		if (file_to_id.contains(absolute_path)){
			file_id = file_to_id[absolute_path];
		}
		else{
			file_id = StringUtils::get_file_hash(absolute_path);
			file_to_id[absolute_path] = file_id;
			id_to_file[file_id] = absolute_path;
		}

		if (ifs.is_open()) {
			string line;
			uint32_t line_number = 1;
			while (getline(ifs, line)) {
				index_line(line, file_id, line_number, word_index);
                line_number++;
                if(_mem_table.is_full()){
                    flush_memtable();
                }
			}
		}
		else {
			throw runtime_error(format("Unable to open file {}", filepath.string()));
		}

		ifs.close();
    }


    void LSMIndex::serialize() const{
        _table_manager.save_metadata();

        path meta_file = _working_dir / "index.meta";
        ofstream ofs(meta_file, ios::binary);

        if (!ofs.is_open()){
            throw runtime_error(format("Unable to open file {}", meta_file.string()));
        }

        uint8_t type_len = TYPE.size();
        ofs.write(reinterpret_cast<char*>(type_len), sizeof(type_len));
        ofs.write(TYPE.data(), type_len);

        uint32_t map_size = file_to_id.size();
        SSTableIO::write_varint(ofs, map_size);
        
        for(const auto& pair: file_to_id){
            string filename = pair.first.string();
            uint16_t filename_len = filename.size();
            SSTableIO::write_varint(ofs, filename_len);
            ofs.write(filename.data(), filename_len);

            SSTableIO::write_varint(ofs, pair.second);
        }
        ofs.close();
    }

    void LSMIndex::deserialize(){
        path meta_file = _working_dir / "index.meta";
        ifstream ifs(meta_file, ios::binary);

        if(!ifs.is_open()){
            throw runtime_error(format("Unable to open file {}", meta_file.string()));
        }
        try{
            uint8_t type_len;
            string type(type_len, '\0');
            ifs.read(reinterpret_cast<char*>(type_len), sizeof(type_len));
            ifs.read(type.data(), type_len);

            if (type != TYPE){
                throw runtime_error(format("Invalid index type {}", type));
            }

            uint32_t map_size = SSTableIO::read_varint(ifs);
            id_to_file.reserve(map_size);
            file_to_id.reserve(map_size);

            for (int i = 0; i < map_size; i++){
                uint16_t filename_len = SSTableIO::read_varint(ifs);
                string filename(filename_len, '\0');
                ifs.read(filename.data(), filename_len);
                uint32_t id = SSTableIO::read_varint(ifs);

                path filepath = path(filename);
                id_to_file[id] = filepath;
                file_to_id[filepath] = id;
            }

            ifs.close();
            _table_manager.load_metadata();
        }
        catch(const exception&){
            id_to_file.clear();
            file_to_id.clear();
            ifs.close();
            throw;
        }
        
    }


    bool LSMIndex::operator==(const LSMIndex& other) const{
        return file_to_id == other.file_to_id;
    }
}