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

#include <utility>
#include <algorithm>

namespace noname
{
	namespace tools
	{
		namespace _detail
		{
			//! Helper used for void_t and required by MSVC
			template <class...>
			struct _make_void { using type = void; };
		}

		using namespace _detail;

		//! Utility metafunction that maps a sequence of any types to the type void
		template <typename... T>
		using void_t = typename _detail::_make_void<T...>::type;

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
			struct _is_well_formed : std::false_type {};

			template<template<class...> class Z, class...Ts>
			struct _is_well_formed<Z, void_t<Z<Ts...>>, Ts...> : std::true_type {};
		}

		//! Checks if the supplied template template argument Z is well formed after substituting the supplied template arguments Ts into it. Is either std::true_type or std::false_type.
		template<template<class...> class Z, class...Ts>
		using is_well_formed = _detail::_is_well_formed<Z, void, Ts...>;

		namespace _detail
		{
			//! Unevaluated reference T&
			template<class T>
			using _reference_t = decltype(std::declval<T&>());

			//! Unevaluated swap(T&,U&)
			template<class T, class U>
			using _swap_with_overload = decltype(swap(std::declval<T&>(), std::declval<U&>()));

			//! Unevaluated std::swap(T&,U&)
			template<class T, class U>
			using _swap_with_std = decltype(std::swap(std::declval<T&>(), std::declval<U&>()));

			//! Checks if swap(std::declval<T>(), std::declval<U>()) is a valid expression
			template<class T, class U>
			using _is_swappable_with_overload = is_well_formed<_swap_with_overload, T, U>;

			//! Checks if std::swap(std::declval<T>(), std::declval<U>()) is a valid expression
			template<class T, class U>
			using _is_swappable_with_std = is_well_formed<_swap_with_std, T, U>;

			//! Checks if swap(std::declval<T>(), std::declval<U>()) or std::swap(std::declval<T>(), std::declval<U>()) is a valid expression
			template<class T, class U>
			using _is_swappable_with_unidirectional = disjunction<_is_swappable_with_overload<T, U>, _is_swappable_with_std<T, U>>;
		}

		//! Checks if the supplied type is referenceable, i.e. whether T& is a well-formed type
		template<class T>
		using is_referenceable = is_well_formed<_detail::_reference_t, T>;

		//! Checks if the expressions swap(std::declval<T>(), std::declval<U>()) and swap(std::declval<U>(), std::declval<T>()) are both well formed after "using std::swap"
		template<class T, class U>
		using is_swappable_with = conjunction<_detail::_is_swappable_with_unidirectional<T, U>, _detail::_is_swappable_with_unidirectional<U, T>>;

		//! Checks if a type is referenceable and whether std::is_swappable_with<T&, T&>::value is true
		template<class T>
		using is_swappable = conjunction<is_referenceable<T>, is_swappable_with<T,T>>;
	}
}