#pragma once

#include<string>
namespace igrep::indexer {

	struct Position
	{
		const std::string filename;
		const int line_number;
		const int indent;
		const int word_index;

		bool operator==(const Position& other) const{
			return filename == other.filename &&
				   line_number == other.line_number &&
				   indent == other.indent &&
				   word_index == other.word_index; 
		}
	};

}