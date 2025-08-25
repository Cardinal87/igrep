
#include "indexer/Index.h"
#include "searcher/Searcher.h"
#include "searcher/SearchResult.h"
#include "utils/StringUtils.h"
#include<vector>
#include<sstream>
#include<string>
#include<fstream>
#include<format>

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
        vector<Position> result = index_.get_positions(word);


        while(iss >> word){
            auto positions = index_.get_positions(word);
            get_intersections(result, positions);
            if(result.size() == 0){
                return {};
            }
        }

        return map_result(result);
    }

    void Searcher::get_intersections(vector<Position>& first, const vector<Position>& second) const{
        vector<Position> result;
        for(const auto& pos1: first){
            for(const auto& pos2: second){
                if(pos1.filename != pos2.filename) continue;

                if(pos2.word_index == pos1.word_index + 1){
                    result.push_back(pos2);
                    break;
                }

            }
        }
        first = move(result);

    }
    

    const vector<SearchResult> Searcher::map_result(const vector<Position>& positions) const{
        vector<SearchResult> result;

        for (const auto& position: positions){
            string context = get_context(position.filename, position.line_number - 2, position.line_number);
            result.emplace_back(context, position.filename, position.line_number);

        }
        return result;
    }


    const string Searcher::get_context(const string& filename, size_t start_line, size_t end_line) const{
        ifstream ifs(filename);
        if (!ifs.is_open()){
            throw runtime_error(format("unable open file {}", filename));
        }
        string context;

        string current_line;
        size_t current_line_number = 1;
        bool read = false;
        while(getline(ifs, current_line)){
            if (current_line_number >= start_line && current_line_number <= end_line) {
            context += current_line + '\n';
            }
            if (current_line_number > end_line) {
                break;
            }
            current_line_number++;
        }
        ifs.close();
        return context;

    } 
}