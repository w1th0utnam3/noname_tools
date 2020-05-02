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

#include <noname_tools/functional_tools.h>

#include "catch2/catch.hpp"

#include <type_traits>

using namespace noname;

#ifdef NONAME_CPP17
TEST_CASE("Testing apply_to_sequence") {
    constexpr const int N = 5;

    int sum = 0;
    auto compute_sum = [N, &sum](auto... Is) {
        sum = (Is() + ...);
    };

    tools::apply_to_sequence<N>(compute_sum);
    REQUIRE(sum == (N * (N - 1))/2);
}

TEST_CASE("Testing constexpr apply_to_sequence") {
    static constexpr const int N = 5;
    constexpr int csum = []() {
        int sum = 0;
        auto compute_sum = [&sum](auto... Is) {
            sum = (Is() + ...);
        };
        tools::apply_to_sequence<N>(compute_sum);
        return sum;
    }();

    REQUIRE(csum == (N * (N - 1))/2);
    REQUIRE(std::integral_constant<int,csum>::value == std::integral_constant<int, (N * (N - 1))/2>::value);
}
#endif
