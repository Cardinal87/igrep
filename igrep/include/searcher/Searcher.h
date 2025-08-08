#pragma once

#include<vector>
#include<string>
#include "../../include/indexer/Index.h"
#include "../../include/indexer/Position.h"
#include "SearchResult.h"



namespace igrep::searcher{
    class Searcher{
        public:
            explicit Searcher(const igrep::indexer::Index& index);

            const std::vector<SearchResult> search(const std::string& query) const;

        private:
            void get_intersections(std::vector<igrep::indexer::Position>& first,const std::vector<igrep::indexer::Position>& second) const;
            const std::vector<SearchResult> map_result(const std::vector<igrep::indexer::Position>& positions) const;
            const std::string get_context(const std::string& filename, size_t start_line, size_t end_line) const;

            const igrep::indexer::Index& index_;
            
        

    };
}