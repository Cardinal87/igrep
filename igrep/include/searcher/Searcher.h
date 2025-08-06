#pragma once

#include<vector>
#include<string>
#include "igrep/include/indexer/Index.h"
#include "SearchResult.h"



namespace include::searcher{
    class Searcher{
        public:
            explicit Searcher(const include::indexer::Index& index);

            std::vector<SearchResult> search(const std::string& query) const;

        private:
            const include::indexer::Index& index_;


    };
}