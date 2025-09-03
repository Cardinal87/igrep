// igrep.cpp: определяет точку входа для приложения.
//

#include "igrep.h"

using namespace std;
using namespace std::filesystem;
using namespace igrep::indexer;
using namespace igrep::searcher;

int main(int argc, char* argv[])
{
	try{
		vector<string> args(argv + 1, argv + argc);
		const char* home_var = getenv("HOME");
		if (!home_var) {
			std::cerr << "Error: HOME environment variable not set" << std::endl;
			return 1;
		}
		path igrep_dir = path(home_var) / ".config" / "igrep";
		path index_path = igrep_dir / "index.bin";

		if(!exists(igrep_dir)){
			create_directories(igrep_dir);
		}




		if (args.empty()
		|| find(args.begin(), args.end(), "-v") != args.end()
		|| find(args.begin(), args.end(), "--version") != args.end()){
			cout << "igrep 1.1.0" << endl << "Try 'igrep -h' to get more information" << endl;
			return 0;
		}
		if (find(args.begin(), args.end(), "-h") != args.end()
		|| find(args.begin(), args.end(), "--help") != args.end()){
			const int flagWidth = 20; 
			const int descWidth = 30; 
			
			cout << "Usage: igrep [OPTION] [PARAMS]\n\n"

				<< "CREATING INDEX:\n"
				<< "  Usage: igrep create [PARAMS]\n"
				<< "  Optional flags:\n"
				<< "    " << left << setw(flagWidth) << "-d, --destination"
					<< setw(descWidth) << "Specify path for index file " + format("(default: {})", index_path.string()) << "\n"
				<< "  Examples:\n"
				<< "    igrep create\n"
				<< "    igrep create -d ~/myindex.bin\n\n"

				<< "INDEXING FILES:\n"
				<< "  Usage: igrep index [PARAMS]\n"
				<< "  One of the next flags is required:\n"
				<< "    " << left << setw(flagWidth) << "-f, --file" 
					<< setw(descWidth) << "Specify path to file" << "\n"
				<< "    " << left << setw(flagWidth) << "-d, --directory" 
					<< setw(descWidth) << "Specify directory for recursive indexing" << "\n"
				<< "  Optional flags:\n"
				<< "    " << left << setw(flagWidth) << "-s, --source-index" 
					<< setw(descWidth) << "Specify path to index file " + format("(default: {})", index_path.string()) << "\n"
				<< "  Examples:\n"
				<< "    igrep index -f ./mylog.txt\n"
				<< "    igrep index -d ./logs -s ~/myindex.bin\n\n"
				
				<< "DELETE FILES FROM INDEX:\n"
				<< "  Usage: igrep remove [PARAMS]\n"
				<< "  One of the next flags is required:\n"
				<< "    " << left << setw(flagWidth) << "-f, --file" 
					<< setw(descWidth) << "Specify file path (file does not have to exist)" << "\n"
				<< "  Optional flags:\n"
				<< "    " << left << setw(flagWidth) << "-s, --source-index" 
					<< setw(descWidth) << "Specify path to index file " + format("(default: {})", index_path.string()) << "\n"
				<< "  Examples:\n"
				<< "    igrep remove -f ./old-log.txt -s  ~/myindex.bin\n\n"
				
				<< "FIND QUERIES:\n"
				<< "  Usage: igrep find [PARAMS]\n"
				<< "  One of the next flags is required:\n"
				<< "    " << left << setw(flagWidth) << "-q, --query" 
					<< setw(descWidth) << "Specify string you want to find" << "\n"
				<< "  Optional flags:\n"
				<< "    " << left << setw(flagWidth) << "-s, --source-index" 
					<< setw(descWidth) << "Specify path to index file " + format("(default: {})", index_path.string()) << "\n"
				<< "  Examples:\n"
				<< "    igrep find -q ERROR\n";


			return 0;
		}

		if (args[0] == "create"){
			Index index;
			for(size_t i = 0; i < args.size(); i++){
				if (args[i] == "-d" || args[i] == "--destination"){
					if(i + 1 < args.size()){
						path filepath = args[i + 1];
						if (filepath.extension() != ".bin"){
							cerr << format("Error: invalid index file format {}", filepath.string()) << "\n"
								 << "File must have .bin format" << endl;
							return 1;
						}
						index_path = filepath;
						index.serialize(index_path);
						cout << format("Index was successfully created at {}", index_path.string()) << endl;
						return 0;
					}
					else{
						cerr << "Error: file path was not provided" << endl;
						return 1;
					}
				}
			}
			index.serialize(index_path);
			cout << format("Index was successfully created at {}", index_path.string()) << endl;
			return 0;
		}


		if (args[0] == "index"){
			bool has_file = false;
			bool has_dir = false;
			path file_path;
			path dir_path;

			for(size_t i = 0; i < args.size(); i++){
				if ((args[i] == "-f" || args[i] == "--file") && !(has_file || has_dir)){
					if(i + 1 < args.size()){
						file_path = args[i + 1];
						has_file = true;
						i++;
					}
					else{
						cerr << "Error: file path was not provided" << endl;
						return 1;
					}
				}
				else if ((args[i] == "-d" || args[i] == "--directory") && !(has_file || has_dir)){
					if(i + 1 < args.size()){
						dir_path = args[i + 1];
						has_dir = true;
						i++;
					}
					else{
						cerr << "Error: directory path was not provided" << endl;
						return 1;
					}
				}
				else if (args[i] == "-s" || args[i] == "--source-index"){
					if(i + 1 < args.size()){
						path path_to_index = args[i + 1];
						index_path = path_to_index;
						i++;
					}
					else{
						cerr << "Error: source index path was not provided" << endl;
						return 1;
					}
				}
			}

			Index index;
			index.deserialize(index_path);
			FileIndexer indexer(index);
			
			if (!has_file && !has_dir){
				cerr << "Error: either -f or -d must be specified" << endl;
				return 1;
			}

			if (has_file){
				bool is_success = indexer.index_file(file_path);
				if (is_success){
					cout << "File was successfully indexed" << endl;
				} else{
					cout << "File has already been indexed" << endl;
				}
			}
			if (has_dir){
				indexer.index_directory(dir_path);
				cout << "Directory was successfully indexed" << endl;
			}
			
			index.serialize(index_path);
			return 0;
		}
		
		if(args[0] == "remove"){
			bool has_file = false;
			path file_path;
			for(size_t i = 0; i < args.size(); i++){
				if (args[i] == "-f" || args[i] == "--file"){
					if(i + 1 < args.size()){
						file_path  = args[i + 1];
						has_file = true;
						i++;
					}
					else{
						cerr << "Error: file path was not provided" << endl;
						return 1;
					}
				}
				else if (args[i] == "-s" || args[i] == "--source-index"){
					if(i + 1 < args.size()){
						index_path = args[i + 1];
						i++;
					}
					else{
						cerr << "Error: source index path was not provided" << endl;
						return 1;
					}
				}
			}

			Index index;
			index.deserialize(index_path);

			if (!has_file){
				cerr << "Error: -f flag must be specified" << endl;
				return 1;
			}

			bool is_success = index.remove_file(file_path);
			if (is_success){
				cout << "File was successfully removed from index" << endl;
			} else{
				cout << format("{} was not indexed", file_path.string()) << endl;
			}
			
			index.serialize(index_path);
			return 0;
		}

		if (args[0] == "find"){
			bool has_query = false;
			string query;

			for(size_t i = 0; i < args.size(); i++){
				if (args[i] == "-q" || args[i] == "--query"){
					if(i + 1 < args.size()){
						query = args[i + 1];
						has_query = true;
						i++;
					}	
					else{
						cerr << "Error: query was not provided" << endl;
						return 1;
					}
				}
				else if (args[i] == "-s" || args[i] == "--source-index"){
					if(i + 1 < args.size()){
						index_path = args[i + 1];
						i++;
					}
					else{
						cerr << "Error: source index path was not provided" << endl;
						return 1;
					}
				}
			
			}

			
			Index index;
			index.deserialize(index_path);
			Searcher searcher(index);

			if (!has_query){
				cerr << "Error: -q flag must be specified" << endl;
				return 1;
			}

			
			vector<SearchResult> results = searcher.search(query);
			for(const auto result: results){
				cout << format("file {}", result.file_path) << endl
				<< format("line {}", result.line) << endl
				<< format("context:\n {}", result.result) << endl << endl;
			}
			
			if (results.empty()) {
                cout << "No results were found" << endl;
            }
			return 0;
		}


		cout << "Error: invalid option" << endl;
		return 1;
	}
	catch(exception& ex){
		cerr << "Exception: " << ex.what() << endl;
	}
}


