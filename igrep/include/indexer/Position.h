#pragma once

#include<string>
namespace igrep::indexer {

	struct Position
	{
		std::string filename;
		size_t line_number;
		size_t indent;
		size_t word_index;

		bool operator==(const Position& other) const{
			return filename == other.filename &&
				   line_number == other.line_number &&
				   indent == other.indent &&
				   word_index == other.word_index; 
		}
	};

}