#pragma once


#include<string>

namespace igrep::searcher
{
   struct SearchResult
   {
      std::string result;
      std::string file_path;
      uint32_t line;


      bool operator==(const SearchResult& other) const{
         return result == other.result &&
                file_path == other.file_path &&
                line == other.line;
      }
   };
    
    
   
}
