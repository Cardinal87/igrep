#include "indexer/lsm_tree/MemTable.h"
#include "indexer/common/Position.h"
#include<gtest/gtest.h>

using namespace igrep::indexer::lsm_tree;
using namespace igrep::indexer::common;

TEST(MemtableTest, InsertAndFlush) {
    MemTable memtable;
    memtable.insert("hello", Position{1, 10, 0, 1});
    memtable.insert("world", Position{1, 10, 6, 2});
    memtable.insert("hello", Position{1, 11, 0, 3});


    EXPECT_FALSE(memtable.is_empty());
    EXPECT_FALSE(memtable.is_full());

    auto sorted = memtable.flush_to_sstable();

    ASSERT_EQ(sorted.size(), 2);
    EXPECT_EQ(sorted[0].first, "hello");
    EXPECT_EQ(sorted[0].second.size(), 2);
    EXPECT_EQ(sorted[1].first, "world");
    EXPECT_EQ(memtable.is_empty(), true);
}