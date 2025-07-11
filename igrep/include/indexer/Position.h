#pragma once

#include<string>
namespace include::indexer {

	struct Position
	{
		const std::string filename;
		const int line_number;
		const int indent;
	};

}