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

#include <noname_tools/range_tools.h>

#include "catch2/catch.hpp"

#include <vector>
#include <type_traits>

using namespace noname;

TEST_CASE("Testing iterator_range")
{
	// Value type for the test container
	typedef std::size_t value_t;

	// Empty source container
	std::vector<value_t> source;
	REQUIRE(source.size() == 0);

	SECTION("Default version")
	{
		auto range = tools::iterator_range<decltype(source)::iterator>(source.begin(), source.end());

		REQUIRE((std::is_same<decltype(range.begin()), decltype(source)::iterator>::value) == true);
		REQUIRE((std::is_same<decltype(range.end()), decltype(source)::iterator>::value) == true);

		REQUIRE(range.begin() == source.begin());
		REQUIRE(range.end() == source.end());
		REQUIRE(range.size() == source.size());

		source.emplace_back();
		source.emplace_back();
		range = tools::iterator_range<decltype(source)::iterator>(source.begin(), source.end());

		REQUIRE(range.size() == source.size());
	}

	SECTION("Sentinel version")
	{
		const int i = 42;
		const auto range = tools::iterator_range<int, double>(i, 47.11);

		REQUIRE((std::is_same<decltype(range.begin()), int>::value) == true);
		REQUIRE((std::is_same<decltype(range.end()), double>::value) == true);

		REQUIRE(range.begin() == i);
		REQUIRE(range.end() == 47.11);
	}

	SECTION("Call to make_range")
	{
		const int i = 42;
		const auto range = tools::make_range(i, 47.11f);

		REQUIRE((std::is_same<decltype(range), const tools::iterator_range<int, float>>::value) == true);

		REQUIRE(range.begin() == i);
		REQUIRE(range.end() == 47.11f);
	}

	SECTION("Call to make_range with range size")
	{
		static const int int_array[] = { 0, 1, 2, 3 };
		static const auto arr_length = sizeof(int_array) / sizeof(int);

		const auto range = tools::make_range_sized(&int_array[0], arr_length);

		REQUIRE(*range.begin() == 0);
		REQUIRE(*std::prev(range.end()) == 3);

		int sum = 0;
		for (int i : range) sum += i;

		REQUIRE(sum == 6);
	}

	SECTION("Call to make_range with array")
	{
		static const int int_array[] = { 0, 1, 2, 3, 4 };

		const auto range = tools::make_range(int_array);

		REQUIRE(*range.begin() == 0);
		REQUIRE(*std::prev(range.end()) == 4);

		int sum = 0;
		for (int i : range) sum += i;

		REQUIRE(sum == 10);
	}

	SECTION("Call to make_range with array of arrays")
	{
		static const int int_array[][2] = { { 1, 2 }, { 3, 4 }, { 5, 6 } };

		const auto range = tools::make_range(int_array);

		REQUIRE((*range.begin())[0] == 1);
		REQUIRE((*std::prev(range.end()))[0] == 5);

		int sum = 0;
		for (const auto& arr : range) {
			for (int i = 0; i < 2; i++) {
				sum += arr[i];
			}
		}

		REQUIRE(sum == 21);
	}
}
