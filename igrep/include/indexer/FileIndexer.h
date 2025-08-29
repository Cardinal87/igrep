#pragma once

#include<filesystem>
#include "Index.h"
#include<string>

namespace igrep::indexer
{
	class FileIndexer {
	public:
		explicit FileIndexer(Index& index);
		
		void index_file(const std::filesystem::path& file);
		void index_directory(const std::filesystem::path& dir);
	
	private:
		Index& _index;
		
	};
}