#include "indexer/lsm_tree/SSTableIO.h"
#include "indexer/common/Position.h"

#include<filesystem>
#include<vector>
#include<fstream>
#include<cstdint>
#include<random>

using namespace std;
using namespace std::filesystem;
using namespace igrep::indexer::common;

namespace igrep::indexer::lsm_tree
{
    uint64_t SSTableIO::read_varint(ifstream& ifs){
        unsigned char buffer;
		ifs.read(reinterpret_cast<char*>(&buffer), 1);
		uint64_t value = static_cast<uint64_t>(buffer & 0x7F);
		uint8_t shift = 7;

		while(buffer & 0x80 && shift < 70){
			ifs.read(reinterpret_cast<char*>(&buffer), 1);
			value |= (static_cast<uint64_t>(buffer & 0x7F) << shift);
			shift += 7;
		}

		return value;
    }

    void SSTableIO::write_varint(ofstream& ofs, uint64_t value){
        while(value >= 0x80){
			char ch = value | 0x80;
			ofs.write(&ch, 1);
			value >>= 7;	
		}
		ofs.write(reinterpret_cast<char*>(&value), 1);
    }


    SSTableMeta SSTableIO::write_table(const vector<pair<string,vector<Position>>>& sorted_positions, const path& working_dir, uint8_t level ){
        random_device ran_dev;
        mt19937 generator(ran_dev());
        uniform_int_distribution<uint32_t> distrib;

        uint32_t table_id = distrib(generator);
        path table_path = working_dir / (to_string(table_id) + ".sstable");
        path index_path = working_dir / (to_string(table_id) + ".idx");
        ofstream ofs(table_path, ios::binary);
        ofstream idx(index_path, ios::binary);
        if (!ofs.is_open()){
            throw runtime_error("failed to open file" + table_path.string());
        }
        if (!idx.is_open()){
            throw runtime_error("failed to open file" + index_path.string());
        }
        
        try{

            SSTableIO::write_varint(idx, sorted_positions.size());

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
        
        return SSTableMeta{level, table_id, sorted_positions.front().first, sorted_positions.back().first};
    }
} // namespace igrep::indexer::lsm_tree
