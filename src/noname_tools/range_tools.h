#pragma once

#include <utility>
#include <type_traits>
#include <iostream>

namespace noname
{
	namespace tools
	{
		//! Range with a begin() and end() method to use range-based for loops with any pair of iterators, sentinel version
		template <typename begin_t, typename end_t = void>
		class iterator_range
		{
		public:
			typedef begin_t begin_type;
			typedef end_t end_type;

			//! Construct a range with the specified begin and end iterators
			iterator_range(begin_type first, end_type last)
				: first(std::move(first))
				  , last(std::move(last))
			{
				static_assert(std::is_convertible<decltype(first != last), bool>::value,
					"Error: Expression [first != last] must return a type convertible to bool in order to use the range object for range based for loops!");
			}

			//! Returns begin of the range
			begin_type begin() const
			{
				return first;
			}

			//! Returns end of the range
			end_type end() const
			{
				return last;
			}

		private:
			begin_type first;
			end_type last;
		};

		//! Range with a begin() and end() method to use range-based for loops with any pair of iterators
		template <typename iterator_t>
		class iterator_range<iterator_t, void>
		{
		public:
			//! Iterator type of the range
			typedef iterator_t begin_type;
			typedef iterator_t end_type;

			//! Construct a range with the specified begin and end iterators
			iterator_range(begin_type first, end_type last)
				: first(std::move(first))
				  , last(std::move(last))
			{
			}

			//! Returns begin of the range
			begin_type begin() const
			{
				return first;
			}

			//! Returns end of the range
			end_type end() const
			{
				return last;
			}

		private:
			begin_type first;
			end_type last;
		};

		//! Convenience constructor for range objects
		template <typename begin_t, typename end_t>
		iterator_range<typename std::decay<begin_t>::type, typename std::decay<end_t>::type> make_range(begin_t&& begin, end_t&& end)
		{
			return iterator_range<typename std::decay<begin_t>::type, typename std::decay<end_t>::type>(std::forward<begin_t>(begin), std::forward<end_t>(end));
		}
	}
}
