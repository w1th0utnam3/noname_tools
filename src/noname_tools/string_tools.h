#pragma once

#include <string>
#include <vector>

namespace noname
{
	namespace tools
	{
		//! Truncates a string at the first occurance of the specified character or returns the full string if the character was not found
		template <typename StrT, typename CharT>
		StrT truncate_string(const StrT& str, CharT ch)
		{
			auto pos = str.find_first_of(ch);
			return ((pos != std::string::npos) ? str.substr(0, pos) : str);
		}

		//! Returns a vector of substrings of the original string, split at every occurance of the specified character
		template <typename StrT, typename CharT>
		std::vector<StrT> split_string(const StrT& str, CharT ch)
		{
			std::vector<StrT> strings;
			const auto size = str.size();
			auto start = decltype(size)(0);
			auto count = str.find_first_of(ch);
			while (count != std::string::npos) {
				count -= start;
				strings.emplace_back(str.substr(start, count));
				start += count + 1;
				count = str.find_first_of(ch, start);
			}
			strings.emplace_back(str.substr(start, size - start));
			return strings;
		}
	}
}
