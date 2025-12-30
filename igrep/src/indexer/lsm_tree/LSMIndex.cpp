#include"indexer/lsm_tree/LSMIndex.h"
#include"indexer/lsm_tree/SSTableManager.h"
#include"indexer/lsm_tree/CompactionManager.h"
#include"indexer/lsm_tree/MemTable.h"
#include"indexer/common/IndexBase.h"
#include"utils/StringUtils.h"
#include"indexer/common/Position.h"


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
        throw runtime_error("not implemented yet");
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

}