//	MIT License
//
//	Copyright (c) 2016 Fabian Löschner
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.

#pragma once

#include <string>
#include <vector>

namespace noname
{
	namespace tools
	{
		//! Truncates a string at the first occurrence of the specified character or returns the full string if the character was not found
		template <typename StringT, typename CharT>
		StringT truncate_string(const StringT& str, CharT ch)
		{
			auto pos = str.find_first_of(ch);
			return ((pos != std::string::npos) ? str.substr(0, pos) : str);
		}

		//! Returns a vector of substrings of the original string, split at every occurrence of the specified character
		template <typename StringT, typename CharT>
		std::vector<StringT> split_string(const StringT& str, CharT ch)
		{
			std::vector<StringT> strings;
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
