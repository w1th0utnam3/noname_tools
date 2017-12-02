//	MIT License
//
//	Copyright (c) 2017 Fabian Löschner
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

#include <type_traits>
#include <utility>

#include "utility_tools.h"

#if defined(_MSC_VER)
#define NONAME_EBO __declspec(empty_bases)
#else
#define NONAME_EBO
#endif

// TODO: Allow non-unique typelists (we have to be careful with inheritance)

namespace noname
{
	namespace tools
	{
		template <typename T>
		struct type_alias
		{
			using type = T;
		};

		template <typename... Ts>
		struct NONAME_EBO typelist : public type_alias<Ts>... {};

		template <typename T>
		struct typelist_size;

		template <typename... Ts>
		struct typelist_size<typelist<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)>
		{};

		template <typename T>
		constexpr std::size_t typelist_size_v = typelist_size<T>::value;

		template <typename... Ts>
		constexpr std::size_t typelist_size_v<typelist<Ts...>> = sizeof...(Ts);

		template <std::size_t I, typename T>
		struct nth_typelist_element;

		template <std::size_t I, typename... Ts>
		struct nth_typelist_element<I, typelist<Ts...>>
		{
			using type = nth_element_t<I, Ts...>;
		};

		template <std::size_t I, typename T>
		using nth_typelist_element_t = typename nth_typelist_element<I, T>::type;

		namespace _detail
		{
			template <typename Typelist, typename F, std::size_t ...Indices>
			F typelist_for_each(Typelist&& tl, F f, std::index_sequence<Indices...>)
			{
				using swallow = int[];
				(void)swallow {
					1, (f(static_cast<type_alias<nth_typelist_element_t<Indices, typename std::decay<Typelist>::type>>>(tl)), void(), int{})...
				};
				return f;
			}
		}

		//! Calls a function for each element of a typelist in order and returns the function, the function is called with a type_alias<T> for each T from the list
		template <typename Typelist, typename F>
		F typelist_for_each(F f)
		{
			static constexpr Typelist tl;
			static constexpr std::size_t N(typelist_size_v<std::remove_reference_t<Typelist>>);
			return _detail::typelist_for_each(
				tl,
				std::forward<F>(f),
				std::make_index_sequence<N>{}
			);
		}
	}
}

#undef NONAME_EBO
