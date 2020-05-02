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

#include <noname_tools/typelist_tools.h>

#include "catch2/catch.hpp"

using namespace noname;

#include <iostream>

TEST_CASE("Testing typelist") {
    using empty_typelist = tools::typelist<>;
    using idc_typelist = tools::typelist<int, double, char, float>;
    using idicff_typelist = tools::typelist<int, double, int, char, float, float>;

    SECTION("Making static tests, empty list") {
        static_assert(tools::typelist_size<empty_typelist>::value == 0, "typelist_size should have the correct value");
        static_assert(sizeof(empty_typelist) == 1, "Typelist size has to of size one (EBO)");
    }

    SECTION("Making static tests, unique types") {
        static_assert(tools::typelist_size<idc_typelist>::value == 4, "typelist_size should have the correct value");
        static_assert(tools::typelist_size_v<idc_typelist> == 4, "typelist_size should have the correct value");

        static_assert(std::is_same<tools::nth_typelist_element<0, idc_typelist>::type, int>::value,
                      "nth_typelist_element should have the correct type");
        static_assert(std::is_same<tools::nth_typelist_element<1, idc_typelist>::type, double>::value,
                      "nth_typelist_element should have the correct type");
        static_assert(std::is_same<tools::nth_typelist_element<2, idc_typelist>::type, char>::value,
                      "nth_typelist_element should have the correct type");
        static_assert(std::is_same<tools::nth_typelist_element<3, idc_typelist>::type, float>::value,
                      "nth_typelist_element should have the correct type");

        static_assert(std::is_same<tools::nth_typelist_element_t<0, idc_typelist>, int>::value,
                      "nth_typelist_element should have the correct type");
        static_assert(std::is_same<tools::nth_typelist_element_t<1, idc_typelist>, double>::value,
                      "nth_typelist_element should have the correct type");
        static_assert(std::is_same<tools::nth_typelist_element_t<2, idc_typelist>, char>::value,
                      "nth_typelist_element should have the correct type");
        static_assert(std::is_same<tools::nth_typelist_element_t<3, idc_typelist>, float>::value,
                      "nth_typelist_element should have the correct type");

        static_assert(sizeof(idc_typelist) == 1, "Typelist size has to of size one (EBO)");
    }

    SECTION("Making static tests, non-unique types") {
        static_assert(tools::typelist_size<idicff_typelist>::value == 6, "typelist_size should have the correct value");
        static_assert(tools::typelist_size_v<idicff_typelist> == 6, "typelist_size should have the correct value");

        static_assert(std::is_same<tools::nth_typelist_element_t<0, idicff_typelist>, int>::value,
                      "nth_typelist_element should have the correct type");
        static_assert(std::is_same<tools::nth_typelist_element_t<1, idicff_typelist>, double>::value,
                      "nth_typelist_element should have the correct type");
        static_assert(std::is_same<tools::nth_typelist_element_t<2, idicff_typelist>, int>::value,
                      "nth_typelist_element should have the correct type");
        static_assert(std::is_same<tools::nth_typelist_element_t<3, idicff_typelist>, char>::value,
                      "nth_typelist_element should have the correct type");
        static_assert(std::is_same<tools::nth_typelist_element_t<4, idicff_typelist>, float>::value,
                      "nth_typelist_element should have the correct type");
        static_assert(std::is_same<tools::nth_typelist_element_t<5, idicff_typelist>, float>::value,
                      "nth_typelist_element should have the correct type");

        static_assert(sizeof(idc_typelist) == 1, "Typelist size has to of size one (EBO)");
    }

    SECTION("Testing typelist_for_each with empty list") {
        double result = 0;
        tools::typelist_for_each<empty_typelist>([&result](auto ta) {
            using type = typename decltype(ta)::type;
            result += (type(256) / type(3)) + type(10);
        });

        double comparison = 0;
        REQUIRE(result == comparison);
    }

    SECTION("Testing typelist_for_each with unique types") {
        double result = 0;
        tools::typelist_for_each<idc_typelist>([&result](auto ta) {
            using type = typename decltype(ta)::type;
            result += (type(256) / type(3)) + type(10);
        });

        double comparison = 0;
        comparison += (int(256) / int(3)) + int(10);
        comparison += (double(256) / double(3)) + double(10);
        comparison += (char(256) / char(3)) + char(10);
        comparison += (float(256) / float(3)) + float(10);

        REQUIRE(result == comparison);
    }

    SECTION("Testing typelist_for_each with non-unique types") {
        double result = 0;
        tools::typelist_for_each<idicff_typelist>([&result](auto ta) {
            using type = typename decltype(ta)::type;
            result += (type(256) / type(3)) + type(10);
        });

        double comparison = 0;
        comparison += (int(256) / int(3)) + int(10);
        comparison += (double(256) / double(3)) + double(10);
        comparison += (int(256) / int(3)) + int(10);
        comparison += (char(256) / char(3)) + char(10);
        comparison += (float(256) / float(3)) + float(10);
        comparison += (float(256) / float(3)) + float(10);

        REQUIRE(result == comparison);
    }

}
