#include<gtest/gtest.h>
#include"indexer/FileIndexer.h"
#include"indexer/Position.h"
#include"indexer/Index.h"
#include"searcher/Searcher.h"
#include"searcher/SearchResult.h"
#include<filesystem>
#include<fstream>
#include<string>
#include<vector>

using namespace std;
using namespace std::filesystem;
using namespace igrep::indexer;
using namespace igrep::searcher;


TEST(SearcherTest, SearchWord_ValidQuery_ReturnWithContext){
    Index index;
    FileIndexer indexer(index);
    path filepath = string(SOURCE_DIR) + "/testdata/wiki_data.txt"; 
    indexer.index_file(filepath);
    Searcher searcher(index);
    string expected_context = "The exact numbers of the fishing fleet, thought to be in poor condition, are not known. "
                              "In 1998, North Korea had eight large fishing vessels (3,750 displacement tonnage,  2,759 gross tons, 83 m length, 2,250 horsepower) "
                              "and 1,545 small vessels (485 displacement tonnage,  267 gross tons, 39 m length, 400 horsepower). Numbers of smaller vessels were not reported.";
    expected_context += '\n';
    SearchResult expected_result{expected_context, filepath, 1};
    
    
    vector<SearchResult> results = searcher.search("fleet");


    ASSERT_EQ(results.size(), 1);
    SearchResult result = results[0];
    ASSERT_EQ(expected_result, result);


}

TEST(SearcherTest, SearchQuery_QuerySeparatedByLines_ReturnCorrectContext){
    Index index;
    FileIndexer indexer(index);
    path filepath = string(SOURCE_DIR) + "/testdata/external.txt"; 
    indexer.index_file(filepath);
    Searcher searcher(index);
    string expected_context = "Data of external file\n" 
                              "of the directory testdata\n";
    SearchResult expected_result{expected_context, filepath, 1};


    vector<SearchResult> results = searcher.search("file of the directory");


    ASSERT_EQ(results.size(), 1);
    SearchResult result = results[0];
    ASSERT_EQ(expected_result, result);
}


TEST(SearcherTest, SearchQuery_Empty_ReturnEmpty){
    Index index;
    FileIndexer indexer(index);
    path filepath = string(SOURCE_DIR) + "/testdata/wiki_data.txt"; 
    indexer.index_file(filepath);
    Searcher searcher(index);


    vector<SearchResult> results = searcher.search("");


    ASSERT_TRUE(results.empty());
}

TEST(SearcherTest, SearchWord_NonExistent_ReturnEmpty){
    Index index;
    FileIndexer indexer(index);
    path filepath = string(SOURCE_DIR) + "/testdata/wiki_data.txt"; 
    indexer.index_file(filepath);
    Searcher searcher(index);


    vector<SearchResult> results = searcher.search("nonexistent");


    ASSERT_TRUE(results.empty());
}