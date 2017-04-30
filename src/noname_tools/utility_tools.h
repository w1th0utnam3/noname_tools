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

#include <cstdio>
#include <type_traits>

#define _NONAME_ASSERT(x, m) ((x) ? (void)0 : (std::fprintf(stderr, "%s\n", m), std::abort()))

namespace noname
{
	namespace tools
	{
		// TODO: Rename nth_element because there is std::nth_element algorithm

		//! Type used by nth_element to indicate that the type index is out of range of the parameter pack
		struct out_of_range_t {};

		namespace _detail
		{
			// Null type
			struct dummy_t {};

			template <bool, std::size_t, typename T, typename... Ts>
			struct _nth_element;

			template <std::size_t I, typename T, typename ...Ts>
			struct _nth_element<false, I, T, Ts...>
			{
				using type = out_of_range_t;
			};

			// For efficiency see: http://ldionne.com/2015/11/29/efficient-parameter-pack-indexing/
			template <std::size_t I, typename T, typename ...Ts>
			struct _nth_element<true, I, T, Ts...>
			{
				using type = typename _nth_element<true, I - 1, Ts...>::type;
			};

			template <typename T, typename ...Ts>
			struct _nth_element<true, 0, T, Ts...>
			{
				using type = T;
			};
		} // namespace _detail

		//! Alias for the I-th element of Ts
		template <std::size_t I, typename ...Ts>
		struct nth_element : _detail::_nth_element<I <= sizeof...(Ts), I, Ts...>
		{
		};

		template <std::size_t I, typename ...Ts>
		using nth_element_t = typename nth_element<I, Ts...>::type;

		//! Value used by element_index to indicate that the type was not found in the parameter pack.
		constexpr std::size_t element_not_found = -1;

		namespace _detail
		{
			template <std::size_t I, typename... Ts>
			struct _element_index;

			template <std::size_t I, typename T>
			struct _element_index<I, T> : std::integral_constant<std::size_t, element_not_found>
			{
			};

			template <std::size_t I, typename T, typename U, typename... Us>
			struct _element_index<I, T, U, Us...> : std::conditional_t<std::is_same<T,U>::value, std::integral_constant<std::size_t, I>
																							, _element_index<I+1, T, Us...>>
			{
			};
		} // namespace _detail

		//! Returns the index of the first occurrence of 'T' in 'Ts...' or element_not_found.
		template <typename T, typename... Ts>
		struct element_index : _detail::_element_index<std::size_t(0), T, Ts...>
		{
		};

		template <typename T, typename... Ts>
		constexpr std::size_t element_index_v = element_index<T, Ts...>::value;

		namespace _detail {
			template<typename... Ts>
			struct _count;

			template<typename T>
			struct _count<T> : std::integral_constant<std::size_t, 0>
			{
			};

			template<typename T, typename U, typename... Us>
			struct _count<T, U, Us...> : std::integral_constant<std::size_t, std::conditional_t<std::is_same<T, U>::value, std::integral_constant<std::size_t, 1>
																														 , std::integral_constant<std::size_t, 0>
																			 >::value
															+ _count<T, Us...>::value>
			{
			};
		} // namespace _detail

		//! Counts the number of occurrences of 'T' in 'Ts...'.
		template<typename T, typename... Ts>
		struct count_element : _detail::_count<T, Ts...>
		{
		};

		template <typename T, typename... Ts>
		constexpr std::size_t count_element_v = count_element<T, Ts...>::value;

		namespace _detail
		{
			template <typename... Ts>
			struct _unique;

			template <>
			struct _unique<> : std::false_type
			{
			};

			template <typename T>
			struct _unique<T> : std::true_type
			{
			};

			template <typename T, typename... Ts>
			struct _unique<T, Ts...> : std::integral_constant<bool, (count_element_v<T, Ts...> == 0) && _unique<Ts...>::value>
			{
			};
		}

		//! Checks whether every type occurrs only once in the parameter pack
		template <typename... Ts>
		struct unique_types : _detail::_unique<Ts...>
		{
		};

		template <typename... Ts>
		constexpr bool unique_types_v = unique_types<Ts...>::value;

		namespace _detail
		{
			template <typename... Ts> struct overload;
			template <> struct overload<> { void operator()(); };

			//! Recursively defines an 'T operator()(T) const' for every 'T' in 'Ts...'
			template <typename T, typename... Ts>
			struct overload<T, Ts...> : overload<Ts...>
			{
				using overload<Ts...>::operator();
				T operator()(T);
			};

			template <typename... Ts>
			struct overload<void, Ts...> : overload<Ts...>
			{
				using overload<Ts...>::operator();
				void operator()();
			};

			template <typename T, typename... Ts>
			using best_match_impl = std::enable_if_t<(count_element<T, Ts...>::value == 1), T>;
		} // namespace _detail

		// Applies overloading rules to find the unique best conversion from 'T' to any type of 'Ts...'. Based on http://stackoverflow.com/a/39548402/929037.
		template <typename T, typename... Ts>
		using best_match = _detail::best_match_impl<std::result_of_t<_detail::overload<Ts...>(T)>, Ts...>;

		namespace _detail
		{
			struct __in_place_tag {};
			template <class> struct __in_place_type_tag {};
			template <std::size_t> struct __in_place_index_tag {};
		}
		
		//! Disambiguation tag to create an optional, any or variant in-place. Actually calling any of the in_place functions results in undefined behavior.
		struct in_place_tag;
		
		using in_place_t = in_place_tag(&)(_detail::__in_place_tag);
		template <class _Tp>
		using in_place_type_t = in_place_tag(&)(_detail::__in_place_type_tag<_Tp>);
		template <std::size_t _Nx>
		using in_place_index_t = in_place_tag(&)(_detail::__in_place_index_tag<_Nx>);
		
		struct in_place_tag
		{
			in_place_tag() = delete;
			private:
				explicit in_place_tag(_detail::__in_place_tag) {}
				
				friend inline in_place_tag in_place(_detail::__in_place_tag __t);

				template <class _Tp>
				friend inline in_place_tag in_place(_detail::__in_place_type_tag<_Tp>);

				template <std::size_t _Nx>
				friend inline in_place_tag in_place(_detail::__in_place_index_tag<_Nx>);
				
		};
		
		inline in_place_tag in_place(_detail::__in_place_tag __t)
		{
			_NONAME_ASSERT(false, "The in_place function cannot be invoked");
		}
		
		template <class _Tp>
		inline in_place_tag in_place(_detail::__in_place_type_tag<_Tp>)
		{
			_NONAME_ASSERT(false, "The in_place function cannot be invoked");
			return in_place_tag(_detail::__in_place_tag{});
		}

		template <std::size_t _Nx>
		inline in_place_tag in_place(_detail::__in_place_index_tag<_Nx>)
		{
			_NONAME_ASSERT(false, "The in_place function cannot be invoked");
			return in_place_tag(_detail::__in_place_tag{});
		}
	}
}

#undef _NONAME_ASSERT
