#include<gtest/gtest.h>
#include"indexer/Index.h"
#include"indexer/Position.h"
#include<filesystem>
#include<fstream>
#include<string>
#include<vector>
#include<cstdint>

using namespace std;
using namespace std::filesystem;
using namespace igrep::indexer;

TEST(IndexTest, ProcessLine_GetPositions_Success){
    Index index;
    uint32_t word_index = 1;
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
    uint32_t first_file_index = 1;
    uint32_t second_file_index = 1;
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


TEST(IndexTest, DeleteFile_IndexedFile_CorrectIndex){
    Index index;
    Index expected_index;
    string first_file = "/path/to/file1.txt";
    string second_file = "/path/to/file2.txt";
    string first_string = "First file string";
    string second_string = "Second file string";
    uint32_t first_file_index = 1;
    uint32_t second_file_index = 1;
	uint32_t expected_word_index = 1;
    index.process_line(first_string, first_file, 1, first_file_index);
    index.process_line(second_string, second_file, 1, second_file_index);
    expected_index.process_line(second_string, second_file, 1, expected_word_index);


    index.remove_file(first_file);


    ASSERT_EQ(index, expected_index);
}
