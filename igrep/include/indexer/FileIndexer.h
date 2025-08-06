#pragma once

#include<filesystem>
#include "Index.h"
#include<string>

namespace igrep::indexer
{
	class FileIndexer {
	public:
		FileIndexer() = default;
		
		void index_file(const std::filesystem::path& file);
		void index_directory(const std::filesystem::path& dir);

		void load_index();

		void save_index() const;

		const Index& get_index() const { return index; };
	
	private:
		Index index;
		
	};
}