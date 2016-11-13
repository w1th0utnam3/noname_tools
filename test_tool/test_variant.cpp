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

#include <noname_tools\tools>

#include "catch.hpp"

#include <string>
#include <type_traits>

using namespace noname;

// TODO: Test copy, move, destructor with proper test helper classes
// TODO: Check is_copy_construcible, is_move_* and other type traits
// TODO: Test with pointers, references, empty types, no types, etc.

TEST_CASE("Testing variant")
{
	typedef tools::variant<double, int, char, double> constexpr_var_t;
	typedef tools::variant<double, int, std::string, char, double> var_t;

	SECTION("Testing variant helper classes")
	{
		REQUIRE((tools::variant_size_v<constexpr_var_t> == 4));
		REQUIRE((tools::variant_size_v<const constexpr_var_t> == 4));
		REQUIRE((tools::variant_size_v<volatile constexpr_var_t> == 4));
		REQUIRE((tools::variant_size_v<const volatile constexpr_var_t> == 4));

		REQUIRE((std::is_same<tools::variant_alternative_t<0, constexpr_var_t>, double>::value == true));
		REQUIRE((std::is_same<tools::variant_alternative_t<1, constexpr_var_t>, int>::value == true));
		REQUIRE((std::is_same<tools::variant_alternative_t<2, constexpr_var_t>, char>::value == true));
		REQUIRE((std::is_same<tools::variant_alternative_t<3, constexpr_var_t>, double>::value == true));
		REQUIRE((std::is_same<tools::variant_alternative_t<1, const constexpr_var_t>, const int>::value == true));
		REQUIRE((std::is_same<tools::variant_alternative_t<1, volatile constexpr_var_t>, volatile int>::value == true));
		REQUIRE((std::is_same<tools::variant_alternative_t<1, const volatile constexpr_var_t>, const volatile int>::value == true));
	}

	SECTION("Testing constexpr constructor and index")
	{
		static_assert(constexpr_var_t().index() == 0, "Index has to be zero after default construction.");

		static_assert(constexpr_var_t(tools::in_place<0>, 3.14).index() == 0, "Index has to be the specified value after in place construction.");
		static_assert(constexpr_var_t(tools::in_place<1>, 27).index() == 1, "Index has to be the specified value after in place construction.");
		static_assert(constexpr_var_t(tools::in_place<2>, 'a').index() == 2, "Index has to be the specified value after in place construction.");
		static_assert(constexpr_var_t(tools::in_place<3>, 99.9).index() == 3, "Index has to be the specified value after in place construction.");

		static_assert(constexpr_var_t(tools::in_place<int>, 27).index() == 1, "Index has to be the specified value after in place construction.");
		static_assert(constexpr_var_t(tools::in_place<char>, 'a').index() == 2, "Index has to be the specified value after in place construction.");

		static_assert(std::is_constructible<constexpr_var_t, tools::in_place_type_t<double>, double>::value == false, 
			"Double occurs multiple times in type pack and therefore cannot be used for indexing");

		static_assert(tools::variant<char, double, int>(3.14).index() == 1, "Index has to be the specified value.");
		static_assert(tools::variant<char, double, int>(27).index() == 2, "Index has to be the specified value.");
	}

	SECTION("Testing constexpr constructor and get_if")
	{
		constexpr const constexpr_var_t* p = nullptr;
		static_assert(tools::get_if<2>(p) == nullptr, "get_if has to return nullptr for nullptr parameter.");

		constexpr const constexpr_var_t v0(tools::in_place<0>, 3.14);
		constexpr const constexpr_var_t v1(tools::in_place<1>, 27);
		constexpr const constexpr_var_t v2(tools::in_place<2>, 'a');
		constexpr const constexpr_var_t v3(tools::in_place<3>, 99.9);

		static_assert(tools::get_if<0>(&v0) != nullptr, "get_if may not return nullptr on correct index accesss.");
		static_assert(tools::get_if<1>(&v0) == nullptr, "get_if has to return nullptr on invalid index access.");
		static_assert(tools::get_if<2>(&v0) == nullptr, "get_if has to return nullptr on invalid index access.");
		static_assert(tools::get_if<3>(&v0) == nullptr, "get_if has to return nullptr on invalid index access.");

		static_assert(*tools::get_if<0>(&v0) == 3.14, "get_if has to return the correct value.");
		static_assert(*tools::get_if<1>(&v1) == 27, "get_if has to return the correct value.");
		static_assert(*tools::get_if<2>(&v2) == 'a', "get_if has to return the correct value.");
		static_assert(*tools::get_if<3>(&v3) == 99.9, "get_if has to return the correct value.");

		static_assert(*tools::get_if<int>(&v1) == 27, "get_if has to return the correct value.");
		static_assert(*tools::get_if<char>(&v2) == 'a', "get_if has to return the correct value.");
	}

	SECTION("Testing constructors and index")
	{
		REQUIRE(var_t(tools::in_place<0>, 3.14).index() == 0);
		REQUIRE(var_t(tools::in_place<1>, 27).index() == 1);
		REQUIRE(var_t(tools::in_place<2>, "Hello World!").index() == 2);
		REQUIRE(var_t(tools::in_place<3>, 'a').index() == 3);
		REQUIRE(var_t(tools::in_place<4>, 99.9).index() == 4);

		REQUIRE(var_t(27).index() == 1);
		REQUIRE(var_t("Hello World!").index() == 2);
		REQUIRE(var_t('a').index() == 3);
	}

	SECTION("Testing constructors and get_if")
	{
		const var_t* p = nullptr;
		REQUIRE(tools::get_if<2>(p) == nullptr);

		const var_t v0(tools::in_place<0>, 3.14);
		const var_t v1(tools::in_place<1>, 27);
		const var_t v2(tools::in_place<2>, "Hello World! Hello World! Hello World! Hello World!");
		const var_t v3(tools::in_place<3>, 'a');
		const var_t v4(tools::in_place<4>, 99.9);

		const var_t v5(27);
		const var_t v6("Hello World! Hello World! Hello World! Hello World!");
		const var_t v7('a');

		const var_t v8(tools::in_place<2>, { 'H', 'e', 'l', 'l', 'o' });
		const var_t v9(tools::in_place<std::string>, "Hello World! Hello World! Hello World! Hello World!");
		const var_t v10(tools::in_place<std::string>, { 'H', 'e', 'l', 'l', 'o' });

		REQUIRE(tools::get_if<0>(&v0) != nullptr);
		REQUIRE(tools::get_if<1>(&v0) == nullptr);
		REQUIRE(tools::get_if<2>(&v0) == nullptr);
		REQUIRE(tools::get_if<3>(&v0) == nullptr);
		REQUIRE(tools::get_if<4>(&v0) == nullptr);

		REQUIRE(*tools::get_if<0>(&v0) == 3.14);
		REQUIRE(*tools::get_if<1>(&v1) == 27);
		REQUIRE(*tools::get_if<2>(&v2) == "Hello World! Hello World! Hello World! Hello World!");
		REQUIRE(*tools::get_if<3>(&v3) == 'a');
		REQUIRE(*tools::get_if<4>(&v4) == 99.9);

		REQUIRE(*tools::get_if<1>(&v5) == 27);
		REQUIRE(*tools::get_if<2>(&v6) == "Hello World! Hello World! Hello World! Hello World!");
		REQUIRE(*tools::get_if<3>(&v7) == 'a');

		REQUIRE(*tools::get_if<2>(&v8) == "Hello");
		REQUIRE(*tools::get_if<2>(&v9) == "Hello World! Hello World! Hello World! Hello World!");
		REQUIRE(*tools::get_if<2>(&v10) == "Hello");

		REQUIRE(*tools::get_if<int>(&v1) == 27);
		REQUIRE(tools::get_if<std::string>(&v1) == nullptr);
		REQUIRE(tools::get_if<char>(&v1) == nullptr);

		REQUIRE(tools::get_if<int>(&v2) == nullptr);
		REQUIRE(*tools::get_if<std::string>(&v2) == "Hello World! Hello World! Hello World! Hello World!");
		REQUIRE(tools::get_if<char>(&v2) == nullptr);

		REQUIRE(tools::get_if<int>(&v3) == nullptr);
		REQUIRE(*tools::get_if<char>(&v3) == 'a');
		REQUIRE(tools::get_if<std::string>(&v1) == nullptr);
	}

	SECTION("Testing copy constructor")
	{
		var_t v0(tools::in_place<2>, "Hello World! Hello World! Hello World! Hello World!");

		REQUIRE(v0.index() == 2);
		REQUIRE(*tools::get_if<2>(&v0) == "Hello World! Hello World! Hello World! Hello World!");

		{
			var_t v1(v0);

			REQUIRE(v0.index() == 2);
			REQUIRE(*tools::get_if<2>(&v0) == "Hello World! Hello World! Hello World! Hello World!");
			REQUIRE(v1.index() == 2);
			REQUIRE(*tools::get_if<2>(&v1) == "Hello World! Hello World! Hello World! Hello World!");
		}

		{
			const var_t& c_v0 = v0;
			var_t v1(c_v0);

			REQUIRE(v1.index() == 2);
			REQUIRE(*tools::get_if<2>(&v1) == "Hello World! Hello World! Hello World! Hello World!");
		}
	}

	SECTION("Testing move constructor")
	{
		var_t v0(tools::in_place<2>, "Hello World! Hello World! Hello World! Hello World!");

		REQUIRE(v0.index() == 2);
		REQUIRE(*tools::get_if<2>(&v0) == "Hello World! Hello World! Hello World! Hello World!");

		{
			var_t v1(std::move(v0));

			REQUIRE(v1.index() == 2);
			REQUIRE(*tools::get_if<2>(&v1) == "Hello World! Hello World! Hello World! Hello World!");
		}
	}
}
