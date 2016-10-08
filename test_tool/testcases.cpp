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

#include "catch.hpp"

#include <noname_tools\tools>

#include <numeric>
#include <functional>
#include <type_traits>
#include <vector>
#include <tuple>

using namespace noname;
using namespace std::string_literals;

// TODO: Tests for strict_unique_copy
// TODO: Tests for truncate_string
// TODO: Tests for split_string

TEST_CASE("Testing sorted_vector")
{
	SECTION("Call with initializer list") {
		const auto sortedVector = tools::sorted_vector<std::string>({ "ccc","bbb","aaa" });

		REQUIRE((std::is_same<decltype(sortedVector), const std::vector<std::string>>::value) == true);
		REQUIRE(sortedVector.size() == 3);
		REQUIRE(sortedVector.at(0) == "aaa");
		REQUIRE(sortedVector.at(1) == "bbb");
		REQUIRE(sortedVector.at(2) == "ccc");
	}

	SECTION("Call with r-value vector") {
		const auto sortedVector = tools::sorted_vector<std::string>(std::vector<std::string>({ "ccc","bbb","aaa" }));

		REQUIRE((std::is_same<decltype(sortedVector), const std::vector<std::string>>::value) == true);
		REQUIRE(sortedVector.size() == 3);
		REQUIRE(sortedVector.at(0) == "aaa");
		REQUIRE(sortedVector.at(1) == "bbb");
		REQUIRE(sortedVector.at(2) == "ccc");
	}

	SECTION("Call with empty r-value vector") {
		const auto sortedVector = tools::sorted_vector<std::string>(std::vector<std::string>());

		REQUIRE((std::is_same<decltype(sortedVector), const std::vector<std::string>>::value) == true);
		REQUIRE(sortedVector.size() == 0);
	}
}

TEST_CASE("Testing n_subranges")
{
	// TODO: Test with std::list or other container
	// TODO: Test with n > length

	// Number of subranges to divide source range into
	const int n = 3;
	// Minimum number of elements per subrange
	const int factor = 5;

	// Type used for source vector
	typedef size_t value_t;
	// Source vector
	std::vector<value_t> source;
	// Vector of output of n_subranges
	std::vector<decltype(source)::iterator> ranges;

	// Start each test with empty vectors
	REQUIRE(source.size() == 0);
	REQUIRE(ranges.size() == 0);

	SECTION("Input range with: (range length) % n == 0") {
		// Prepare destination vector
		ranges.reserve(n + 1);
		// Prepare source vector
		source.resize(n * factor);
		std::iota(source.begin(), source.end(), 0);

		tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), n);

		REQUIRE(ranges.size() == n + 1);
		REQUIRE(*ranges.begin() == source.begin());
		REQUIRE(ranges.back() == source.end());
		for (size_t i = 0; i < n; i++) {
			REQUIRE(*ranges.at(i) == i*factor);
		}
	}

	SECTION("Input range with: (range length) % n == n-1") {
		ranges.reserve(n + 1);
		source.resize(n * (factor + 1) - 1);
		std::iota(source.begin(), source.end(), 0);

		tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), n);

		REQUIRE(ranges.size() == n + 1);
		REQUIRE(*ranges.begin() == source.begin());
		REQUIRE(ranges.back() == source.end());

		// Reconstruct source vector from subranges
		decltype(source) result;
		result.reserve(source.size());
		for (auto it = ranges.begin(); it != std::prev(ranges.end()); ++it) {
			for (auto it2 = *it; it2 != *std::next(it); ++it2) {
				result.push_back(*it2);
			}
		}

		REQUIRE(result.size() == source.size());
		REQUIRE(result == source);
	}

	SECTION("Empty input range") {
		tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), 1);

		REQUIRE(ranges.size() == 0);
	}

	SECTION("Split into n = 0 ranges") {
		source.resize(n * factor);
		std::iota(source.begin(), source.end(), 0);

		tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), 0);

		REQUIRE(ranges.size() == 0);
	}

	SECTION("Split into n = 1 ranges") {
		source.resize(n * factor);
		std::iota(source.begin(), source.end(), 0);

		tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), 1);

		REQUIRE(ranges.size() == 2);
		REQUIRE(*ranges.begin() == source.begin());
		REQUIRE(ranges.back() == source.end());
	}
}

