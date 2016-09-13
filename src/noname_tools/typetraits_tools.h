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

#pragma once

namespace noname
{
	namespace tools
	{
		namespace _detail
		{
			//! Helper used for void_t and required by MSVC
			template <class...>
			struct make_void { using type = void; };
		}

		//! Utility metafunction that maps a sequence of any types to the type void
		template <typename... T>
		using void_t = typename _detail::make_void<T...>::type;

		//! Helper alias template for std::integral_constant for the common case where T is bool.
		template <bool B>
		using bool_constant = std::integral_constant<bool, B>;

		//! Forms the logical negation of the type trait B.
		template<class B>
		struct negation : bool_constant<!B::value> {};

		//! Forms the logical conjunction of the type traits B..., effectively performing a logical AND on the sequence of traits.
		template<class...> struct conjunction : std::true_type {};
		template<class B1> struct conjunction<B1> : B1 {};
		template<class B1, class... Bn>
		struct conjunction<B1, Bn...> : std::conditional_t<B1::value != false, conjunction<Bn...>, B1> {};

		//! Forms the logical disjunction of the type traits B..., effectively performing a logical or on the sequence of traits.
		template<class...> struct disjunction : std::false_type {};
		template<class B1> struct disjunction<B1> : B1 {};
		template<class B1, class... Bn>
		struct disjunction<B1, Bn...> : std::conditional_t<B1::value != false, B1, disjunction<Bn...>> {};

		namespace _detail
		{
			template<template<class...> class Z, class = void, class...Ts>
			struct is_well_formed : std::false_type {};

			template<template<class...> class Z, class...Ts>
			struct is_well_formed<Z, void_t<Z<Ts...>>, Ts...> : std::true_type {};
		}

		//! Checks if the supplied template template argument Z is well formed after substituting the supplied template arguments Ts into it. Is either std::true_type or std::false_type.
		template<template<class...> class Z, class...Ts>
		using is_well_formed = _detail::is_well_formed<Z, void, Ts...>;

		namespace _detail
		{
			//! Unevaluated reference T&
			template<class T>
			using reference_t = decltype(std::declval<T&>());
		}

		template<class T>
		using is_referenceable = is_well_formed<_detail::reference_t, T>;
	}
}