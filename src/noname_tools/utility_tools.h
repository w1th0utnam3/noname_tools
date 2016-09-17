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
			// For efficiency see: http://ldionne.com/2015/11/29/efficient-parameter-pack-indexing/
			template <std::size_t I, typename T, typename ...Ts>
			struct nth_element_impl
			{
				using type = typename nth_element_impl<I - 1, Ts...>::type;
			};

			template <typename T, typename ...Ts>
			struct nth_element_impl<0, T, Ts...>
			{
				using type = T;
			};
		} // namespace _detail

		//! Alias for the I-th element of Ts
		template <std::size_t I, typename ...Ts>
		using nth_element = typename nth_element_impl<I, Ts...>::type;

		//! in_place_tag is an empty class type used as the return types of the in_place functions for disambiguation.
		struct in_place_tag { in_place_tag() = delete; };

// MSVC doesn't allow non-void methods without return - but we need it here
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4716)
#endif

		//! Disambiguation tag to create an optional, any or variant in-place. Actually calling any of the in_place functions results in undefined behavior.
		in_place_tag in_place() {};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

		using in_place_t = in_place_tag(&)();
	}
}