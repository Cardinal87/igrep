#include "indexer/lsm_tree/MemTable.h"
#include<algorithm>

using namespace igrep::indexer::common;
using namespace std;

namespace igrep::indexer::lsm_tree{
    void MemTable::insert(const string& word, const Position& position){
        words[word].push_back(position);
        current_count += 1;
    }

    bool MemTable::is_full() const{
        return current_count >= MAX_COUNT;
    }

    bool MemTable::is_empty() const{
        return current_count == 0;
    }

    vector<pair<string, vector<Position>>> MemTable::flush_to_sstable(){
        vector<pair<string, vector<Position>>> sorted;
        sorted.reserve(words.size());

        for(auto it = words.begin(); it != words.end(); ) {
            sorted.emplace_back(std::move(it->first), std::move(it->second));
            it = words.erase(it);  
        }

        sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b)
        {
            return a.first < b.first;
        });
        current_count = 0;
        return sorted;
    }

}