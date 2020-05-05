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

static constexpr auto ARRAY = std::array{7, 3, 14, 27};

TEST_CASE("Testing internals of the rtct_map map() function") {
    int result = 0;
    const auto square = [&result](const auto x) {
        result = x * x;
    };

    SECTION("Test with make_integral_constant_typelist") {
        for (int i = 0; i < 7; ++i) {
            result = -1;
            const bool ran = tools::rtct_map::_detail::rtct_map(
                    square,
                    i,
                    tools::rtct_map::_detail::make_integral_constant_typelist<int, 0, 1, 2, 3, 4, 5, 6>());
            REQUIRE(ran);
            REQUIRE(result == i * i);
        }
    }

    SECTION("Test with make_integer_sequence_typelist") {
        static constexpr int N = 9;
        for (int i = 0; i < N; ++i) {
            result = -1;
            const bool ran = tools::rtct_map::_detail::rtct_map(
                    square,
                    i,
                    tools::rtct_map::_detail::make_integer_sequence_typelist<int, N>());
            REQUIRE(ran);
            REQUIRE(result == i * i);
        }
    }

    SECTION("Test with make_integral_constant_typelist_from_array") {
        static constexpr int N = ARRAY.size();
        using ValueT = typename decltype(ARRAY)::value_type;
        for (int i = 0; i < N; ++i) {
            result = -1;
            const bool ran = tools::rtct_map::_detail::rtct_map(
                    square,
                    ARRAY[i],
                    tools::rtct_map::_detail::make_integral_constant_typelist_from_array<ValueT, N, ARRAY>());
            REQUIRE(ran);
            REQUIRE(result == ARRAY[i] * ARRAY[i]);
        }

        result = -1;
        const bool ran = tools::rtct_map::_detail::rtct_map(
                square,
                0,
                tools::rtct_map::_detail::make_integral_constant_typelist_from_array<ValueT, N, ARRAY>());
        REQUIRE(!ran);
    }
}

TEST_CASE("Testing internals of the rtct_map map_transform() function") {
    static constexpr auto square = [](const auto x) {
        return x * x;
    };

    SECTION("Test with make_integral_constant_typelist") {
        for (int i = 0; i < 7; ++i) {
            REQUIRE(tools::rtct_map::_detail::rtct_map_transform(
                    square,
                    i,
                    tools::rtct_map::_detail::make_integral_constant_typelist<int, 0, 1, 2, 3, 4, 5, 6>()) == i * i);
        }
    }

    SECTION("Test with make_integer_sequence_typelist") {
        static constexpr int N = 9;
        for (int i = 0; i < N; ++i) {
            REQUIRE(tools::rtct_map::_detail::rtct_map_transform(
                    square,
                    i,
                    tools::rtct_map::_detail::make_integer_sequence_typelist<int, N>()) == i * i);
        }
    }

    SECTION("Test with make_integral_constant_typelist_from_array") {
        static constexpr int N = ARRAY.size();
        using ValueT = typename decltype(ARRAY)::value_type;
        for (int i = 0; i < N; ++i) {
            REQUIRE(tools::rtct_map::_detail::rtct_map_transform(
                    square,
                    ARRAY[i],
                    tools::rtct_map::_detail::make_integral_constant_typelist_from_array<ValueT, N, ARRAY>()) ==
                    ARRAY[i] * ARRAY[i]);
        }

        REQUIRE(!tools::rtct_map::_detail::rtct_map_transform(
                square,
                0,
                tools::rtct_map::_detail::make_integral_constant_typelist_from_array<ValueT, N, ARRAY>()).has_value());
    }

    SECTION("Test with make_integer_sequence_typelist in constexpr context") {
        static constexpr int N = 9;

        constexpr auto map_index_to_square = [](int i) {
            return *tools::rtct_map::_detail::rtct_map_transform(
                    square,
                    i,
                    tools::rtct_map::_detail::make_integer_sequence_typelist<int, N>());
        };

        REQUIRE(std::integral_constant<int, map_index_to_square(0)>::value == square(0));
        REQUIRE(std::integral_constant<int, map_index_to_square(1)>::value == square(1));
        REQUIRE(std::integral_constant<int, map_index_to_square(2)>::value == square(2));
        REQUIRE(std::integral_constant<int, map_index_to_square(3)>::value == square(3));
        REQUIRE(std::integral_constant<int, map_index_to_square(4)>::value == square(4));
        REQUIRE(std::integral_constant<int, map_index_to_square(8)>::value == square(8));
    }
}

template<int Value>
constexpr int squared() {
    return Value * Value;
}

TEST_CASE("Testing public rtct_map::make_* functions") {
    static constexpr auto square_tmp = [](const auto x) {
        return squared<x>();
    };

    static constexpr auto square = [](const auto x) {
        return x * x;
    };

    SECTION("Test rtct_map::make_map") {
        const auto map = tools::rtct_map::make_map<int, 0, 1, 2, 3, 4, 5, 6>();

        for (int i = 0; i < 7; ++i) {
            const auto result = map.map_transform(square_tmp, i);
            REQUIRE(result.has_value());
            REQUIRE(*result == i * i);
        }

        {
            const auto result = map.map_transform(square_tmp, 27);
            REQUIRE(!result.has_value());
        }

        {
            const auto result = map.map_transform(square_tmp, -1);
            REQUIRE(!result.has_value());
        }
    }

    SECTION("Test rtct_map::make_sequence_map") {
        static constexpr int N = 9;
        const auto sequence_map = tools::rtct_map::make_sequence_map<decltype(N), N>();

        for (int i = 0; i < N; ++i) {
            const auto result = sequence_map.map_transform(square_tmp, i);
            REQUIRE(result.has_value());
            REQUIRE(*result == i * i);
        }

        {
            const auto result = sequence_map.map_transform(square_tmp, N + 2);
            REQUIRE(!result.has_value());
        }

        {
            const auto result = sequence_map.map_transform(square_tmp, -10);
            REQUIRE(!result.has_value());
        }
    }

    SECTION("Test rtct_map::make_array_map") {
        static constexpr int N = ARRAY.size();
        const auto array_map = tools::rtct_map::make_array_map<ARRAY>();

        for (int i = 0; i < N; ++i) {
            const auto result = array_map.map_transform(square_tmp, ARRAY[i]);
            REQUIRE(result.has_value());
            REQUIRE(*result == ARRAY[i] * ARRAY[i]);
        }

        const auto result = array_map.map_transform(square_tmp, 0);
        REQUIRE(!result.has_value());
    }

    SECTION("Test rtct_map::make_sequence_map in constexpr context") {
        static constexpr int N = 9;

        constexpr auto map_index_to_square = [](int i) -> int {
            const auto sequence_map = tools::rtct_map::make_sequence_map<decltype(N), N>();
            const auto result = sequence_map.map_transform(square_tmp, i);
            return *result;
        };

        REQUIRE(std::integral_constant<int, map_index_to_square(0)>::value == square(0));
        REQUIRE(std::integral_constant<int, map_index_to_square(1)>::value == square(1));
        REQUIRE(std::integral_constant<int, map_index_to_square(2)>::value == square(2));
        REQUIRE(std::integral_constant<int, map_index_to_square(3)>::value == square(3));
        REQUIRE(std::integral_constant<int, map_index_to_square(4)>::value == square(4));
        REQUIRE(std::integral_constant<int, map_index_to_square(8)>::value == square(8));
    }
}

#endif
