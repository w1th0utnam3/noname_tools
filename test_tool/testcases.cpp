#include "catch.hpp"

#include <numeric>

#include <noname_tools\tools>

using namespace noname;

TEST_CASE("Testing sorted_vector")
{
	SECTION("Call with initializer list") {
		const auto sortedVector = tools::sorted_vector<std::string>({"ccc","bbb","aaa"});

		REQUIRE(sortedVector.size() == 3) ;
		REQUIRE(sortedVector.at(0) == "aaa") ;
		REQUIRE(sortedVector.at(1) == "bbb") ;
		REQUIRE(sortedVector.at(2) == "ccc") ;
	}

	SECTION("Call with r-value vector") {
		const auto sortedVector = tools::sorted_vector<std::string>(std::vector<std::string>({"ccc","bbb","aaa"}));

		REQUIRE(sortedVector.size() == 3) ;
		REQUIRE(sortedVector.at(0) == "aaa") ;
		REQUIRE(sortedVector.at(1) == "bbb") ;
		REQUIRE(sortedVector.at(2) == "ccc") ;
	}

	SECTION("Call with empty r-value vector") {
		const auto sortedVector = tools::sorted_vector<std::string>(std::vector<std::string>());

		REQUIRE(sortedVector.size() == 0) ;
	}
}

TEST_CASE("Testing n_subranges")
{
	// TODO: Test with std::list or other container
	// TODO: Test with n > length

	// Number of subranges to divide source range into
	const size_t n = 3;
	// Minimum number of elements per subrange
	const size_t factor = 5;

	// Source vector
	std::vector<size_t> source;
	// Vector of output of n_subranges
	std::vector<decltype(source)::iterator> ranges;

	// Start each test with empty vectors
	REQUIRE(source.size() == 0) ;
	REQUIRE(ranges.size() == 0) ;

	SECTION("Input range with: (range length) % n == 0") {
		// Prepare destination vector
		ranges.reserve(n + 1);
		// Prepare source vector
		source.resize(n * factor);
		std::iota(source.begin(), source.end(), 0);

		tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), n);

		REQUIRE(ranges.size() == n + 1) ;
		REQUIRE(*ranges.begin() == source.begin()) ;
		REQUIRE(ranges.back() == source.end()) ;
		for (size_t i = 0; i < n; i++) {
			REQUIRE(*ranges.at(i) == i*factor) ;
		}
	}

	SECTION("Input range with: (range length) % n == n-1") {
		ranges.reserve(n + 1);
		source.resize(n * (factor + 1) - 1);
		std::iota(source.begin(), source.end(), 0);

		tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), n);

		REQUIRE(ranges.size() == n + 1) ;
		REQUIRE(*ranges.begin() == source.begin()) ;
		REQUIRE(ranges.back() == source.end()) ;

		// Reconstruct source vector from subranges
		std::vector<size_t> result;
		result.reserve(source.size());
		for (auto it = ranges.begin(); it != std::prev(ranges.end()); ++it) {
			for (auto it2 = *it; it2 != *std::next(it); ++it2) {
				result.push_back(*it2);
			}
		}

		REQUIRE(result.size() == source.size()) ;
		REQUIRE(result == source) ;
	}

	SECTION("Empty input range") {
		tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), 1);

		REQUIRE(ranges.size() == 0) ;
	}

	SECTION("Split into n = 0 ranges") {
		source.resize(n * factor);
		std::iota(source.begin(), source.end(), 0);

		tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), 0);

		REQUIRE(ranges.size() == 0) ;
	}

	SECTION("Split into n = 1 ranges") {
		source.resize(n * factor);
		std::iota(source.begin(), source.end(), 0);

		tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), 1);

		REQUIRE(ranges.size() == 2) ;
		REQUIRE(*ranges.begin() == source.begin()) ;
		REQUIRE(ranges.back() == source.end()) ;
	}
}

TEST_CASE("Testing find_unequal_usccessor")
{
	// TODO: Test with custom comparison predicate

	// Start with emptry source vector
	std::vector<int> source;
	REQUIRE(source.size() == 0) ;

	SECTION("Repeated application on returned iterator") {
		source = {1, 1, 1, 2, 2, 3};

		auto it = tools::find_unequal_successor(source.begin(), source.end());
		REQUIRE(*it == 2) ;

		it = tools::find_unequal_successor(it, source.end());
		REQUIRE(*it == 3) ;

		it = tools::find_unequal_successor(it, source.end());
		REQUIRE(it == source.end()) ;
	}

	SECTION("Input range with equal elements") {
		source = {1, 1, 1};

		const auto it = tools::find_unequal_successor(source.begin(), source.end());

		REQUIRE(it == source.end()) ;
	}

	SECTION("Empty input range") {
		const auto it = tools::find_unequal_successor(source.begin(), source.end());

		REQUIRE(it == source.end()) ;
	}
}
