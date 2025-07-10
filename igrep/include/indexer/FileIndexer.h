#pragma once

#include<filesystem>

using namespace std::filesystem;
using namespace std;

namespace include::indexer
{
	class FileIndexer {
	public:
		void index_file(path& file);
		void index_directory(path& dir);

	
	private:
		void save_index(string& filename);

	};



}