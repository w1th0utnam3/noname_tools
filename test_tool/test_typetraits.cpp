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

#include <noname_tools/typetraits_tools.h>
#include <noname_tools/utility_tools.h>

#include "catch2/catch.hpp"

#include <string>
#include <type_traits>

using namespace noname;

// TODO: Test if void_t really works for its intended use case

TEST_CASE("Testing typetraits")
{
    SECTION("Testing type_identity")
    {
        REQUIRE((std::is_same<tools::type_identity<int>::type, int>::value == true));
        REQUIRE((std::is_same<tools::type_identity<double>::type, double>::value == true));
        REQUIRE((std::is_same<tools::type_identity<char>::type, char>::value == true));
    }

	SECTION("Testing dependent_false")
	{
		REQUIRE((tools::dependent_false<int>::value == false));
		REQUIRE((tools::dependent_false<double>::value == false));
		REQUIRE((tools::dependent_false<void>::value == false));
	}

	SECTION("Testing void_t")
	{
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
		REQUIRE((tools::is_referenceable_v<double> == true));
		REQUIRE((tools::is_referenceable_v<double&> == true));
		REQUIRE((tools::is_referenceable_v<void> == false));
	}

	SECTION("Testing is_swappable")
	{
		REQUIRE((tools::is_swappable_with_v<double, double> == true));
		REQUIRE((tools::is_swappable_with_v<double, void> == false));

		REQUIRE((tools::is_swappable_v<double> == true));
		REQUIRE((tools::is_swappable_v<void> == false));
	}

    SECTION("Testing remove_cvref")
    {
        REQUIRE((std::is_same<tools::remove_cvref_t<double>, double>::value == true));
        REQUIRE((std::is_same<tools::remove_cvref_t<double&>, double>::value == true));
        REQUIRE((std::is_same<tools::remove_cvref_t<double&&>, double>::value == true));
        REQUIRE((std::is_same<tools::remove_cvref_t<const double&>, double>::value == true));
        REQUIRE((std::is_same<tools::remove_cvref_t<const volatile double&>, double>::value == true));
        REQUIRE((std::is_same<tools::remove_cvref_t<const double[2]>, double[2]>::value == true));
        REQUIRE((std::is_same<tools::remove_cvref_t<const double(&)[2]>, double[2]>::value == true));
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

	REQUIRE((tools::unique_elements_v<int, double, char> == true));
	REQUIRE((tools::unique_elements_v<int, double, int, char> == false));
	REQUIRE((tools::unique_elements_v<int> == true));
	REQUIRE((tools::unique_elements_v<> == false));
	REQUIRE((tools::unique_elements_v<void, double> == true));
}
