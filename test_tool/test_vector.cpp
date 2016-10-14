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

#include <noname_tools\tools>

#include "catch.hpp"

#include <string>
#include <type_traits>
#include <vector>

using namespace noname;

TEST_CASE("Testing sorted_vector")
{
	SECTION("Call with initializer list")
	{
		const auto sortedVector = tools::sorted_vector<std::string>({ "ccc","bbb","aaa" });

		REQUIRE((std::is_same<decltype(sortedVector), const std::vector<std::string>>::value) == true);
		REQUIRE(sortedVector.size() == 3);
		REQUIRE(sortedVector.at(0) == "aaa");
		REQUIRE(sortedVector.at(1) == "bbb");
		REQUIRE(sortedVector.at(2) == "ccc");
	}

	SECTION("Call with r-value vector")
	{
		const auto sortedVector = tools::sorted_vector<std::string>(std::vector<std::string>({ "ccc","bbb","aaa" }));

		REQUIRE((std::is_same<decltype(sortedVector), const std::vector<std::string>>::value) == true);
		REQUIRE(sortedVector.size() == 3);
		REQUIRE(sortedVector.at(0) == "aaa");
		REQUIRE(sortedVector.at(1) == "bbb");
		REQUIRE(sortedVector.at(2) == "ccc");
	}

	SECTION("Call with empty r-value vector")
	{
		const auto sortedVector = tools::sorted_vector<std::string>(std::vector<std::string>());

		REQUIRE((std::is_same<decltype(sortedVector), const std::vector<std::string>>::value) == true);
		REQUIRE(sortedVector.size() == 0);
	}
}
