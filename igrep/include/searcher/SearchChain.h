#pragma once


#include"indexer/Position.h"

namespace igrep::searcher{
    struct SearchChain{
        igrep::indexer::Position start;
        igrep::indexer::Position end;
    };
}