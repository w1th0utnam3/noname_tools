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

#include <noname_tools/rtctmap_tools.h>

#include "catch2/catch.hpp"

#include <array>

using namespace noname;

#ifdef NONAME_CPP17
static constexpr const auto array = std::array { 7, 3, 14, 27 };
TEST_CASE("Testing internal _detail::rtct_map") {
    int result = 0;
    const auto square = [&result](const auto x) {
        result = x * x;
    };

    SECTION("Test with make_integral_constant_typelist") {
        for (int i = 0; i < 7; ++i) {
            const bool ran = tools::_detail::rtct_map(square, i,
                    tools::_detail::make_integral_constant_typelist<int, 0, 1, 2, 3, 4, 5, 6>());
            REQUIRE(ran);
            REQUIRE(result == i * i);
        }
    }

    SECTION("Test with make_integer_sequence_typelist") {
        static constexpr const int N = 9;
        for (int i = 0; i < N; ++i) {
            const bool ran = tools::_detail::rtct_map(square, i,
                    tools::_detail::make_integer_sequence_typelist<int, N>());
            REQUIRE(ran);
            REQUIRE(result == i * i);
        }
    }

    SECTION("Test with make_integral_constant_typelist_from_array") {
        static constexpr const int N = array.size();
        using ValueT = typename decltype(array)::value_type;
        for (int i = 0; i < N; ++i) {
            const bool ran = tools::_detail::rtct_map(square, array[i],
                    tools::_detail::make_integral_constant_typelist_from_array<ValueT, N, array>());
            REQUIRE(ran);
            REQUIRE(result == array[i] * array[i]);
        }

        const bool ran = tools::_detail::rtct_map(square, 0,
                tools::_detail::make_integral_constant_typelist_from_array<ValueT, N, array>());
        REQUIRE(!ran);
    }
}

TEST_CASE("Testing internal _detail::rtct_map_transform") {
    const auto square = [](const auto x) {
        return x * x;
    };

    SECTION("Test with make_integral_constant_typelist") {
        for (int i = 0; i < 7; ++i) {
            REQUIRE(tools::_detail::rtct_map_transform(square, i, tools::_detail::make_integral_constant_typelist<int, 0, 1, 2, 3, 4, 5, 6>()) == i * i);
        }
    }

    SECTION("Test with make_integer_sequence_typelist") {
        static constexpr const int N = 9;
        for (int i = 0; i < N; ++i) {
            REQUIRE(tools::_detail::rtct_map_transform(square, i, tools::_detail::make_integer_sequence_typelist<int, N>()) == i * i);
        }
    }

    SECTION("Test with make_integral_constant_typelist_from_array") {
        static constexpr const int N = array.size();
        using ValueT = typename decltype(array)::value_type;
        for (int i = 0; i < N; ++i) {
            REQUIRE(tools::_detail::rtct_map_transform(square, array[i],
                    tools::_detail::make_integral_constant_typelist_from_array<ValueT, N, array>()) == array[i] * array[i]);
        }

        REQUIRE(!tools::_detail::rtct_map_transform(square, 0,
                tools::_detail::make_integral_constant_typelist_from_array<ValueT, N, array>()).has_value());
    }
}
#endif
