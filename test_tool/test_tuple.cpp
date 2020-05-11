//	MIT License
//
//	Copyright (c) 2020 Fabian Löschner
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

#include <noname_tools/tuple_tools.h>

#include "catch2/catch.hpp"

#include <tuple>
#include <string>
#include <vector>
#include <utility>
#include <type_traits>
#include <functional>

using namespace noname;

TEST_CASE("Testing tuple_for_each") {
    SECTION("Testing with tuple size of 3 and l-value") {
        std::tuple<int, double, float> tuple{2, 42.5, 33.5f};

        std::vector<double> target;
        REQUIRE(target.size() == 0);

        tools::tuple_for_each(tuple, [&target](auto x) {
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

        tools::tuple_for_each(std::make_tuple(source), [&out](const std::string &x) {
            out = x;
        });

        REQUIRE(out == source);
    }

    SECTION("Testing return value of functor") {
        std::tuple<int, double, float> tuple{2, 42.5, 33.5f};

        // Define functor that increments counter on call
        struct Functor {
            int i;

            Functor(int i) : i(i) {};

            void operator()(double d) {
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

    SECTION("Testing with pairs") {
        std::pair<double, double> pair{42, 24};

        std::vector<double> target;
        REQUIRE(target.size() == 0);

        tools::tuple_for_each(pair, [&target](auto x) {
            target.emplace_back(x);
        });

        REQUIRE(target.size() == 2);
        REQUIRE(target.at(0) == 42);
        REQUIRE(target.at(1) == 24);
    }

    SECTION("Testing modification of tuple using references") {
        std::tuple<int, double, float> tuple{2, 42.5, 33.5f};

        // Test preconditions
        REQUIRE(std::get<0>(tuple) != 42);
        REQUIRE(std::get<1>(tuple) != 42);
        REQUIRE(std::get<2>(tuple) != 42);

        tools::tuple_for_each(tuple, [](auto &val) {
            val = 42;
        });

        // Check whether modification of tuple values with reference works
        REQUIRE(std::get<0>(tuple) == 42);
        REQUIRE(std::get<1>(tuple) == 42);
        REQUIRE(std::get<2>(tuple) == 42);
    }
}

TEST_CASE("Testing ref_tuple/cref_tuple") {
    SECTION("Testing ref_tuple") {
        std::tuple<int, double, char> tuple{2, 42.5, 'c'};

        auto ref_tuple = tools::ref_tuple(tuple);

        REQUIRE(std::is_same<std::tuple_element<0, decltype(ref_tuple)>::type, std::reference_wrapper<int>>::value);
        REQUIRE(std::is_same<std::tuple_element<1, decltype(ref_tuple)>::type, std::reference_wrapper<double>>::value);
        REQUIRE(std::is_same<std::tuple_element<2, decltype(ref_tuple)>::type, std::reference_wrapper<char>>::value);

        std::get<0>(ref_tuple).get() = 27;
        std::get<1>(ref_tuple).get() = 43.2;
        std::get<2>(ref_tuple).get() = 'x';

        REQUIRE(std::get<0>(tuple) == 27);
        REQUIRE(std::get<1>(tuple) == 43.2);
        REQUIRE(std::get<2>(tuple) == 'x');
    }

    SECTION("Testing cref_tuple") {
        std::tuple<int, double, char> tuple{2, 42.5, 'c'};

        auto cref_tuple = tools::cref_tuple(tuple);

        REQUIRE(std::is_same<std::tuple_element<0, decltype(cref_tuple)>::type, std::reference_wrapper<const int>>::value);
        REQUIRE(std::is_same<std::tuple_element<1, decltype(cref_tuple)>::type, std::reference_wrapper<const double>>::value);
        REQUIRE(std::is_same<std::tuple_element<2, decltype(cref_tuple)>::type, std::reference_wrapper<const char>>::value);

        std::get<0>(tuple) = 27;
        std::get<1>(tuple) = 43.2;
        std::get<2>(tuple) = 'x';

        REQUIRE(std::get<0>(cref_tuple) == 27);
        REQUIRE(std::get<1>(cref_tuple) == 43.2);
        REQUIRE(std::get<2>(cref_tuple) == 'x');
    }
}
