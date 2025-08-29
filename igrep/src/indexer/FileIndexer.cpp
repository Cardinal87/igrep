#include "indexer/FileIndexer.h"
#include "utils/StringUtils.h"
#include "indexer/Index.h"
#include<filesystem>
#include<fstream>
#include<format>

using namespace std;
using namespace std::filesystem;
using namespace igrep::indexer;
using namespace igrep::utils;


namespace igrep::indexer {

	FileIndexer::FileIndexer(Index& index): _index(index){}

	void FileIndexer::index_file(const path& file_path) {
		ifstream ifs;
		ifs.open(file_path);
		int word_index = 1;
		if (ifs.is_open()) {
			string line;
			size_t line_number = 1;
			while (getline(ifs, line)) {
				_index.process_line(line, file_path.string(), line_number++, word_index);
			}
		}
		else {
			throw runtime_error(format("Unable to open file {}", file_path.string()));
		}

		ifs.close();
	}



	void FileIndexer::index_directory(const path& dir_path) {

		if (!exists(dir_path)){
			throw runtime_error(format("File or directory {} does not exists", dir_path.string()));
		}
		if (!is_directory(dir_path)){
			throw runtime_error(format("{} is not directory", dir_path.string()));
		}
		for (const auto& entry : recursive_directory_iterator(dir_path)) {

			if (is_regular_file(entry.path())) {
				index_file(entry.path());
			}
		}
	}


}