TEST_CASE("Testing for_each_and_successor")
{
	// Value type for the test container
	typedef size_t value_t;

	// Start with empty source vector
	std::vector<value_t> source;
	REQUIRE(source.size() == 0);

	// Functor that will be applied to all elements
	struct accumulator
	{
		void operator()(const value_t& a, const value_t& b)
		{
			x += a;
			x += b;
		}

		value_t value() const
		{
			return x;
		}

	private:
		value_t x = 0;
	};
	accumulator acc;
	REQUIRE(acc.value() == 0);

	// Make sure that accumulator works as intended
	SECTION("Testing accumulator functor") {
		acc(42, 4711);
		REQUIRE(acc.value() == 4753);
	}

	SECTION("Testing with ascending input range") {
		source = { 0, 1, 2, 3, 4 };

		const auto acc_return = tools::for_each_and_successor(source.begin(), source.end(), acc);

		REQUIRE(acc_return.value() == 16);
	}

	SECTION("Testing with empty input range") {
		const auto acc_return = tools::for_each_and_successor(source.begin(), source.end(), acc);

		REQUIRE(acc_return.value() == 0);
	}

	SECTION("Testing with one elment in input range") {
		source = { 42 };

		const auto acc_return = tools::for_each_and_successor(source.begin(), source.end(), acc);

		REQUIRE(acc_return.value() == 0);
	}
}

TEST_CASE("Testing find_unequal_successor")
{
	// Value type for source container
	typedef size_t value_t;
	// Start with emptry source vector
	std::vector<value_t> source;
	REQUIRE(source.size() == 0);

	SECTION("Using equal operator for comparison") {

		SECTION("Repeated application on returned iterator") {
			source = { 1, 1, 1, 2, 2, 3 };

			auto it = tools::find_unequal_successor(source.begin(), source.end());
			REQUIRE((std::is_same<decltype(it), decltype(source)::iterator>::value) == true);
			REQUIRE(*it == 2);

			it = tools::find_unequal_successor(it, source.end());
			REQUIRE(*it == 3);

			it = tools::find_unequal_successor(it, source.end());
			REQUIRE(it == source.end());
		}

		SECTION("Input range with equal elements") {
			source = { 1, 1, 1 };

			const auto it = tools::find_unequal_successor(source.cbegin(), source.cend());

			REQUIRE((std::is_same<decltype(it), const decltype(source)::const_iterator>::value) == true);
			REQUIRE(it == source.end());
		}

		SECTION("Empty input range") {
			const auto it = tools::find_unequal_successor(source.cbegin(), source.cend());

			REQUIRE((std::is_same<decltype(it), const decltype(source)::const_iterator>::value) == true);
			REQUIRE(it == source.end());
		}

	}

	SECTION("Using predicate for comparison") {

		// Get default equal operator function
		const auto not_equal = std::not_equal_to<decltype(source)::value_type>();

		SECTION("Repeated application on returned iterator") {
			source = { 1, 1, 1, 2, 2, 3 };

			auto it = tools::find_unequal_successor(source.begin(), source.end(), not_equal);
			REQUIRE((std::is_same<decltype(it), decltype(source)::iterator>::value) == true);
			REQUIRE(*it == 2);

			it = tools::find_unequal_successor(it, source.end(), not_equal);
			REQUIRE(*it == 3);

			it = tools::find_unequal_successor(it, source.end(), not_equal);
			REQUIRE(it == source.end());
		}

		SECTION("Input range with equal elements") {
			source = { 1, 1, 1 };

			const auto it = tools::find_unequal_successor(source.cbegin(), source.cend(), not_equal);

			REQUIRE((std::is_same<decltype(it), const decltype(source)::const_iterator>::value) == true);
			REQUIRE(it == source.end());
		}

		SECTION("Empty input range") {
			const auto it = tools::find_unequal_successor(source.cbegin(), source.cend(), not_equal);

			REQUIRE((std::is_same<decltype(it), const decltype(source)::const_iterator>::value) == true);
			REQUIRE(it == source.end());
		}
	}
}

