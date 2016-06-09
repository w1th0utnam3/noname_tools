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

#include <vector>
#include <string>
#include <fstream>

namespace noname
{
	namespace tools
	{
		// TODO: Make methods templated in string type

		//! Reads all lines from the specified file to a vector
		inline std::vector<std::string> read_all_lines(const std::string& file_path)
		{
			std::vector<std::string> lines;
			std::string currentLine;
			std::ifstream file(file_path);
			while (std::getline(file, currentLine)) lines.push_back(currentLine);
			file.close();
			return lines;
		}

		//! Reads the specified number of lines from a file or reads the whole file if number of lines is zero
		inline std::vector<std::string> read_lines(const std::string& file_path, size_t number_of_lines = 0)
		{
			if (number_of_lines == 0) return read_all_lines(file_path);

			std::vector<std::string> lines;
			lines.reserve(number_of_lines);

			std::string currentLine;
			std::ifstream file(file_path);
			size_t counter = 0;
			while (counter < number_of_lines && std::getline(file, currentLine)) {
				lines.push_back(currentLine);
				number_of_lines++;
			}
			file.close();
			return lines;
		}
	}
}
