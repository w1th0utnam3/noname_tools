#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>
#include <cassert>

#include <noname_tools\tools>

using namespace noname;

namespace noname
{
	namespace test
	{
		void sorted_vector()
		{
			{
				const auto test1 = tools::sorted_vector<std::string>({"ccc","bbb","aaa"});
				assert(test1.size() == 3);
				assert(test1.at(0) == "aaa");
				assert(test1.at(1) == "bbb");
				assert(test1.at(2) == "ccc");
				std::cout << "Tested sorted_vector with initializer list.\n";
			}

			{
				const auto test2 = tools::sorted_vector(std::vector<std::string>({"ccc","bbb","aaa"}));
				assert(test2.size() == 3);
				assert(test2.at(0) == "aaa");
				assert(test2.at(1) == "bbb");
				assert(test2.at(2) == "ccc");
				std::cout << "Tested sorted_vector with r-value vector.\n";
			}
		}

		void n_subranges()
		{
			// TODO: Test with std::list or other container
			// TODO: Test with n > length

			// Number of subranges to divide source range into
			const size_t n = 3;
			// Minimum number of elements per subrange
			const size_t factor = 5;

			{
				// Prepare source vector
				std::vector<size_t> source(n*factor);
				std::iota(source.begin(), source.end(), 0);

				// Prepare destination vector
				std::vector<decltype(source)::iterator> ranges;
				ranges.reserve(n+1);

				tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), n);

				assert(ranges.size() == n+1);
				assert(*ranges.begin() == source.begin());
				assert(ranges.back() == source.end());
				for(size_t i = 0; i < n; i++) {
					assert(*ranges.at(i) == i*factor);
				}

				std::cout << "Tested n_subranges with: (range length) % n == 0.\n";
			}

			{
				std::vector<size_t> source(n*(factor+1)-1);
				std::iota(source.begin(), source.end(), 0);

				std::vector<decltype(source)::iterator> ranges;
				ranges.reserve(n+1);

				tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), n);

				assert(ranges.size() == n+1);
				assert(*ranges.begin() == source.begin());
				assert(ranges.back() == source.end());

				// Reconstruct source vector from subranges
				std::vector<size_t> result;
				result.reserve(source.size());
				for (auto it = ranges.begin(); it != std::prev(ranges.end()); ++it) {
					for (auto it2 = *it; it2 != *std::next(it); ++it2) {
						result.push_back(*it2);
					}
				}

				assert(result.size() == source.size());
				assert(result == source);

				std::cout << "Tested n_subranges with: (range length) % n == n-1.\n";
			}

			{
				std::vector<size_t> source(n*factor);
				std::iota(source.begin(), source.end(), 0);

				std::vector<decltype(source)::iterator> ranges;

				tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), 0);

				assert(ranges.size() == 0);
				std::cout << "Tested n_subranges with n = 0.\n";

				tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), 1);

				assert(ranges.size() == 2);
				assert(*ranges.begin() == source.begin());
				assert(ranges.back() == source.end());

				std::cout << "Tested n_subranges with n = 1.\n";
			}

			{
				std::vector<size_t> source;
				std::vector<decltype(source)::iterator> ranges;

				tools::n_subranges(source.begin(), source.end(), std::back_inserter(ranges), 1);

				std::cout << "Tested n_subranges with empty source.\n";
			}
		}

		void for_each_and_successor()
		{
			const size_t n = 10;

			{
				std::vector<size_t> source(n);
				std::iota(source.begin(), source.end(), 0);

				tools::for_each_and_successor(source.begin(), source.end(), [](size_t i, size_t j)
				{
					assert(i+1 == j);
				});

				std::cout << "Tested for_each_and_successor.\n";
			}

			{
				std::vector<size_t> test;

				tools::for_each_and_successor(test.begin(), test.end(), [](size_t i, size_t j)
				{
					assert(false);
				});

				std::cout << "Tested for_each_and_successor with empty range.\n";
			}
		}

		void find_unequal_successor()
		{
			// TODO: Test with custom comparison predicate

				{
					std::vector<int> source({1,1,1,2,2,3});

					auto it = tools::find_unequal_successor(source.begin(), source.end());
					assert(*it == 2);

					it = tools::find_unequal_successor(it, source.end());
					assert(*it == 3);

					it = tools::find_unequal_successor(it, source.end());
					assert(it == source.end());

					std::cout << "Tested find_unequal_successor.\n";
				}

				{
					std::vector<int> source({1,1,1});

					auto it = tools::find_unequal_successor(source.begin(), source.end());
					assert(it == source.end());

					std::cout << "Tested find_unequal_successor with only equal elements.\n";
				}

				{
					std::vector<int> test;

					auto it = tools::find_unequal_successor(test.begin(), test.end());
					assert(it == test.end());

					std::cout << "Tested find_unequal_successor with empty range.\n";
				}
		}
	}
}

int main(int argc, char* argv[])
{
	std::cout << "noname_tools test program\n\n";

	#ifdef NDEBUG
	std::cout << "WARNING: Not built in debug mode! Asserts won't be triggered!\n";
	#endif

	test::sorted_vector();
	test::n_subranges();
	test::for_each_and_successor();
	test::find_unequal_successor();

	std::cout << "\nDone.\n";
	std::cout << std::endl;
	return 0;
}