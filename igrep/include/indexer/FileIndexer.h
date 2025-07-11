#pragma once

#include<filesystem>
#include "Index.h"

using namespace std::filesystem;
using namespace std;

namespace include::indexer
{
	class FileIndexer {
	public:
		void index_file(path& file);
		void index_directory(path& dir);

		void load_index();

		const Index& get_index() const { return index; };
	
	private:
		Index& index;

		void save_index(string& filename) const;

	};



}