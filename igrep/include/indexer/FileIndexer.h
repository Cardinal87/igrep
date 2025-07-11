#pragma once

#include<filesystem>
#include "Index.h"
#include<string>

namespace include::indexer
{
	class FileIndexer {
	public:
		void index_file(std::filesystem::path& file);
		void index_directory(std::filesystem::path& dir);

		void load_index();

		const Index& get_index() const { return index; };
	
	private:
		Index& index;

		void save_index(std::string& filename) const;

	};



}