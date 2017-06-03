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

#include <noname_tools/tools>

#include "catch.hpp"

#include <string>
#include <type_traits>
#include <iostream>
#include <ostream>
#include <memory>
#include <functional>

using namespace noname;

// TODO: Tests for r-value visit
// TODO: Tests for bad_variant_access
// TODO: Test copy, move, destructor with proper test helper classes
// TODO: Check is_copy_constructible, is_move_* and other type traits
// TODO: Test with pointers, references, empty types, no types, etc.

TEST_CASE("Testing variant")
{
	typedef tools::variant<double, int, char, double> constexpr_var_t;
	typedef tools::variant<double, int, std::string, char, double> var_t;
	typedef tools::variant<std::reference_wrapper<double>, std::unique_ptr<std::string>, std::shared_ptr<double>, int*> pointer_var_t;

	const std::string long_string = "Hello World! Hello World! Hello World! Hello World!";

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

	SECTION("Testing type_traits, trivial types")
	{
		static_assert((std::is_default_constructible<constexpr_var_t>::value == true), "variant should be default constructible");
		static_assert((std::is_copy_constructible<constexpr_var_t>::value == true), "variant should be copy constructible");
		static_assert((std::is_move_constructible<constexpr_var_t>::value == true), "variant should be move constructible");
		static_assert((std::is_copy_assignable<constexpr_var_t>::value == true), "variant should be copy assignable");
		static_assert((std::is_move_assignable<constexpr_var_t>::value == true), "variant should be move assignable");
		static_assert((std::is_destructible<constexpr_var_t>::value == true), "variant should be destructible");

		static_assert((std::is_trivially_default_constructible<constexpr_var_t>::value == false), "variant should not be trivially default constructible (because of value initialization and index=0)");
		static_assert((std::is_trivially_copy_constructible<constexpr_var_t>::value == true), "variant should be trivially copy constructible");
		static_assert((std::is_trivially_move_constructible<constexpr_var_t>::value == true), "variant should be trivially move constructible");
		static_assert((std::is_trivially_copy_assignable<constexpr_var_t>::value == true), "variant should be trivially copy assignable");
		static_assert((std::is_trivially_move_assignable<constexpr_var_t>::value == true), "variant should be trivially move assignable");
		static_assert((std::is_trivially_destructible<constexpr_var_t>::value == true), "variant should be trivially destructible");

		static_assert((std::is_assignable<var_t, double>::value == false), "variant should not be assignable from this type (ambigious)");
		static_assert((std::is_assignable<var_t, int>::value == true), "variant should be assignable from this type");
		static_assert((std::is_assignable<var_t, char>::value == true), "variant should be assignable from this type");
	}

	SECTION("Testing type_traits, non trivial types")
	{
		static_assert((std::is_default_constructible<var_t>::value == true), "variant should be default constructible");
		static_assert((std::is_copy_constructible<var_t>::value == true), "variant should be copy constructible");
		static_assert((std::is_move_constructible<var_t>::value == true), "variant should be move constructible");
		static_assert((std::is_copy_assignable<var_t>::value == true), "variant should be copy assignable");
		static_assert((std::is_move_assignable<var_t>::value == true), "variant should be move assignable");
		static_assert((std::is_destructible<var_t>::value == true), "variant should be destructible");

		static_assert((std::is_trivially_default_constructible<var_t>::value == false), "variant should not be trivially default constructible (because of value initialization and index=0)");
		static_assert((std::is_trivially_copy_constructible<var_t>::value == false), "variant should not be trivially copy constructible");
		static_assert((std::is_trivially_move_constructible<var_t>::value == false), "variant should not be trivially move constructible");
		static_assert((std::is_trivially_copy_assignable<var_t>::value == false), "variant should not be trivially copy assignable");
		static_assert((std::is_trivially_move_assignable<var_t>::value == false), "variant should not be trivially move assignable");
		static_assert((std::is_trivially_destructible<var_t>::value == false), "variant should not be trivially destructible");

		static_assert((std::is_assignable<var_t, double>::value == false), "variant should not be assignable from this type (ambigious)");
		static_assert((std::is_assignable<var_t, int>::value == true), "variant should be assignable from this type");
		static_assert((std::is_assignable<var_t, char>::value == true), "variant should be assignable from this type");
		static_assert((std::is_assignable<var_t, std::string>::value == true), "variant should be assignable from this type");
	}

	SECTION("Testing type_traits, special types (no default construction, no copy, etc.)")
	{
		static_assert((std::is_default_constructible<pointer_var_t>::value == false), "variant should not be default constructible (because of std::reference_wrapper)");
		//static_assert((std::is_copy_constructible<pointer_var_t>::value == false), "variant should not be copy constructible (because of std::unique_ptr)");
		static_assert((std::is_move_constructible<pointer_var_t>::value == true), "variant should be move constructible");
		//static_assert((std::is_copy_assignable<pointer_var_t>::value == false), "variant should not be copy assignable (because of std::unique_ptr)");
		static_assert((std::is_move_assignable<pointer_var_t>::value == true), "variant should be move assignable");
		static_assert((std::is_destructible<pointer_var_t>::value == true), "variant should be destructible");

		static_assert((std::is_assignable<pointer_var_t, std::reference_wrapper<double>>::value == true), "variant should be assignable from this type");
		static_assert((std::is_assignable<pointer_var_t, std::unique_ptr<std::string>&&>::value == true), "variant should be assignable from this type");
		//static_assert((std::is_assignable<pointer_var_t, std::unique_ptr<std::string>>::value == false), "variant should be not assignable from this type");
		static_assert((std::is_assignable<pointer_var_t, std::shared_ptr<double>>::value == true), "variant should be assignable from this type");
		static_assert((std::is_assignable<pointer_var_t, int*>::value == true), "variant should be assignable from this type");
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

		var_t v0{ 27 };
		var_t v1{ "Hello World!" };
		var_t v2{ 'a' };

		REQUIRE(v0.index() == 1);
		REQUIRE(v1.index() == 2);
		REQUIRE(v2.index() == 3);
	}

	SECTION("Testing constructors and get_if")
	{
		const var_t* p = nullptr;
		REQUIRE(tools::get_if<2>(p) == nullptr);

		const var_t v0(tools::in_place<0>, 3.14);
		const var_t v1(tools::in_place<1>, 27);
		const var_t v2(tools::in_place<2>, long_string);
		const var_t v3(tools::in_place<3>, 'a');
		const var_t v4(tools::in_place<4>, 99.9);

		const var_t v5(27);
		const var_t v6(long_string);
		const var_t v7('a');

		const var_t v8(tools::in_place<2>, { 'H', 'e', 'l', 'l', 'o' });
		const var_t v9(tools::in_place<std::string>, long_string);
		const var_t v10(tools::in_place<std::string>, { 'H', 'e', 'l', 'l', 'o' });

		REQUIRE(tools::get_if<0>(&v0) != nullptr);
		REQUIRE(tools::get_if<1>(&v0) == nullptr);
		REQUIRE(tools::get_if<2>(&v0) == nullptr);
		REQUIRE(tools::get_if<3>(&v0) == nullptr);
		REQUIRE(tools::get_if<4>(&v0) == nullptr);

		REQUIRE(*tools::get_if<0>(&v0) == 3.14);
		REQUIRE(*tools::get_if<1>(&v1) == 27);
		REQUIRE(*tools::get_if<2>(&v2) == long_string);
		REQUIRE(*tools::get_if<3>(&v3) == 'a');
		REQUIRE(*tools::get_if<4>(&v4) == 99.9);

		REQUIRE(*tools::get_if<1>(&v5) == 27);
		REQUIRE(*tools::get_if<2>(&v6) == long_string);
		REQUIRE(*tools::get_if<3>(&v7) == 'a');

		REQUIRE(*tools::get_if<2>(&v8) == "Hello");
		REQUIRE(*tools::get_if<2>(&v9) == long_string);
		REQUIRE(*tools::get_if<2>(&v10) == "Hello");

		REQUIRE(*tools::get_if<int>(&v1) == 27);
		REQUIRE(tools::get_if<std::string>(&v1) == nullptr);
		REQUIRE(tools::get_if<char>(&v1) == nullptr);

		REQUIRE(tools::get_if<int>(&v2) == nullptr);
		REQUIRE(*tools::get_if<std::string>(&v2) == long_string);
		REQUIRE(tools::get_if<char>(&v2) == nullptr);

		REQUIRE(tools::get_if<int>(&v3) == nullptr);
		REQUIRE(*tools::get_if<char>(&v3) == 'a');
		REQUIRE(tools::get_if<std::string>(&v1) == nullptr);
	}

	SECTION("Testing constructors, index and get_if for special types (reference_wrapper, unique_ptr...)")
	{
		double val0 = 12.3;
		auto val2 = std::make_shared<double>(45.2);
		int val3 = 27;

		pointer_var_t v0{ std::reference_wrapper<double>(val0) };
		pointer_var_t v1{ std::make_unique<std::string>(long_string) };
		pointer_var_t v2(val2);
		pointer_var_t v3(&val3);

		REQUIRE(v0.index() == 0);
		REQUIRE(v1.index() == 1);
		REQUIRE(v2.index() == 2);
		REQUIRE(v3.index() == 3);

		REQUIRE(*tools::get_if<0>(&v0) == 12.3);
		REQUIRE(**tools::get_if<1>(&v1) == long_string);
		REQUIRE(**tools::get_if<2>(&v2) == 45.2);
		REQUIRE(**tools::get_if<3>(&v3) == 27);
	}

	SECTION("Testing copy constructor, non trivial types")
	{
		var_t v0(tools::in_place<2>, long_string);

		REQUIRE(v0.index() == 2);
		REQUIRE(*tools::get_if<2>(&v0) == long_string);

		{
			var_t v1(v0);

			REQUIRE(v0.index() == 2);
			REQUIRE(*tools::get_if<2>(&v0) == long_string);
			REQUIRE(v1.index() == 2);
			REQUIRE(*tools::get_if<2>(&v1) == long_string);
		}

		{
			const var_t& c_v0 = v0;
			var_t v1(c_v0);

			REQUIRE(v1.index() == 2);
			REQUIRE(*tools::get_if<2>(&v1) == long_string);
		}
	}

	SECTION("Testing copy constructor, trivial types")
	{
		constexpr_var_t v0(tools::in_place<3>, 27.0);

		REQUIRE(v0.index() == 3);
		REQUIRE(*tools::get_if<3>(&v0) == 27.0);

		{
			constexpr_var_t v1(v0);

			REQUIRE(v0.index() == 3);
			REQUIRE(*tools::get_if<3>(&v0) == 27.0);
			REQUIRE(v1.index() == 3);
			REQUIRE(*tools::get_if<3>(&v1) == 27.0);
		}

		{
			const constexpr_var_t& c_v0 = v0;
			constexpr_var_t v1(c_v0);

			REQUIRE(v1.index() == 3);
			REQUIRE(*tools::get_if<3>(&v1) == 27.0);
		}
	}

	SECTION("Testing move constructor, non trivial types")
	{
		var_t v0(tools::in_place<2>, long_string);

		REQUIRE(v0.index() == 2);
		REQUIRE(*tools::get_if<2>(&v0) == long_string);

		{
			var_t v1(std::move(v0));

			REQUIRE(v1.index() == 2);
			REQUIRE(*tools::get_if<2>(&v1) == long_string);
		}
	}	

	SECTION("Testing move constructor, trivial types")
	{
		constexpr_var_t v0(tools::in_place<2>, 'c');

		REQUIRE(v0.index() == 2);
		REQUIRE(*tools::get_if<2>(&v0) == 'c');

		{
			constexpr_var_t v1(std::move(v0));

			REQUIRE(v1.index() == 2);
			REQUIRE(*tools::get_if<2>(&v1) == 'c');
		}
	}

	SECTION("Testing move constructor, special types (reference_wrapper, unique_ptr...)")
	{
		double val0 = 12.3;

		pointer_var_t v0{ std::reference_wrapper<double>(val0) };
		pointer_var_t v1{ std::make_unique<std::string>(long_string) };

		REQUIRE(v0.index() == 0);
		REQUIRE(v1.index() == 1);

		auto v0_m(std::move(v0));
		auto v1_m(std::move(v1));

		REQUIRE(v0_m.index() == 0);
		REQUIRE(v1_m.index() == 1);

		REQUIRE(*tools::get_if<0>(&v0_m) == 12.3);
		REQUIRE(**tools::get_if<1>(&v1_m) == long_string);
	}

	SECTION("Testing emplace, non trivial types")
	{
		var_t v0(27);

		REQUIRE(v0.index() == 1);

		v0.emplace<0>(22.2);
		REQUIRE(v0.index() == 0);
		REQUIRE(*tools::get_if<0>(&v0) == 22.2);

		v0.emplace<2>(long_string);
		REQUIRE(v0.index() == 2);
		REQUIRE(*tools::get_if<2>(&v0) == long_string);

		auto& s = v0.emplace<2>({ 'a', 'b', 'c' });
		static_assert(std::is_same<decltype(s), std::string&>::value, "emplace has to return a reference");
		REQUIRE(v0.index() == 2);
		REQUIRE(*tools::get_if<2>(&v0) == "abc");
		REQUIRE(s == "abc");

		char& c = v0.emplace<char>('q');
		REQUIRE(v0.index() == 3);
		REQUIRE(*tools::get_if<3>(&v0) == 'q');
		REQUIRE(c == 'q');
	}

	SECTION("Testing emplace, trivial types")
	{
		constexpr_var_t v0(27);

		REQUIRE(v0.index() == 1);

		v0.emplace<0>(22.2);
		REQUIRE(v0.index() == 0);
		REQUIRE(*tools::get_if<0>(&v0) == 22.2);

		char& c = v0.emplace<char>('q');
		REQUIRE(v0.index() == 2);
		REQUIRE(*tools::get_if<2>(&v0) == 'q');
		REQUIRE(c == 'q');

		c = 'x';
		REQUIRE(*tools::get_if<2>(&v0) == 'x');
	}

	SECTION("Testing emplace, special types (reference_wrapper, unique_ptr...)")
	{
		double val0 = 12.3;
		double val1 = 27.3;

		pointer_var_t v0{ std::make_unique<std::string>(long_string) };

		v0.emplace<std::reference_wrapper<double>>(val0);

		REQUIRE(v0.index() == 0);
		REQUIRE(*tools::get_if<0>(&v0) == 12.3);
		
		v0.emplace<0>(val1);

		REQUIRE(v0.index() == 0);
		REQUIRE(*tools::get_if<0>(&v0) == 27.3);
		
		std::string* s0 = new std::string(long_string);
		v0.emplace<std::unique_ptr<std::string>>(s0);

		REQUIRE(v0.index() == 1);
		REQUIRE(**tools::get_if<1>(&v0) == long_string);
	}

	SECTION("Testing copy assignment, non trivial types")
	{
		var_t v0(27);
		var_t v1(long_string);
		var_t v2(long_string + long_string);

		v0 = v1;

		REQUIRE(v0.index() == 2);
		REQUIRE(v1.index() == 2);

		REQUIRE(*tools::get_if<2>(&v0) == long_string);
		REQUIRE(*tools::get_if<2>(&v1) == long_string);

		v0 = v2;

		REQUIRE(v0.index() == 2);
		REQUIRE(v2.index() == 2);

		REQUIRE(*tools::get_if<2>(&v0) == long_string + long_string);
		REQUIRE(*tools::get_if<2>(&v2) == long_string + long_string);
	}

	SECTION("Testing copy assignment, trivial types")
	{
		constexpr_var_t v0(2);
		constexpr_var_t v1(27);
		const constexpr_var_t v2('c');

		v0 = v1;

		REQUIRE(v0.index() == 1);
		REQUIRE(v1.index() == 1);

		REQUIRE(*tools::get_if<1>(&v0) == 27);
		REQUIRE(*tools::get_if<1>(&v1) == 27);

		v0 = v2;

		REQUIRE(v0.index() == 2);
		REQUIRE(*tools::get_if<2>(&v0) == 'c');
	}

	SECTION("Testing move assignment, non trivial types")
	{
		var_t v0(27);

		REQUIRE(v0.index() == 1);
		REQUIRE(*tools::get_if<1>(&v0) == 27);

		v0 = var_t(long_string);

		REQUIRE(v0.index() == 2);
		REQUIRE(*tools::get_if<2>(&v0) == long_string);

		v0 = var_t(long_string + long_string);

		REQUIRE(v0.index() == 2);
		REQUIRE(*tools::get_if<2>(&v0) == long_string + long_string);

		var_t v1('c');

		v0 = std::move(v1);

		REQUIRE(v0.index() == 3);
		REQUIRE(*tools::get_if<3>(&v0) == 'c');
	}

	SECTION("Testing move assignment, trivial types")
	{
		constexpr_var_t v0(27);

		REQUIRE(v0.index() == 1);
		REQUIRE(*tools::get_if<1>(&v0) == 27);

		v0 = constexpr_var_t(72);

		REQUIRE(v0.index() == 1);
		REQUIRE(*tools::get_if<1>(&v0) == 72);

		v0 = constexpr_var_t(tools::in_place<3>, 12.3);

		REQUIRE(v0.index() == 3);
		REQUIRE(*tools::get_if<3>(&v0) == 12.3);

		constexpr_var_t v1('c');
		v0 = std::move(v1);

		REQUIRE(v0.index() == 2);
		REQUIRE(*tools::get_if<2>(&v0) == 'c');
	}

	SECTION("Testing move assignment, special types (reference_wrapper, unique_ptr...)")
	{
		double val0 = 12.3;

		pointer_var_t v0{ std::reference_wrapper<double> (val0) };
		pointer_var_t v1{ std::make_unique<std::string>(long_string) };

		pointer_var_t v3{ std::make_unique<std::string>(long_string) };
		pointer_var_t v4{ std::make_unique<std::string>(long_string) };

		std::string s0("Hello");

		pointer_var_t v0_m = std::move(v0);
		pointer_var_t v1_m = std::move(v1);
		pointer_var_t v2_m = pointer_var_t{ std::make_unique<std::string>(long_string) };
		pointer_var_t v3_m{ std::reference_wrapper<double>(val0) };
		pointer_var_t v4_m{ std::make_unique<std::string>(s0) };

		v3_m = std::move(v3);
		v4_m = std::move(v4);

		REQUIRE(v0_m.index() == 0);
		REQUIRE(v1_m.index() == 1);
		REQUIRE(v2_m.index() == 1);
		REQUIRE(v3_m.index() == 1);
		REQUIRE(v4_m.index() == 1);

		REQUIRE(*tools::get_if<0>(&v0_m) == 12.3);
		REQUIRE(**tools::get_if<1>(&v1_m) == long_string);
		REQUIRE(**tools::get_if<1>(&v2_m) == long_string);
		REQUIRE(**tools::get_if<1>(&v3_m) == long_string);
		REQUIRE(**tools::get_if<1>(&v4_m) == long_string);
	}

	SECTION("Testing converting assignment, non trivial types")
	{
		var_t v0(27);

		v0 = long_string;
		REQUIRE(v0.index() == 2);
		REQUIRE(*tools::get_if<2>(&v0) == long_string);

		v0 = 'q';
		REQUIRE(v0.index() == 3);
		REQUIRE(*tools::get_if<3>(&v0) == 'q');
	}

	SECTION("Testing converting assignment, trivial types")
	{
		constexpr_var_t v0(27);

		v0 = 45;
		REQUIRE(v0.index() == 1);
		REQUIRE(*tools::get_if<1>(&v0) == 45);

		v0 = 'q';
		REQUIRE(v0.index() == 2);
		REQUIRE(*tools::get_if<2>(&v0) == 'q');
	}

	SECTION("Testing converting assignment, special types (reference_wrapper, unique_ptr...)")
	{
		double val0 = 12.3;
		double val1 = 27.3;
		int val2 = 27;

		pointer_var_t v0{ std::reference_wrapper<double>(val0) };
		pointer_var_t v1{ std::reference_wrapper<double>(val0) };
		pointer_var_t v2{ std::reference_wrapper<double>(val0) };

		v0 = std::reference_wrapper<double>(val1);
		v1 = std::move(std::make_unique<std::string>(long_string));
		v2 = &val2;

		REQUIRE(v0.index() == 0);
		REQUIRE(v1.index() == 1);
		REQUIRE(v2.index() == 3);

		REQUIRE(*tools::get_if<0>(&v0) == val1);
		REQUIRE(**tools::get_if<1>(&v1) == long_string);
		REQUIRE(**tools::get_if<3>(&v2) == val2);
	}

	SECTION("Testing constexpr get")
	{
		constexpr const constexpr_var_t v0(tools::in_place<0>, 3.14);
		constexpr const constexpr_var_t v1(tools::in_place<1>, 27);
		constexpr const constexpr_var_t v2(tools::in_place<2>, 'a');
		constexpr const constexpr_var_t v3(tools::in_place<3>, 99.9);
		constexpr constexpr_var_t v4(tools::in_place<0>, 3.14);

		static_assert(tools::get<0>(v0) == 3.14, "get has to return the correct value.");
		static_assert(tools::get<1>(v1) == 27, "get has to return the correct value.");
		static_assert(tools::get<2>(v2) == 'a', "get has to return the correct value.");
		static_assert(tools::get<3>(v3) == 99.9, "get has to return the correct value.");
		static_assert(tools::get<0>(v4) == 3.14, "get has to return the correct value.");

		static_assert(tools::get<int>(v1) == 27, "get has to return the correct value.");
		static_assert(tools::get<char>(v2) == 'a', "get has to return the correct value.");

#if defined(_MSC_VER) && _MSC_VER >= 1910 || !defined(_MSC_VER)
		static_assert(tools::get<0>(constexpr_var_t(tools::in_place<0>, 3.14)) == 3.14, "get has to return the correct value.");
		static_assert(tools::get<1>(constexpr_var_t(tools::in_place<1>, 27)) == 27, "get has to return the correct value.");
		static_assert(tools::get<2>(constexpr_var_t(tools::in_place<2>, 'a')) == 'a', "get has to return the correct value.");
		static_assert(tools::get<3>(constexpr_var_t(tools::in_place<3>, 99.9)) == 99.9, "get has to return the correct value.");

		constexpr constexpr_var_t v5(tools::in_place<0>, 3.14);
		constexpr auto val0 = tools::get<0>(std::move(v5));

		static_assert(val0 == 3.14, "get has to return the correct value.");

		static_assert(tools::get<int>(constexpr_var_t(tools::in_place<1>, 27)) == 27, "get has to return the correct value.");
		static_assert(tools::get<char>(constexpr_var_t(tools::in_place<2>, 'a')) == 'a', "get has to return the correct value.");
#endif
	}

	SECTION("Testing get")
	{
		// l-value ref
		{
			var_t v0(tools::in_place<0>, 3.14);
			var_t v1(tools::in_place<1>, 27);
			var_t v2(tools::in_place<2>, "Hello");
			var_t v3(tools::in_place<3>, 'a');
			var_t v4(tools::in_place<4>, 99.9);

			auto& ref0 = tools::get<0>(v0);
			auto& ref1 = tools::get<1>(v1);
			auto& ref2 = tools::get<2>(v2);
			auto& ref3 = tools::get<3>(v3);
			auto& ref4 = tools::get<4>(v4);

			static_assert(std::is_same<decltype(ref0), double&>::value == true, "get has to return reference type.");
			static_assert(std::is_same<decltype(ref1), int&>::value == true, "get has to return reference type.");
			static_assert(std::is_same<decltype(ref2), std::string&>::value == true, "get has to return reference type.");
			static_assert(std::is_same<decltype(ref3), char&>::value == true, "get has to return reference type.");
			static_assert(std::is_same<decltype(ref4), double&>::value == true, "get has to return reference type.");

			REQUIRE(ref0 == 3.14);
			REQUIRE(ref1 == 27);
			REQUIRE(ref2 == "Hello");
			REQUIRE(ref3 == 'a');
			REQUIRE(ref4 == 99.9);

			auto& ref5 = tools::get<int>(v1);
			auto& ref6 = tools::get<std::string>(v2);
			auto& ref7 = tools::get<char>(v3);

			static_assert(std::is_same<decltype(ref5), int&>::value == true, "get has to return reference type.");
			static_assert(std::is_same<decltype(ref6), std::string&>::value == true, "get has to return reference type.");
			static_assert(std::is_same<decltype(ref7), char&>::value == true, "get has to return reference type.");

			REQUIRE(ref5 == 27);
			REQUIRE(ref6 == "Hello");
			REQUIRE(ref7 == 'a');
		}

		// r-value ref
		{
			auto v0 = var_t(tools::in_place<2>, long_string);

			auto val0 = tools::get<0>(var_t(tools::in_place<0>, 3.14));
			auto val1 = tools::get<1>(var_t(tools::in_place<1>, 27));
			auto val2 = tools::get<2>(var_t(tools::in_place<2>, long_string));
			auto val3 = tools::get<3>(var_t(tools::in_place<3>, 'a'));
			auto val4 = tools::get<4>(var_t(tools::in_place<4>, 99.9));
			auto val5 = tools::get<2>(std::move(v0));
			auto val6 = tools::get<2>(v0);

			static_assert(std::is_same<decltype(val0), double>::value == true, "get has to return value type.");
			static_assert(std::is_same<decltype(val1), int>::value == true, "get has to return value type.");
			static_assert(std::is_same<decltype(val2), std::string>::value == true, "get has to return value type.");
			static_assert(std::is_same<decltype(val3), char>::value == true, "get has to return value type.");
			static_assert(std::is_same<decltype(val4), double>::value == true, "get has to return value type.");
			static_assert(std::is_same<decltype(val5), std::string>::value == true, "get has to return value type.");
			static_assert(std::is_same<decltype(val6), std::string>::value == true, "get has to return value type.");

			REQUIRE(val0 == 3.14);
			REQUIRE(val1 == 27);
			REQUIRE(val2 == long_string);
			REQUIRE(val3 == 'a');
			REQUIRE(val4 == 99.9);
			REQUIRE(val5 == long_string);
			REQUIRE(val6 == "");

			auto v1 = var_t(tools::in_place<2>, long_string);

			auto val7 = tools::get<int>(var_t(tools::in_place<1>, 27));
			auto val8 = tools::get<std::string>(var_t(tools::in_place<2>, long_string));
			auto val9 = tools::get<char>(var_t(tools::in_place<3>, 'a'));
			auto val10 = tools::get<std::string>(std::move(v1));
			auto val11 = tools::get<std::string>(v1);

			static_assert(std::is_same<decltype(val7), int>::value == true, "get has to return value type.");
			static_assert(std::is_same<decltype(val8), std::string>::value == true, "get has to return value type.");
			static_assert(std::is_same<decltype(val9), char>::value == true, "get has to return value type.");
			static_assert(std::is_same<decltype(val10), std::string>::value == true, "get has to return value type.");
			static_assert(std::is_same<decltype(val11), std::string>::value == true, "get has to return value type.");

			REQUIRE(val7 == 27);
			REQUIRE(val8 == long_string);
			REQUIRE(val9 == 'a');
			REQUIRE(val10 == long_string);
			REQUIRE(val11 == "");
		}

		// Const l-value ref
		{
			const var_t v0(tools::in_place<0>, 3.14);
			const var_t v1(tools::in_place<1>, 27);
			const var_t v2(tools::in_place<2>, "Hello");
			const var_t v3(tools::in_place<3>, 'a');
			const var_t v4(tools::in_place<4>, 99.9);

			const auto& ref0 = tools::get<0>(v0);
			const auto& ref1 = tools::get<1>(v1);
			const auto& ref2 = tools::get<2>(v2);
			const auto& ref3 = tools::get<3>(v3);
			const auto& ref4 = tools::get<4>(v4);

			static_assert(std::is_same<decltype(ref0), const double&>::value == true, "get has to return reference type.");
			static_assert(std::is_same<decltype(ref1), const int&>::value == true, "get has to return reference type.");
			static_assert(std::is_same<decltype(ref2), const std::string&>::value == true, "get has to return reference type.");
			static_assert(std::is_same<decltype(ref3), const char&>::value == true, "get has to return reference type.");
			static_assert(std::is_same<decltype(ref4), const double&>::value == true, "get has to return reference type.");

			REQUIRE(ref0 == 3.14);
			REQUIRE(ref1 == 27);
			REQUIRE(ref2 == "Hello");
			REQUIRE(ref3 == 'a');
			REQUIRE(ref4 == 99.9);

			const auto& ref5 = tools::get<int>(v1);
			const auto& ref6 = tools::get<std::string>(v2);
			const auto& ref7 = tools::get<char>(v3);

			static_assert(std::is_same<decltype(ref5), const int&>::value == true, "get has to return reference type.");
			static_assert(std::is_same<decltype(ref6), const std::string&>::value == true, "get has to return reference type.");
			static_assert(std::is_same<decltype(ref7), const char&>::value == true, "get has to return reference type.");

			REQUIRE(ref5 == 27);
			REQUIRE(ref6 == "Hello");
			REQUIRE(ref7 == 'a');
		}
	}

	struct visitor_constexpr_t
	{
		constexpr std::size_t operator()(double d) const { return static_cast<std::size_t>(d); }
		constexpr std::size_t operator()(int i) const { return static_cast<std::size_t>(i); }
		constexpr std::size_t operator()(const char& c) const { return static_cast<std::size_t>(c); }
	};

#if defined(_MSC_VER) && _MSC_VER >= 1910 || !defined(_MSC_VER)
	SECTION("Testing constexpr visit")
	{
		constexpr visitor_constexpr_t visitor{};

		constexpr constexpr_var_t v1(tools::in_place<0>, 3.14);
		constexpr constexpr_var_t v2(tools::in_place<1>, 1);
		constexpr constexpr_var_t v3(tools::in_place<2>, 'x');
		constexpr constexpr_var_t v4(tools::in_place<0>, 6.28);

		constexpr auto res1 = tools::visit(visitor, v1);
		constexpr auto res2 = tools::visit(visitor, v2);
		constexpr auto res3 = tools::visit(visitor, v3);
		constexpr auto res4 = tools::visit(visitor, v4);

		static_assert(std::is_same<decltype(res1), const std::size_t>::value == true, "visit has to return the return type of the visitor.");
		static_assert(std::is_same<decltype(res2), const std::size_t>::value == true, "visit has to return the return type of the visitor.");
		static_assert(std::is_same<decltype(res3), const std::size_t>::value == true, "visit has to return the return type of the visitor.");
		static_assert(std::is_same<decltype(res4), const std::size_t>::value == true, "visit has to return the return type of the visitor.");

		static_assert(res1 == static_cast<std::size_t>(3.14), "visit has to return the return value of the visitor.");
		static_assert(res2 == static_cast<std::size_t>(1), "visit has to return the return value of the visitor.");
		static_assert(res3 == static_cast<std::size_t>('x'), "visit has to return the return value of the visitor.");
		static_assert(res4 == static_cast<std::size_t>(6.28), "visit has to return the return value of the visitor.");
	}
#endif

	SECTION("Testing l-value visit")
	{
		auto addTwoVisitor = [](auto& value) {
			value += 2;
		};

		var_t v1(tools::in_place<0>, 3.14);
		var_t v2(tools::in_place<1>, 27);
		var_t v3(tools::in_place<2>, long_string);
		var_t v4(tools::in_place<3>, 'x');

		tools::visit(addTwoVisitor, v1);
		tools::visit(addTwoVisitor, v2);
		tools::visit(addTwoVisitor, v3);
		tools::visit(addTwoVisitor, v4);

		std::string long_string_2 = long_string;
		long_string_2 += 2;

		REQUIRE(tools::get<0>(v1) == 3.14 + 2);
		REQUIRE(tools::get<1>(v2) == 29);
		REQUIRE(tools::get<2>(v3) == long_string_2);
		REQUIRE(tools::get<3>(v4) == 'x' + 2);
	}

	SECTION("Testing const l-value visit")
	{
		auto toStringVisitor = [](auto value) {
			std::ostringstream strs;
			strs << value;
			return strs.str();
		};

		const var_t v1(tools::in_place<1>, 27);
		const var_t v2(tools::in_place<2>, long_string);
		const var_t v3(tools::in_place<3>, 'x');

		REQUIRE(tools::visit(toStringVisitor, v1) == "27");
		REQUIRE(tools::visit(toStringVisitor, v2) == long_string);
		REQUIRE(tools::visit(toStringVisitor, v3) == "x");
	}

	SECTION("Testing, special types (reference_wrapper, unique_ptr...)")
	{
		auto address_to_str_visitor = [](auto& value) {
			std::ostringstream strs;
			strs << std::addressof(value);
			return strs.str();
		};

		auto ptr_to_str = [](void* ptr)
		{
			std::ostringstream strs;
			strs << ptr;
			return strs.str();
		};

		double val1 = 12.3;
		int val4 = 27;

		pointer_var_t v1{ std::reference_wrapper<double>(val1) };
		pointer_var_t v2{ std::make_unique<std::string>(long_string) };
		pointer_var_t v3{ std::make_shared<double>(45.2) };
		pointer_var_t v4(&val4);

		REQUIRE(tools::visit(address_to_str_visitor, v1) == ptr_to_str(static_cast<void*>(tools::get_if<0>(&v1))));
		REQUIRE(tools::visit(address_to_str_visitor, v2) == ptr_to_str(static_cast<void*>(tools::get_if<1>(&v2))));
		REQUIRE(tools::visit(address_to_str_visitor, v3) == ptr_to_str(static_cast<void*>(tools::get_if<2>(&v3))));
		REQUIRE(tools::visit(address_to_str_visitor, v4) == ptr_to_str(static_cast<void*>(tools::get_if<3>(&v4))));

		struct
		{
			std::string s;

			void operator()(double& val)
			{
				val = 23.3;
			}

			void operator()(std::unique_ptr<std::string>& val)
			{
				*val = s + s;
			}

			void operator()(std::shared_ptr<double>& val)
			{
				*val = 45.1;
			}

			void operator()(int*& val)
			{
				*val = 27;
			}
		} visitor{long_string};

		tools::visit(visitor, v1);
		tools::visit(visitor, v2);
		tools::visit(visitor, v3);
		tools::visit(visitor, v4);

		REQUIRE(*tools::get_if<0>(&v1) == 23.3);
		REQUIRE(**tools::get_if<1>(&v2) == long_string + long_string);
		REQUIRE(**tools::get_if<2>(&v3) == 45.1);
		REQUIRE(**tools::get_if<3>(&v4) == 27);
	 }
}
