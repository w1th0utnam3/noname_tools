#pragma once

#include <vector>
#include <string>
#include <fstream>

namespace noname
{
	namespace tools
	{
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
