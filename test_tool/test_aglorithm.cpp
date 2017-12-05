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

#include <noname_tools/algorithm_tools.h>

#include "catch.hpp"

#include <vector>
#include <array>
#include <numeric>
#include <functional>
#include <type_traits>
#include <algorithm>

using namespace noname;

// TODO: Tests for strict_unique_copy
// TODO: Test with std::list or other container
// TODO: Test with n > length

TEST_CASE("Testing n_subranges")
{
	// Number of subranges to divide source range into
	const int n = 3;
	// Minimum number of elements per subrange
	const int factor = 5;

	// Type used for source vector
	typedef std::size_t value_t;
	// Source vector
	std::vector<value_t> source;
	// Vector of output of n_subranges
	std::vector<decltype(source)::iterator> ranges;

	// Start each test with empty vectors
	REQUIRE(source.size() == 0);
	REQUIRE(ranges.size() == 0);

	SECTION("Input range with: (range length) % n == 0")
	{
		// Prepare destination vector
		ranges.reserve(n + 1);
		// Prepare source vector
		source.resize(n * factor);
		std::iota(source.begin(), source.end(), 0);

		tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), n);

		REQUIRE(ranges.size() == n + 1);
		REQUIRE(*ranges.begin() == source.begin());
		REQUIRE(ranges.back() == source.end());
		for (std::size_t i = 0; i < n; i++) {
			REQUIRE(*ranges.at(i) == i*factor);
		}
	}

	SECTION("Input range with: (range length) % n == n-1")
	{
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

	SECTION("Empty input range")
	{
		tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), 1);

		REQUIRE(ranges.size() == 0);
	}

	SECTION("Split into n = 0 ranges")
	{
		source.resize(n * factor);
		std::iota(source.begin(), source.end(), 0);

		tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), 0);

		REQUIRE(ranges.size() == 0);
	}

	SECTION("Split into n = 1 ranges")
	{
		source.resize(n * factor);
		std::iota(source.begin(), source.end(), 0);

		tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), 1);

		REQUIRE(ranges.size() == 2);
		REQUIRE(*ranges.begin() == source.begin());
		REQUIRE(ranges.back() == source.end());
	}

	SECTION("sources.size() < n")
	{
		source.resize(5);
		std::iota(source.begin(), source.end(), 0);

		tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), 10);

		REQUIRE(ranges.size() == 6);
		REQUIRE(*ranges.begin() == source.begin());
		REQUIRE(ranges.back() == source.end());
	}
}

TEST_CASE("Testing for_each_and_successor")
{
	// Value type for the test container
	typedef std::size_t value_t;

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
	SECTION("Testing accumulator functor")
	{
		acc(42, 4711);
		REQUIRE(acc.value() == 4753);
	}

	SECTION("Testing with ascending input range")
	{
		source = { 0, 1, 2, 3, 4 };

		const auto acc_return = tools::for_each_and_successor(source.begin(), source.end(), acc);

		REQUIRE(acc_return.value() == 16);
	}

	SECTION("Testing with empty input range")
	{
		const auto acc_return = tools::for_each_and_successor(source.begin(), source.end(), acc);

		REQUIRE(acc_return.value() == 0);
	}

	SECTION("Testing with one elment in input range")
	{
		source = { 42 };

		const auto acc_return = tools::for_each_and_successor(source.begin(), source.end(), acc);

		REQUIRE(acc_return.value() == 0);
	}
}

TEST_CASE("Testing find_unequal_successor")
{
	// Value type for source container
	typedef std::size_t value_t;
	// Start with emptry source vector
	std::vector<value_t> source;
	REQUIRE(source.size() == 0);

	SECTION("Using equal operator for comparison")
	{

		SECTION("Repeated application on returned iterator")
		{
			source = { 1, 1, 1, 2, 2, 3 };

			auto it = tools::find_unequal_successor(source.begin(), source.end());
			REQUIRE((std::is_same<decltype(it), decltype(source)::iterator>::value) == true);
			REQUIRE(*it == 2);

			it = tools::find_unequal_successor(it, source.end());
			REQUIRE(*it == 3);

			it = tools::find_unequal_successor(it, source.end());
			REQUIRE(it == source.end());
		}

		SECTION("Input range with equal elements")
		{
			source = { 1, 1, 1 };

			const auto it = tools::find_unequal_successor(source.cbegin(), source.cend());

			REQUIRE((std::is_same<decltype(it), const decltype(source)::const_iterator>::value) == true);
			REQUIRE(it == source.end());
		}

		SECTION("Empty input range")
		{
			const auto it = tools::find_unequal_successor(source.cbegin(), source.cend());

			REQUIRE((std::is_same<decltype(it), const decltype(source)::const_iterator>::value) == true);
			REQUIRE(it == source.end());
		}

	}

	SECTION("Using predicate for comparison")
	{
		// Get default equal operator function
		const auto not_equal = std::not_equal_to<decltype(source)::value_type>();

		SECTION("Repeated application on returned iterator")
		{
			source = { 1, 1, 1, 2, 2, 3 };

			auto it = tools::find_unequal_successor(source.begin(), source.end(), not_equal);
			REQUIRE((std::is_same<decltype(it), decltype(source)::iterator>::value) == true);
			REQUIRE(*it == 2);

			it = tools::find_unequal_successor(it, source.end(), not_equal);
			REQUIRE(*it == 3);

			it = tools::find_unequal_successor(it, source.end(), not_equal);
			REQUIRE(it == source.end());
		}

		SECTION("Input range with equal elements")
		{
			source = { 1, 1, 1 };

			const auto it = tools::find_unequal_successor(source.cbegin(), source.cend(), not_equal);

			REQUIRE((std::is_same<decltype(it), const decltype(source)::const_iterator>::value) == true);
			REQUIRE(it == source.end());
		}

		SECTION("Empty input range")
		{
			const auto it = tools::find_unequal_successor(source.cbegin(), source.cend(), not_equal);

			REQUIRE((std::is_same<decltype(it), const decltype(source)::const_iterator>::value) == true);
			REQUIRE(it == source.end());
		}
	}
}

TEST_CASE("Testing make_output_iterator_adapter")
{
	SECTION("Testing in-place constructed, generic lambda")
	{
		const std::array<int, 7> a1{ 0, 10, 20, 30, 40, 50, 60 };
		const std::array<int, 6> a2{ 5, 10, 22, 30, 33, 60 };
		std::vector<int> out1;
		std::vector<int> out2;

		std::set_difference(a1.begin(), a1.end(), a2.begin(), a2.begin(), std::back_inserter(out1));
		std::set_difference(a1.begin(), a1.end(), a2.begin(), a2.begin(), tools::make_output_iterator_adapter(
			[&](auto i) { out2.push_back(i); }
		));

		REQUIRE(std::equal(out1.begin(), out1.end(), out2.begin(), out2.end()) == true);
	}
}
