#pragma once

#include<vector>
#include<string>
#include "../../include/indexer/Index.h"
#include "SearchResult.h"



namespace igrep::searcher{
    class Searcher{
        public:
            explicit Searcher(const igrep::indexer::Index& index);

            std::vector<SearchResult> search(const std::string& query) const;

        private:
            const igrep::indexer::Index& index_;


    };
}