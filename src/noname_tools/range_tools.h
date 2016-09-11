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
#include <type_traits>
#include <iterator>

namespace noname
{
	namespace tools
	{
		//! Range object with begin() and end() methods to use range-based for loops with any pair of iterators, sentinel version
		template <typename begin_t, typename end_t = void>
		class iterator_range
		{
		public:
			typedef begin_t begin_type;
			typedef end_t end_type;

			//! Construct a range with the specified begin and end iterators
			constexpr iterator_range(begin_type first, end_type last)
				: first(std::move(first))
				  , last(std::move(last))
			{
				static_assert(std::is_convertible<decltype(first != last), bool>::value,
					"Error: Expression [first != last] must return a type convertible to bool in order to use the range object for range based for loops!");
			}

			//! Returns begin of the range
			constexpr begin_type begin() const
			{
				return first;
			}

			//! Returns end of the range
			constexpr end_type end() const
			{
				return last;
			}

		private:
			begin_type first;
			end_type last;
		};

		//! Range object with begin() and end() methods to use range-based for loops with any pair of iterators
		template <typename iterator_t>
		class iterator_range<iterator_t, void>
		{
		public:
			//! Iterator type of the range
			typedef iterator_t begin_type;
			typedef iterator_t end_type;

			//! Construct a range with the specified begin and end iterators
			constexpr iterator_range(begin_type first, end_type last)
				: first(std::move(first))
				  , last(std::move(last))
			{
			}

			//! Returns begin of the range
			constexpr begin_type begin() const
			{
				return first;
			}

			//! Returns end of the range
			constexpr end_type end() const
			{
				return last;
			}

			//! Returns the size of the range
			size_t size() const
			{
				return std::distance(first, last);
			}

		private:
			begin_type first;
			end_type last;
		};

		//! Creates an iterator_range object, deducing the target type from the types of arguments
		template <typename begin_t, typename end_t>
		constexpr iterator_range<typename std::decay<begin_t>::type, typename std::decay<end_t>::type> make_range(begin_t&& begin, end_t&& end)
		{
			return iterator_range<typename std::decay<begin_t>::type, typename std::decay<end_t>::type>(std::forward<begin_t>(begin), std::forward<end_t>(end));
		}
	}
}
