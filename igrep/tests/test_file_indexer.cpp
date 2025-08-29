#include<gtest/gtest.h>
#include"indexer/FileIndexer.h"
#include"indexer/Position.h"
#include"indexer/Index.h"
#include<filesystem>
#include<fstream>
#include<string>
#include<vector>

using namespace std;
using namespace std::filesystem;
using namespace igrep::indexer;



TEST(FileIndexerTest, IndexFile_ValidPath_CorrectIndex){
    path filepath = string(SOURCE_DIR) + "/testdata/wiki_data.txt";
    string fishing = "fishing";
    vector<Position> expected_fishing_word_positions = {
        Position{filepath, 1, 25, 6},
        Position{filepath, 1, 125, 24},
        Position{filepath, 5, 21, 165},
        Position{filepath, 5, 163, 192},
        Position{filepath, 5, 226, 201},
        Position{filepath, 6, 42, 214},
        Position{filepath, 6, 202, 242},
        Position{filepath, 6, 228, 245},
        Position{filepath, 7, 18, 252},
        Position{filepath, 10, 84, 379}
    };

    string fish = "fish";
    vector<Position> expected_fish_word_positions = {
        Position{filepath, 2, 0, 63},
        Position{filepath, 2, 54, 72},
        Position{filepath, 3, 136, 131},
        Position{filepath, 6, 114, 225},
        Position{filepath, 8, 0, 284},
        Position{filepath, 8, 81, 298},
        Position{filepath, 8, 169, 311},
        Position{filepath, 8, 253, 324}
    };

    Index index;
    FileIndexer indexer(index);
    indexer.index_file(filepath);


    vector<Position> fishing_word_positions = index.get_positions(fishing);
    vector<Position> fish_word_positions = index.get_positions(fish);
    EXPECT_EQ(fishing_word_positions, expected_fishing_word_positions);
    EXPECT_EQ(fish_word_positions, expected_fish_word_positions);
}

TEST(FileIndexerTest, IndexFile_NonExistentFile_ThrowRuntimeError){
    path path = "/non/existing/file.txt";
    Index index;
    FileIndexer indexer(index);
    ASSERT_THROW(indexer.index_file(path), runtime_error);
}

TEST(FileIndexerTest, IndexDirectory_ValidPath_CorrectIndex){
    path dirpath = string(SOURCE_DIR) + "/testdata";
    path internal_dir_file = string(SOURCE_DIR) + "/testdata/internal/internal.txt";
    path external_dir_file = string(SOURCE_DIR) + "/testdata/external.txt";
    auto expected_internal_position = Position{internal_dir_file, 1, 8, 3};
    auto expected_external_position = Position{external_dir_file, 1, 8, 3};
    Index index;
    FileIndexer indexer(index);
    
    
    indexer.index_directory(dirpath);

    ASSERT_FALSE(index.get_positions("internal").empty());
    ASSERT_FALSE(index.get_positions("external").empty());

    Position internal_position = (index.get_positions("internal"))[0];
    Position external_position = (index.get_positions("external"))[0];
    EXPECT_EQ(internal_position, expected_internal_position);
    EXPECT_EQ(external_position, expected_external_position);
}

TEST(FileIndexerTest, IndexDirectory_NonExistentDir_ThrowRuntimeError){
    path path = "/non/existing/dir";
    Index index;
    FileIndexer indexer(index);
    ASSERT_THROW(indexer.index_directory(path), runtime_error);
}

TEST(FileIndexerTest, IndexDirectory_PathToFileInstead_ThrowRuntimeError){
    path filepath = string(SOURCE_DIR) + "/testdata/wiki_data.txt";
    Index index;
    FileIndexer indexer(index);
    ASSERT_THROW(indexer.index_directory(filepath), runtime_error);
}