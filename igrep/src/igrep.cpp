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
			cout << "igrep 1.0.0" << endl << "Try 'igrep -h' to get more information" << endl;
			return 0;
		}
		if (find(args.begin(), args.end(), "-h") != args.end()
		|| find(args.begin(), args.end(), "--help") != args.end()){
			const int flagWidth = 20; 
			const int descWidth = 30; 
			
			cout << "Usage: igrep [OPTION] [PARAMS]\n\n"

				<< "CREATING INDEX:\n"
				<< "  Usage: igrep create [PARAMS]\n"
				<< "  Flags:\n"
				<< "    " << left << setw(flagWidth) << "-d, --destination"
					<< setw(descWidth) << "Specify path for index file " + format("(default: {}", index_path.string()) << "\n"
				<< "  Exmaples:\n"
				<< "    igrep create\n"
				<< "    igrep create -d ~/myindex.bin\n\n"

				<< "INDEXING FILES:\n"
				<< "  Usage: igrep index [PARAMS]\n"
				<< "  Flags:\n"
				<< "    " << left << setw(flagWidth) << "-f, --file" 
					<< setw(descWidth) << "Specify path to file" << "\n"
				<< "    " << left << setw(flagWidth) << "-d, --directory" 
					<< setw(descWidth) << "Specify directory for recursive indexing" << "\n"
				<< "  Examples:\n"
				<< "    igrep index -f ./mylog.txt\n"
				<< "    igrep index -d ./logs\n\n"
				
				<< "DELETE FILES FROM INDEX:\n"
				<< "  Usage: igrep remove [PARAMS]\n"
				<< "  Flags:\n"
				<< "    " << left << setw(flagWidth) << "-f, --file" 
					<< setw(descWidth) << "Specify file path (file does not have to exist)" << "\n"
				<< "  Examples:\n"
				<< "    igrep remove -f ./old-log.txt\n\n"
				
				<< "FIND QUERIES:\n"
				<< "  Usage: igrep find [PARAMS]\n"
				<< "  Flags:\n"
				<< "    " << left << setw(flagWidth) << "-q, --query" 
					<< setw(descWidth) << "Specify string you want to find" << "\n"
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
								 << "File must have .bin format";
						}
						index_path = filepath;
						index.serialize(index_path);
						cout << format("Index was successfully created at {}", index_path.string());
						return 0;
					}
					else{
						cerr << "Error: file path was not provided" << endl;
					}
				}
			}
		}


		if (args[0] == "index"){
			Index index;
			index.deserialize(index_path);
			FileIndexer indexer(index);
			
			
			for(size_t i = 0; i < args.size(); i++){
				if (args[i] == "-f" || args[i] == "--file"){
					if(i + 1 < args.size()){
						path filepath = args[i + 1];
						bool is_success = indexer.index_file(filepath);
						index.serialize(index_path);
						if (is_success){
							cout << "File was successfully indexed";
						} else{
							cout << "File has already been indexed";
						}
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
						bool is_success = index.remove_file(filepath);
						index.serialize(index_path);
						if (is_success){
							cout << "File was successfully removed from index";
						} else{
							cout << format("{} was not indexed", filepath.string());
						}
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
		cerr << "Exception: " << ex.what() << endl;
	}
}


