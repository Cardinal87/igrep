// igrep.cpp: определяет точку входа для приложения.
//

#include "igrep.h"

using namespace std;
using namespace std::filesystem;
using namespace igrep::indexer;
using namespace igrep::searcher;

int main(int argc, char* argv[])
{
	vector<string> args(argv + 1, argv + argc);

	if (args.empty()
	|| find(args.begin(), args.end(), "-v") != args.end()
	|| find(args.begin(), args.end(), "--version") != args.end()){
		cout << "igrep 1.0.0" << endl << "Try 'igrep -h' to get more information";
		return 0;
	}
	if (find(args.begin(), args.end(), "-h") != args.end()
	|| find(args.begin(), args.end(), "--help") != args.end()){
		cout << "Usage: igrep [OPTION] [PARAMS]" << endl 
		<< "Indexing files:" << endl
		<< " Index new file: 'igrep save [PARAMS]" << endl
		<< " Require one of the next flags:" << endl
		<< "\t -f, --file\t specify path to file " << endl
		<< "\t -d, --directory\t specify directory for recursive indexing" << endl
		<< "Find query" << endl
		<< " Usage: 'igrep find [PARAMS]" << endl
		<< " Require one of the next flags:" << endl
		<< "\t -q, --query\t specify string you want to find" << endl;

		return 0;
	}

	if (args[0] == "save"){
		FileIndexer indexer;
		indexer.load_index();
		
		for(size_t i = 0; i < args.size(); i++){
			if (args[i] == "-f" || args[i] == "--file"){
				if(i + 1 < args.size()){
					path filepath = args[i + 1];
					indexer.index_file(filepath);
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

	if (args[0] == "find"){
		FileIndexer indexer;
		indexer.load_index();

		Searcher searcher(indexer.get_index());
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


