#pragma once


#include<string>

namespace igrep::searcher
{
   struct SearchResult
   {
      const std::string result;
      const std::string file_path;
      const int line;


      bool operator==(const SearchResult& other) const{
         return result == other.result &&
                file_path == other.file_path &&
                line == other.line;
      }
   };
    
    
   
}
