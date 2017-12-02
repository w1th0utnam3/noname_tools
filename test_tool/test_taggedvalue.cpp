//	MIT License
//
//	Copyright (c) 2017 Fabian L�schner
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

#include <noname_tools/tools>

#include "catch.hpp"

using namespace noname;

TEST_CASE("Testing tagged_value")
{
	struct tag_i {};
	struct tag_d {};
	struct tag_s {};

	using tagged_int = tools::tagged_value<tag_i, int>;
	using tagged_double = tools::tagged_value<tag_d, double>;
	using tagged_string = tools::tagged_value<tag_s, std::string>;

	const std::string long_string = "Hello World! Hello World! Hello World! Hello World!";

	SECTION("Making static tests")
	{
		static_assert(std::is_same<tagged_int::tag_type, tag_i>::value, "Tag type has to be defined properly");
		static_assert(std::is_same<tagged_int::value_type, int>::value, "Value type has to be defined properly");

		static_assert(std::is_same<tagged_double::tag_type, tag_d>::value, "Tag type has to be defined properly");
		static_assert(std::is_same<tagged_double::value_type, double>::value, "Value type has to be defined properly");
	}

	SECTION("Making constexpr tests")
	{
		constexpr tagged_double tv1(3.14);
		constexpr tagged_double tv2(27.11);
		constexpr tagged_double tv3(tv2);

		static_assert(tv1.value == 3.14, "Constexpr construction must work");
		static_assert(tv2.value == 27.11, "Constexpr construction must work");
		static_assert(tv3.value == 27.11, "Constexpr construction must work");
	}

	SECTION("Testing copy/move constructor")
	{
		tagged_string tv1(long_string);
		tagged_string tv2(std::string("Test123"));
		tagged_string tv3("Test123");
		tagged_string tv4(tv1);
		tagged_string tv5((tagged_string(long_string)));

		REQUIRE(tv1.value == long_string);
		REQUIRE(tv2.value == "Test123");
		REQUIRE(tv3.value == "Test123");
		REQUIRE(tv4.value == tv1.value);
		REQUIRE(tv5.value == tv1.value);
	}

	SECTION("Testing access operators")
	{
		tagged_string tv1(long_string);

		REQUIRE(*tv1 == long_string);
		REQUIRE(tv1->size() == long_string.size());
	}

	SECTION("Testing implicit conversions")
	{
		tagged_string tv1(long_string);

		// Const ref to non const
		{
			bool works = false;
			[&works, &long_string](const std::string& s) { works = (s == long_string); }(tv1);
			REQUIRE(works == true);
		}

		// Ref to non const
		{
			[](std::string& s) { s = "Hello"; }(tv1);
			REQUIRE(tv1.value == "Hello");
		}

		// Const ref to const
		{
			const tagged_string tv2(long_string);

			bool works = false;
			[&works, &long_string](std::string s) { works = s == long_string; }(tv2);
		}
	}

	SECTION("Testing comparisons")
	{
		tagged_int tv1(42);

		// Comparison with int (== T)
		REQUIRE(tv1 == 42);
		REQUIRE(42 == tv1);
		REQUIRE(tv1 != 43);
		REQUIRE(43 != tv1);
		REQUIRE(tv1 > 41);
		REQUIRE(tv1 < 43);
		REQUIRE(43 > tv1);
		REQUIRE(41 < tv1);

		// Comparison with double (!= T)
		REQUIRE(tv1 == 42.0);
		REQUIRE(42.0 == tv1);
		REQUIRE(tv1 != 43.0);
		REQUIRE(43.0 != tv1);
		REQUIRE(tv1 > 41.0);
		REQUIRE(tv1 < 43.0);
		REQUIRE(43.0 > tv1);
		REQUIRE(41.0 < tv1);
	}

	SECTION("Testing make_tagged_value")
	{
		auto tv1 = tools::make_tagged_value<tag_i>(42);
		static_assert(std::is_same<decltype(tv1), tagged_int>::value, "make_tagged_value has to return the correct type");

		REQUIRE(tv1 == 42);

		auto tv2 = tools::make_tagged_value<tag_s>(std::string(long_string));
		static_assert(std::is_same<decltype(tv2), tagged_string>::value, "make_tagged_value has to return the correct type");

		REQUIRE(tv2 == long_string);
	}

	SECTION("Testing swap")
	{
		tagged_string tv1(long_string);
		tagged_string tv2("Test123");

		tv1.swap(tv2);

		REQUIRE(tv1 == "Test123");
		REQUIRE(tv2 == long_string);

		tv2.swap(tv1);

		REQUIRE(tv1 == long_string);
		REQUIRE(tv2 == "Test123");
	}
}