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

#include <noname_tools/tools>

#include "catch.hpp"

#include <string>
#include <utility>
#include <type_traits>

using namespace noname;
using namespace std::string_literals;

TEST_CASE("Testing optional")
{
	typedef tools::optional<std::string> op_type;

	REQUIRE((std::is_same<std::string, op_type::value_type>::value));
	REQUIRE(sizeof(op_type) == sizeof(std::aligned_storage_t<sizeof(bool) + sizeof(std::string), alignof(std::string)>));

	// Empty state
	{
		op_type op;

		REQUIRE(static_cast<bool>(op) == false);
		REQUIRE(op.has_value() == false);
		REQUIRE(!op);

		// Visit with empty optional
		{
			bool exceptionThrown = false;
			try {
				op.value();
			} catch (tools::bad_optional_access) {
				exceptionThrown = true;
			}
			REQUIRE(exceptionThrown);
		}

		// Visit with empty const optional
		{
			const op_type& cop = op;
			bool exceptionThrown = false;
			try {
				cop.value();
			} catch (tools::bad_optional_access) {
				exceptionThrown = true;
			}
			REQUIRE(exceptionThrown);
		}

		// Visit with empty r-value optional
		{
			bool exceptionThrown = false;
			try {
				op_type().value();
			} catch (tools::bad_optional_access) {
				exceptionThrown = true;
			}
			REQUIRE(exceptionThrown);
		}

		// Construct using nullopt_t
		{
			op_type op_n(tools::nullopt);

			REQUIRE(static_cast<bool>(op) == false);
			REQUIRE(op.has_value() == false);
			REQUIRE(!op);
		}
	}

	// Construction with l-value of value_type
	{
		{
			std::string string("Test_String");
			op_type op(string);

			// Non const
			REQUIRE(static_cast<bool>(op) == true);
			REQUIRE(op.has_value() == true);
			REQUIRE(op);
			REQUIRE(*op == string);
			REQUIRE(op->compare(string) == 0);
			REQUIRE(op.value() == string);
		}

		{
			std::string string("Test_String");
			const op_type cop(string);

			// Const
			REQUIRE(static_cast<bool>(cop) == true);
			REQUIRE(cop.has_value() == true);
			REQUIRE(*cop == string);
			REQUIRE(cop->compare(string) == 0);
			REQUIRE(cop.value() == string);
		}
	}

	// Construction with r-value of value_type
	{
		{
			op_type op("Test_String"s);

			// Non const
			REQUIRE(static_cast<bool>(op) == true);
			REQUIRE(op.has_value() == true);
			REQUIRE(*op == "Test_String"s);
			REQUIRE(op->compare("Test_String"s) == 0);
		}

		{
			const op_type cop("Test_String"s);

			// Const
			REQUIRE(static_cast<bool>(cop) == true);
			REQUIRE(cop.has_value() == true);
			REQUIRE(*cop == "Test_String"s);
			REQUIRE(cop->compare("Test_String"s) == 0);
		}
	}

	// r-value optional
	{
		std::string string("Test_String");

		REQUIRE(op_type().has_value() == false);
		REQUIRE(op_type(string).has_value() == true);
		REQUIRE(op_type("Test_String"s).has_value() == true);

		REQUIRE(*(static_cast<const op_type>(op_type(string))) == string);
		REQUIRE(*op_type(string) == string);
		REQUIRE(op_type(string).value() == string);
	}

	// Construction with l-value of optional with value
	{
		std::string string("Test_String");
		op_type op1(string);
		op_type op2(op1);

		REQUIRE(op2.has_value() == true);
		REQUIRE(*op2 == *op1);
	}

	// Construction with l-value of empty optional
	{
		op_type op1;
		op_type op2(op1);

		REQUIRE(op2.has_value() == false);
	}

	// Construction with r-value of empty optional
	{
		op_type op(std::move(op_type()));

		REQUIRE(op.has_value() == false);
	}

	// Construction with in_place_t
	{
		// In-place constructor
		{
			const std::string string1("Test_String1");
			op_type op(tools::in_place, string1, 0, 4);

			REQUIRE(op.has_value() == true);
			REQUIRE(op.value() == "Test"s);
		}

		// In-place initializer list
		{
			op_type op(tools::in_place, { 'T', 'e', 's', 't' });

			REQUIRE(op.has_value() == true);
			REQUIRE(op.value() == "Test"s);
		}
	}

	// Assignment
	{
		const std::string string1("Test_String1");
		const std::string string2("Test_String2");

		// Assignemnt with nullopt_t
		{
			{
				op_type op(string1);
				op = tools::nullopt;

				REQUIRE(op.has_value() == false);
			}

			{
				op_type op(string1);
				op = {};

				REQUIRE(op.has_value() == false);
			}

			{
				op_type op;
				op = {};

				REQUIRE(op.has_value() == false);
			}
		}

		// Assignment from other l-value optional to empty optional
		{
			op_type op1;
			const op_type op2(string2);

			op1 = op2;
			REQUIRE(op1.has_value() == true);
			REQUIRE(op2.has_value() == true);
			REQUIRE(op1.value() == string2);
			REQUIRE(op2.value() == string2);
		}

		// Assignment from other l-value optional
		{
			op_type op1(string1);
			const op_type op2(string2);

			op1 = op2;
			REQUIRE(op1.has_value() == true);
			REQUIRE(op2.has_value() == true);
			REQUIRE(op1.value() == string2);
			REQUIRE(op2.value() == string2);
		}

		// Assignment from empty l-value optional
		{
			op_type op1(string1);
			const op_type op2;

			op1 = op2;
			REQUIRE(op1.has_value() == false);
			REQUIRE(op2.has_value() == false);
		}

		// Assignment from r-value optional to empty optional
		{
			op_type op1;

			op1 = op_type(string2);
			REQUIRE(op1.has_value() == true);
			REQUIRE(op1.value() == string2);
		}

		// Assignment from r-value optional
		{
			op_type op1(string1);

			op1 = op_type(string2);
			REQUIRE(op1.has_value() == true);
			REQUIRE(op1.value() == string2);
		}

		// Assignment from empty r-value optional
		{
			op_type op1(string1);

			op1 = op_type();
			REQUIRE(op1.has_value() == false);
		}

		// Assignment from l-value to empty optional
		{
			op_type op1;

			op1 = string2;
			REQUIRE(op1.has_value() == true);
			REQUIRE(op1.value() == string2);
		}

		// Assignment from l-value
		{
			op_type op1(string1);

			op1 = string2;
			REQUIRE(op1.has_value() == true);
			REQUIRE(op1.value() == string2);
		}

		// Assignment from r-value to empty optional
		{
			op_type op1;

			op1 = std::move(std::string(string2));
			REQUIRE(op1.has_value() == true);
			REQUIRE(op1.value() == string2);
		}

		// Assignment from r-value
		{
			op_type op1(string1);

			op1 = std::move(std::string(string2));
			REQUIRE(op1.has_value() == true);
			REQUIRE(op1.value() == string2);
		}
	}

	// Test value_or
	{
		std::string string1("Test_String1");
		std::string string2("Test_String2");

		// Use default value
		{
			op_type op;
			REQUIRE(op.value_or(string1) == string1);
			REQUIRE(op.value_or("Test_String1"s) == "Test_String1"s);
		}

		// Use optional value
		{
			op_type op(string2);
			REQUIRE(op.value_or(string1) == string2);
		}

		// r-value optional
		{
			REQUIRE(op_type().value_or(string1) == string1);
			REQUIRE(op_type(string2).value_or(string1) == string2);
		}

		// Const optional and default value
		{
			const op_type op;
			const std::string& cstring1 = string1;

			REQUIRE(op.value_or(cstring1) == string1);
		}
	}

	// Test swap
	{
		const std::string string1("Test_String1");
		const std::string string2("Test_String2");

		// Both have value
		{
			op_type op1(string1);
			op_type op2(string2);
			op1.swap(op2);

			REQUIRE(op1.has_value() == true);
			REQUIRE(op2.has_value() == true);
			REQUIRE(op1.value() == string2);
			REQUIRE(op2.value() == string1);
		}

		// Swapped to has value
		{
			op_type op1(string1);
			op_type op2;
			op1.swap(op2);

			REQUIRE(op1.has_value() == false);
			REQUIRE(op2.has_value() == true);
			REQUIRE(op2.value() == string1);
		}

		// Swapped from has value
		{
			op_type op1;
			op_type op2(string2);
			op1.swap(op2);

			REQUIRE(op1.has_value() == true);
			REQUIRE(op2.has_value() == false);
			REQUIRE(op1.value() == string2);
		}
	}

	// Test reset
	{
		const std::string string("Test_String1");
		op_type op(string);

		op.reset();
		REQUIRE(op.has_value() == false);
	}

	// Test emplace
	{
		const std::string string("Test_String1");

		// Test with empty optional
		{
			op_type op;
			op.emplace(string, 0, 4);

			REQUIRE(op.value() == string.substr(0, 4));
		}

		// Test with non-empty optional
		{
			op_type op(string);
			op.emplace(string, 0, 4);

			REQUIRE(op.value() == string.substr(0, 4));
		}

		// Test with empty optional and initializer list
		{
			op_type op;
			op.emplace({ 'T', 'e', 's', 't' });

			REQUIRE(op.has_value() == true);
			REQUIRE(op.value() == "Test"s);
		}

		// Test with non-empty optional and initializer list
		{
			op_type op(string);
			op.emplace({ 'T', 'e', 's', 't' });

			REQUIRE(op.has_value() == true);
			REQUIRE(op.value() == "Test"s);
		}
	}

	// Test make_optional
	{
		// r-value
		{
			const auto op = tools::make_optional("Test_String"s);

			REQUIRE((std::is_same<decltype(op), const op_type>::value));
			REQUIRE(op.has_value() == true);
			REQUIRE(op.value() == "Test_String"s);
		}

		// Constructor arguments
		{
			const auto op = tools::make_optional<std::string>("Test_String"s, 0, 4);

			REQUIRE((std::is_same<decltype(op), const op_type>::value));
			REQUIRE(op.has_value() == true);
			REQUIRE(op.value() == "Test"s);
		}

		// Initializer list
		{
			const auto op = tools::make_optional<std::string>({ 'T','e','s','t' });

			REQUIRE((std::is_same<decltype(op), const op_type>::value));
			REQUIRE(op.has_value() == true);
			REQUIRE(op.value() == "Test"s);
		}
	}

	// Test pointer as value type
	{
		tools::optional<std::string*> test;

		REQUIRE(test.has_value() == false);

		std::string str1("Test1");
		std::string str2("Test2");

		test = &str1;
		REQUIRE(test.has_value() == true);
		REQUIRE(*test == &str1);
		REQUIRE(**test == str1);
		REQUIRE((*test)->compare("Test1"s) == 0);

		test = &str2;
		REQUIRE((*test)->compare("Test2"s) == 0);

		str2 = "Test3"s;
		REQUIRE((*test)->compare("Test3"s) == 0);
	}

	// Test array as value type
	{
		int is[5] = { 0,1,2,3,4 };
		auto op = tools::make_optional(is);

		REQUIRE(op.has_value() == true);
		REQUIRE(op.value()[4] == 4);
		REQUIRE((*op)[3] == 3);
	}

	// Test constexpr features
	{
		using op_double = tools::optional<double>;
		constexpr double value = 3.14;
		constexpr op_double test1(value);
		constexpr op_double test2;

		static_assert(test2.has_value() == false, "Optional has to be empty after default construction.");
		static_assert(test2.value_or(2) == 2, "Optional has to return the other value after default construction.");
		static_assert(test1.has_value() == true, "Optional has to contain a value after calling the constructor with a value.");
		static_assert(test1.value() == value, "Optional has to contain the supplied value after calling the constructor with the value.");
	}
}