TEST_CASE("Testing iterator_range")
{
	// Value type for the test container
	typedef size_t value_t;

	// Empty source container
	std::vector<value_t> source;
	REQUIRE(source.size() == 0);

	SECTION("Default version") {
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

	SECTION("Sentinel version") {
		const int i = 42;
		const auto range = tools::iterator_range<int, double>(i, 47.11);

		REQUIRE((std::is_same<decltype(range.begin()), int>::value) == true);
		REQUIRE((std::is_same<decltype(range.end()), double>::value) == true);

		REQUIRE(range.begin() == i);
		REQUIRE(range.end() == 47.11);
	}

	SECTION("Call to make_range") {
		const int i = 42;
		const auto range = tools::make_range(i, 47.11f);

		REQUIRE((std::is_same<decltype(range), const tools::iterator_range<int, float>>::value) == true);

		REQUIRE(range.begin() == i);
		REQUIRE(range.end() == 47.11f);
	}
}

TEST_CASE("Testing tuple_for_each")
{
	SECTION("Testing with tuple size of 3 and l-value") {
		std::tuple<int, double, float> tuple{ 2, 42.5, 33.5f };

		std::vector<double> target;
		REQUIRE(target.size() == 0);

		tools::tuple_for_each(tuple, [&target](auto x)
		{
			target.emplace_back(x);
		});

		REQUIRE(target.size() == 3);
		REQUIRE(target.at(0) == 2);
		REQUIRE(target.at(1) == 42.5);
		REQUIRE(target.at(2) == 33.5);
	}

	SECTION("Testing with tuple size of 1 and r-value") {
		std::string source = "SourceString";
		std::string out;

		tools::tuple_for_each(std::make_tuple(source), [&out](const std::string& x)
		{
			out = x;
		});

		REQUIRE(out == source);
	}

	SECTION("Testing return value of functor")
	{
		std::tuple<int, double, float> tuple{ 2, 42.5, 33.5f };

		// Define functor that increments counter on call
		struct Functor
		{
			int i;
			Functor(int i) : i(i) {};
			void operator() (double d) {
				i++;
			}
		};

		// Check preconditions
		const int offset = 10;
		REQUIRE(Functor(offset).i == offset);

		auto f = tools::tuple_for_each(tuple, Functor(offset));

		// Check whether return works
		REQUIRE(f.i == offset + 3);
	}

	SECTION("Testing with pairs")
	{
		std::pair <double, double> pair{ 42,24 };

		std::vector<double> target;
		REQUIRE(target.size() == 0);

		tools::tuple_for_each(pair, [&target](auto x)
		{
			target.emplace_back(x);
		});

		REQUIRE(target.size() == 2);
		REQUIRE(target.at(0) == 42);
		REQUIRE(target.at(1) == 24);
	}

	SECTION("Testing modification of tuple using references")
	{
		std::tuple<int, double, float> tuple{ 2, 42.5, 33.5f };

		// Test preconditions
		REQUIRE(std::get<0>(tuple) != 42);
		REQUIRE(std::get<1>(tuple) != 42);
		REQUIRE(std::get<2>(tuple) != 42);

		tools::tuple_for_each(tuple, [](auto& val)
		{
			val = 42;
		});

		// Check whether modification of tuple values with reference works
		REQUIRE(std::get<0>(tuple) == 42);
		REQUIRE(std::get<1>(tuple) == 42);
		REQUIRE(std::get<2>(tuple) == 42);
	}
}

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
			}
			catch (tools::bad_optional_access) {
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
			}
			catch (tools::bad_optional_access) {
				exceptionThrown = true;
			}
			REQUIRE(exceptionThrown);
		}

		// Visit with empty r-value optional
		{
			bool exceptionThrown = false;
			try {
				op_type().value();
			}
			catch (tools::bad_optional_access) {
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

TEST_CASE("Testing typetraits")
{
	SECTION("Testing void_t")
	{
		// TODO: Test if void_t really works for its intended use case
		REQUIRE((std::is_same<tools::void_t<bool, int, double>, void>::value == true));
	}

	SECTION("Testing bool_constant")
	{
		REQUIRE((tools::bool_constant<true>::value == true));
		REQUIRE((tools::bool_constant<false>::value == false));
	}

	SECTION("Testing negation")
	{
		REQUIRE((tools::negation<std::true_type>::value == false));
		REQUIRE((tools::negation<std::false_type>::value == true));
	}

	SECTION("Testing conjunction")
	{
		REQUIRE((tools::conjunction<std::true_type, std::true_type>::value == true));
		REQUIRE((tools::conjunction<std::false_type, std::true_type>::value == false));
		REQUIRE((tools::conjunction<std::true_type, std::false_type>::value == false));
		REQUIRE((tools::conjunction<std::false_type, std::false_type>::value == false));
		REQUIRE((tools::conjunction<std::true_type, std::true_type, std::true_type>::value == true));
		REQUIRE((tools::conjunction<std::true_type, std::true_type, std::false_type>::value == false));
		REQUIRE((tools::conjunction<std::false_type, std::false_type, std::true_type>::value == false));
		REQUIRE((tools::disjunction<std::false_type, std::false_type, std::false_type>::value == false));
	}

	SECTION("Testing disjunction")
	{
		REQUIRE((tools::disjunction<std::true_type, std::true_type>::value == true));
		REQUIRE((tools::disjunction<std::false_type, std::true_type>::value == true));
		REQUIRE((tools::disjunction<std::true_type, std::false_type>::value == true));
		REQUIRE((tools::disjunction<std::false_type, std::false_type>::value == false));
		REQUIRE((tools::disjunction<std::true_type, std::true_type, std::true_type>::value == true));
		REQUIRE((tools::disjunction<std::true_type, std::true_type, std::false_type>::value == true));
		REQUIRE((tools::disjunction<std::false_type, std::false_type, std::true_type>::value == true));
		REQUIRE((tools::disjunction<std::false_type, std::false_type, std::false_type>::value == false));
	}

	SECTION("Testing is_referenceable")
	{
		REQUIRE((tools::is_referenceable<double>::value == true));
		REQUIRE((tools::is_referenceable<double&>::value == true));
		REQUIRE((tools::is_referenceable<void>::value == false));
	}

	SECTION("Testing is_swappable")
	{
		REQUIRE((tools::is_swappable_with<double, double>::value == true));
		REQUIRE((tools::is_swappable_with<double, void>::value == false));

		REQUIRE((tools::is_swappable<double>::value == true));
		REQUIRE((tools::is_swappable<void>::value == false));
	}
}

TEST_CASE("Testing utility types")
{
	REQUIRE((std::is_same<tools::nth_element_t<0, int, void, double>, int>::value == true));
	REQUIRE((std::is_same<tools::nth_element_t<1, int, void, double>, void>::value == true));
	REQUIRE((std::is_same<tools::nth_element_t<2, int, void, double>, double>::value == true));

	REQUIRE((tools::element_index_v<int, int, double, void> == 0));
	REQUIRE((tools::element_index_v<double, int, double, void> == 1));
	REQUIRE((tools::element_index_v<void, int, double, void> == 2));
	REQUIRE((tools::element_index_v<std::string, int, double, void> == tools::element_not_found));

	REQUIRE((tools::count_element_v<int, void, double, int, int, char, int> == 3));
	REQUIRE((tools::count_element_v<int> == 0));
}

TEST_CASE("Testing variant")
{
	typedef tools::variant<double, int, char, double> var_t;

	SECTION("Testing helper classes")
	{
		REQUIRE((tools::variant_size_v<var_t> == 4));
		REQUIRE((tools::variant_size_v<const var_t> == 4));
		REQUIRE((tools::variant_size_v<volatile var_t> == 4));
		REQUIRE((tools::variant_size_v<const volatile var_t> == 4));

		REQUIRE((std::is_same<tools::variant_alternative_t<0, var_t>, double>::value == true));
		REQUIRE((std::is_same<tools::variant_alternative_t<1, var_t>, int>::value == true));
		REQUIRE((std::is_same<tools::variant_alternative_t<2, var_t>, char>::value == true));
		REQUIRE((std::is_same<tools::variant_alternative_t<3, var_t>, double>::value == true));
		REQUIRE((std::is_same<tools::variant_alternative_t<1, const var_t>, const int>::value == true));
		REQUIRE((std::is_same<tools::variant_alternative_t<1, volatile var_t>, volatile int>::value == true));
		REQUIRE((std::is_same<tools::variant_alternative_t<1, const volatile var_t>, const volatile int>::value == true));
	}

	SECTION("Testing constexpr constructor")
	{
		static_assert(var_t().index() == 0, "Index has to be zero after default construction.");

		static_assert(var_t(tools::in_place<0>, 3.14).index()	== 0, "Index has to be the specified value after in place construction.");
		static_assert(var_t(tools::in_place<1>, 27).index()		== 1, "Index has to be the specified value after in place construction.");
		static_assert(var_t(tools::in_place<2>, 'a').index()	== 2, "Index has to be the specified value after in place construction.");
		static_assert(var_t(tools::in_place<3>, 99.9).index()	== 3, "Index has to be the specified value after in place construction.");

		constexpr const var_t v1(std::move(var_t(tools::in_place<2>, 'a')));
		static_assert(v1.index() == 2, "Index has to be the specified value after move construction.");

		constexpr const var_t v2(v1);
		static_assert(v2.index() == 2, "Index has to be the specified value after copy construction.");
	}

	SECTION("Testing get_if")
	{
		constexpr const var_t* p = nullptr;
		static_assert(tools::get_if<2>(p) == nullptr, "get_if has to return nullptr for nullptr parameter.");

		constexpr const var_t v0(tools::in_place<0>, 3.14);
		constexpr const var_t v1(tools::in_place<1>, 27);
		constexpr const var_t v2(tools::in_place<2>, 'a');
		constexpr const var_t v3(tools::in_place<3>, 99.9);

		static_assert(tools::get_if<0>(&v0) != nullptr, "get_if may not return nullptr on correct index accesss.");
		static_assert(tools::get_if<1>(&v0) == nullptr, "get_if has to return nullptr on invalid index access.");
		static_assert(tools::get_if<2>(&v0) == nullptr, "get_if has to return nullptr on invalid index access.");
		static_assert(tools::get_if<3>(&v0) == nullptr, "get_if has to return nullptr on invalid index access.");

		static_assert(*tools::get_if<0>(&v0) == 3.14, "get_if has to return the correct value.");
		static_assert(*tools::get_if<1>(&v1) == 27	, "get_if has to return the correct value.");
		static_assert(*tools::get_if<2>(&v2) == 'a'	, "get_if has to return the correct value.");
		static_assert(*tools::get_if<3>(&v3) == 99.9, "get_if has to return the correct value.");

		static_assert(*tools::get_if<int>(&v1) == 27, "get_if has to return the correct value.");
		static_assert(*tools::get_if<char>(&v2) == 'a', "get_if has to return the correct value.");
	}
}
