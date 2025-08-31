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
			cout << "igrep 1.0.0" << endl << "Try 'igrep -h' to get more information";
			return 0;
		}
		if (find(args.begin(), args.end(), "-h") != args.end()
		|| find(args.begin(), args.end(), "--help") != args.end()){
			cout << "Usage: 'igrep [OPTION] [PARAMS]'" << endl 
			<< "Indexing files:" << endl
			<< " Index new file: 'igrep save [PARAMS]'" << endl
			<< " Require one of the next flags:" << endl
			<< "\t -f, --file\t specify path to file " << endl
			<< "\t -d, --directory\t specify directory for recursive indexing" << endl
			<< " Delete file from index: 'igrep remove [PARAMS]'" << endl
			<< " Require one of the next flags:" << endl
			<< "\t -f, --file\t specify file path (file does not have to exist)"
			<< "Find queries" << endl
			<< " Usage: 'igrep find [PARAMS]" << endl
			<< " Require one of the next flags:" << endl
			<< "\t -q, --query\t specify string you want to find" << endl;

			return 0;
		}

		if (args[0] == "save"){
			Index index;
			index.deserialize(index_path);
			FileIndexer indexer(index);
			
			
			for(size_t i = 0; i < args.size(); i++){
				if (args[i] == "-f" || args[i] == "--file"){
					if(i + 1 < args.size()){
						path filepath = args[i + 1];
						indexer.index_file(filepath);
						index.serialize(index_path);
						cout << "File was successfully indexed";
						return 0;
					}
					else{
						cerr << "Error: file path was not provided";
						return 1;
					}
				}
				else if (args[i] == "-d" || args[i] == "--directory"){
					if(i + 1 < args.size()){
						path dirpath = args[i + 1];
						indexer.index_directory(dirpath);
						index.serialize(index_path);
						cout << "Directory was successfully indexed";
						return 0;
					}
					else{
						cerr << "Error: directory path was not provided";
						return 1;
					}
				}
			}

			cerr << "Error: no parameters were provided";
			return 1;
		}

		if(args[0] == "remove"){
			Index index;
			index.deserialize(index_path);

			for(size_t i = 0; i < args.size(); i++){
				if (args[i] == "-f" || args[i] == "--file"){
					if(i + 1 < args.size()){
						path filepath = args[i + 1];
						index.remove_file(filepath);
						index.serialize(index_path);
						cout << "File was successfully removed from index";
						return 0;
					}
					else{
						cerr << "Error: file path was not provided";
						return 1;
					}
				}
			}
			cerr << "Error: no parameters were provided";
			return 1;
		}

		if (args[0] == "find"){
			Index index;
			index.deserialize(index_path);

			Searcher searcher(index);
			for(size_t i = 0; i < args.size(); i++){
				if (args[i] == "-q" || args[i] == "--query"){
					if(i + 1 < args.size()){
						const string query = args[i + 1];
						vector<SearchResult> results = searcher.search(query);
						for(const auto result: results){
							cout << format("file {}", result.file_path) << endl
							<< format("line {}", result.line) << endl
							<< format("context {}", result.result) << endl << endl;
						}
						return 0;
					}	
					else{
						cerr << "Error: query was not provided";
						return 1;
					}
				}
			
			}
			cerr << "Error: no params were provided";
			return 1;
		}


		cout << "Error: invalid option";
		return 1;
	}
	catch(exception& ex){
		cerr << "Exception:" << ex.what() << endl;
	}
}


