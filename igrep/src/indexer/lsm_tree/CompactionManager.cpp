#include "indexer/lsm_tree/CompactionManager.h"
#include<filesystem>
#include<cstdint>
#include<vector>
#include<cmath>

using namespace std;
using namespace std::filesystem;
using namespace igrep::indexer::lsm_tree;
using namespace igrep::indexer::common;

namespace igrep::indexer::lsm_tree{
    ComapactionManager::ComapactionManager(path working_dir): _working_dir(working_dir){}
    
    

    void ComapactionManager::compact(vector<vector<SSTableMeta>>& tables_meta) const{

        int8_t level_for_compact = pick_level_for_compact(tables_meta);
        
        vector<SSTableMeta> candidates = pick_candidates(level_for_compact, tables_meta);


    }


    int8_t ComapactionManager::pick_level_for_compact(vector<vector<SSTableMeta>>& levels) const {
        for (int8_t lv = 0; lv < levels.size(); lv++){
            int8_t level_size = levels[lv].size();

            if (level_size >= MAX_L0_SIZE * pow(FANOUT, lv)){
                return lv;
            }
        }

        return -1;
    }

    vector<SSTableMeta> ComapactionManager::pick_candidates(uint8_t level, const vector<vector<SSTableMeta>>& levels) const {
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



    void ComapactionManager::merge(vector<SSTableMeta> candidates) const{
        
    }
}