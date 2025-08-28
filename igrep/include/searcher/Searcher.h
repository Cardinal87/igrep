#pragma once

#include<vector>
#include<string>
#include "indexer/Index.h"
#include "indexer/Position.h"
#include "SearchResult.h"
#include "SearchChain.h"



namespace igrep::searcher{
    class Searcher{
        public:
            explicit Searcher(const igrep::indexer::Index& index);

            const std::vector<SearchResult> search(const std::string& query) const;

        private:
            void get_intersections(std::vector<igrep::searcher::SearchChain>& chain_vector,const std::vector<igrep::indexer::Position>& positions) const;
            const std::vector<SearchResult> map_result(const std::vector<igrep::searcher::SearchChain>& chains) const;
            const std::string get_context(const igrep::searcher::SearchChain& chain) const;

            const igrep::indexer::Index& index_;
            
        

    };
}