#pragma once


#include"indexer/common/Position.h"

namespace igrep::searcher{
    struct SearchChain{
        igrep::indexer::common::Position start;
        igrep::indexer::common::Position end;
    };
}