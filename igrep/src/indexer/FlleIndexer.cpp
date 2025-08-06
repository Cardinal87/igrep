#include "../../include/indexer/FileIndexer.h"
#include "../../include/utils/StringUtils.h"
#include "../../include/indexer/Index.h"
#include<filesystem>
#include<fstream>
#include<format>

using namespace std;
using namespace std::filesystem;
using namespace include::indexer;
using namespace include::utils;


namespace include::indexer {

	

	void FileIndexer::index_file(const path& file_path) {
		ifstream ifs;
		ifs.open(file_path);

		if (ifs.is_open()) {
			string line;
			size_t line_number = 1;
			while (getline(ifs, line)) {
				index.process_line(line, file_path.string(), line_number++);
			}
		}
		else {
			throw runtime_error(format("Unable to open file {}", file_path));
		}

		ifs.close();
	}



	void FileIndexer::index_directory(const path& dir_path) {

		for (const auto& entry : recursive_directory_iterator(dir_path)) {

			if (is_regular_file(entry.path())) {
				index_file(entry.path());
			}
		}
	}


	void FileIndexer::load_index() {
		index.deserialize("./index.bin");
	}

	void FileIndexer::save_index() const {
		index.serialize("./index.bin");
	}

}