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

#include <noname_tools/utility_tools.h>

#include "catch2/catch.hpp"

#include <type_traits>

using namespace noname;

TEST_CASE("Testing utility types") {
    REQUIRE((std::is_same<tools::nth_element_t<0, int, void, double>, int>::value == true));
    REQUIRE((std::is_same<tools::nth_element_t<1, int, void, double>, void>::value == true));
    REQUIRE((std::is_same<tools::nth_element_t<2, int, void, double>, double>::value == true));

    REQUIRE((tools::element_index_v<int, int, double, void> == 0));
    REQUIRE((tools::element_index_v<double, int, double, void> == 1));
    REQUIRE((tools::element_index_v<void, int, double, void> == 2));
    REQUIRE((tools::element_index_v<std::string, int, double, void> == tools::element_not_found));

    REQUIRE((tools::count_element_v<int, void, double, int, int, char, int> == 3));
    REQUIRE((tools::count_element_v<int> == 0));

    REQUIRE((tools::unique_elements_v<int, double, char> == true));
    REQUIRE((tools::unique_elements_v<int, double, int, char> == false));
    REQUIRE((tools::unique_elements_v<int> == true));
    REQUIRE((tools::unique_elements_v<> == false));
    REQUIRE((tools::unique_elements_v<void, double> == true));
}
