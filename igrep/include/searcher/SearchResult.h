#pragma once


#include<string>

namespace igrep::searcher
{
   struct SearchResult
   {
        const std::string result;
        const std::string file_path;
        const int line;
   };
    
    
}
