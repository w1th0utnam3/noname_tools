#pragma once

#include <string>

namespace noname {

	namespace tools {

		//! Truncates a string at the first occurance of char ch or returns the full string
		template <typename T>
		std::string truncate_at_first(const std::string& str, T ch)
		{
			auto pos = str.find_first_of(" ");
			return ((pos != std::string::npos) ? str.substr(0, pos) : str);
		}

	}

}
