#include<gtest/gtest.h>
#include"indexer/Index.h"
#include"indexer/Position.h"
#include<filesystem>
#include<fstream>
#include<string>
#include<vector>

using namespace std;
using namespace std::filesystem;
using namespace igrep::indexer;

TEST(IndexTest, ProcessLine_GetPositions_Success){
    Index index;
    int word_index = 1;
    string line = "Hello, world";
    path filepath = "/path/to/file.txt";

    index.process_line(line, filepath, 1, word_index);

    vector<Position> position = index.get_positions("world");
    ASSERT_EQ(position.size(), 1);
    EXPECT_EQ(position[0].filename, filepath.string());
    EXPECT_EQ(position[0].line_number, 1);
    EXPECT_EQ(position[0].indent, 7);
    EXPECT_EQ(position[0].word_index, 2);
}

TEST(IndexTest, GetPositions_NonExistentWord_ReturnEmpty){
    Index index;

    vector<Position> position = index.get_positions("nonexistent");
    ASSERT_EQ(position.empty(), true);
}

TEST(IndexTest, SerializeIndex_ThenDeserializeToAnother_SameIndexes){
    Index index;
    string line = "Hello, world";
    int first_file_index = 1;
    int second_file_index = 1;
    index.process_line(line, "/path/to/file1.txt", 1, first_file_index);
    index.process_line(line, "/path/to/file2.txt", 1, second_file_index);
    path index_path = string(SOURCE_DIR) + "/testdata/index.bin";
    index.serialize(index_path);


    Index deserialized_index;
    deserialized_index.deserialize(index_path);


    EXPECT_EQ(index, deserialized_index);
    remove(index_path);
}

TEST(IndexTest, DeserializeIndex_NonExistentPath_ThrowRuntimeError){
    path index_path = "/nonexistent/path/to/index.bin";
    Index index;
    
    ASSERT_THROW(index.deserialize(index_path), runtime_error);
}
