#pragma once

#include<string>
#include<cstdint>
namespace igrep::indexer {

	struct Position
	{
		uint32_t file_id;
		uint32_t line_number;
		uint32_t indent;
		uint32_t word_index;

		bool operator==(const Position& other) const{
			return file_id == other.file_id &&
				   line_number == other.line_number &&
				   indent == other.indent &&
				   word_index == other.word_index; 
		}
	};

}