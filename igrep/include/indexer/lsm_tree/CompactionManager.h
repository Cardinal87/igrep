#pragma once
#include "indexer/lsm_tree/SSTableManager.h"

namespace igrep::indexer::lsm_tree
{
    class ComapactionManager{
        public:
            explicit ComapactionManager(SSTableManager& table_manager);
            ~ComapactionManager() = default;

            void compact();


        private:
            SSTableManager& _table_manager;

    };
} 
