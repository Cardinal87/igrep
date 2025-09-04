
#include "indexer/Index.h"
#include "searcher/Searcher.h"
#include "searcher/SearchResult.h"
#include "searcher/SearchChain.h"
#include "utils/StringUtils.h"
#include<vector>
#include<sstream>
#include<string>
#include<fstream>
#include<format>
#include<cstdint>

using namespace igrep::indexer;
using namespace igrep::searcher;
using namespace igrep::utils;
using namespace std;

namespace igrep::searcher{
    

    Searcher::Searcher(const Index& index) : index_(index){
        
    }

    const vector<SearchResult> Searcher::search(const string& query) const{
        auto normalized_query = StringUtils::normalize_line(query);
        if (normalized_query.empty()) return {};


        istringstream iss(normalized_query);
        string word;
        iss >> word;
        vector<SearchChain> result;
        for(auto& position : index_.get_positions(word)){
            result.emplace_back(position, position);
        }
        
 
        while(iss >> word){
            auto positions = index_.get_positions(word);
            get_intersections(result, positions);
            if(result.size() == 0){
                return {};
            }
        }

        return map_result(result);
    }

    void Searcher::get_intersections(vector<SearchChain>& chain_vector, const vector<Position>& positions) const{
        for(auto& chain: chain_vector){
            for(const auto position: positions){
                if(chain.end.filename != position.filename) continue;

                if(position.word_index == chain.end.word_index + 1){
                    chain.end = position;
                    break;
                }

            }
        }

    }
    

    const vector<SearchResult> Searcher::map_result(const vector<SearchChain>& chains) const{
        vector<SearchResult> result;

        for (const auto& chain: chains){
            string context = get_context(chain);
            result.emplace_back(context, chain.start.filename, chain.start.line_number);

        }
        return result;
    }


    const string Searcher::get_context(const SearchChain& chain) const{
        ifstream ifs(chain.start.filename);
        if (!ifs.is_open()){
            throw runtime_error(format("unable open file {}", chain.start.filename));
        }
        string context;

        string current_line;
        uint32_t current_line_number = 1;
        bool read = false;
        while(getline(ifs, current_line)){
            if (current_line_number >= chain.start.line_number && current_line_number <= chain.end.line_number) {
            context += current_line + '\n';
            }
            if (current_line_number > chain.end.line_number) {
                break;
            }
            current_line_number++;
        }
        ifs.close();
        return context;

    } 
}