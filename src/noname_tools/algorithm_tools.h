#pragma once

#include <vector>
#include <iterator>

namespace noname
{
	namespace tools
	{
		// TODO: Implement method to split range into unique and non-unique elements

		//! Divides a range in n (nearly) equal sized subranges and writes every subrange's begin and end iterator into dest without duplicates (i.e. dest will have n+1 entries)
		template <typename InputIt, typename OutputIt>
		void n_subranges(InputIt first, InputIt last, OutputIt dest, std::size_t n)
		{
			// Inspired from: https://codereview.stackexchange.com/questions/106773/dividing-a-range-into-n-sub-ranges

			static_assert(std::is_same<InputIt, typename std::iterator_traits<OutputIt>::value_type>::value,
				"Error: The output iterator value type has to be the same as the input iterator type!");

			if (n == 0) return;

			const auto dist = std::distance(first, last);
			n = std::min<size_t>(n, dist);
			const auto chunk = dist / n;
			const auto remainder = dist % n;

			*dest++ = first;

			for (size_t i = 0; i < n - 1; i++) {
				first = std::next(first, chunk + (remainder ? 1 : 0));
				*dest++ = first;

				if (remainder) remainder -= 1;
			}

			*dest++ = last;
		}

		//! Returns the first element in the specified range that is unequal to its predecessor
		template <typename InputIt>
		InputIt find_unequal_successor(InputIt first, InputIt last)
		{
			if (first != last) {
				InputIt next = std::next(first);
				while (next != last) {
					if (*first != *next) return next;
					first = next;
					++next;
				}
			}
			return last;
		}

		//! Returns the first element in the specified range that is unequal to its predecessor, uses p to compare two elements for equality
		template <typename InputIt, typename BinaryPredicate>
		InputIt find_unequal_successor(InputIt first, InputIt last, BinaryPredicate p)
		{
			if (first != last) {
				InputIt next = std::next(first);
				while (next != last) {
					if (!p(*first, *next)) return next;
					first = next;
					++next;
				}
			}
			return last;
		}

		//! Copies the elements from the specified range to dest in such a way that all groups of consecutive equal objects are omitted
		template <typename InputIt, typename OutputIt>
		OutputIt strict_unique_copy(InputIt first, InputIt last, OutputIt dest)
		{
			// Return if input range is empty
			if (first == last) return dest;

			// Variable for result of previous comparison
			bool prev_check = false;
			// Current comparison result
			bool cur_check = false;
			// Next element of the range
			auto next = std::next(first);

			while (next != last) {
				// Compare current and next element
				cur_check = (*first == *next);
				// Copy if element does not belong to a group
				if (!prev_check && !cur_check) {
					*dest++ = *first;
				}

				prev_check = cur_check;
				first = next;
				next = std::next(first);
			}

			// Copy last element
			if (!prev_check) *dest++ = *first;

			return dest;
		}

		//! Copies the elements from the specified range to dest in such a way that all groups of consecutive equal objects are omitted, uses p to compare elements for equality
		template <typename InputIt, typename OutputIt, typename BinaryPredicate>
		OutputIt strict_unique_copy(InputIt first, InputIt last, OutputIt dest, BinaryPredicate p)
		{
			// Return if input range is empty
			if (first == last) return dest;

			// Variable for result of previous comparison
			bool prev_check = false;
			// Current comparison result
			bool cur_check = false;
			// Next element of the range
			auto next = std::next(first);

			while (next != last) {
				// Compare current and next element
				cur_check = p(*first, *next);
				// Copy if element does not belong to a group
				if (!prev_check && !cur_check) {
					*dest++ = *first;
				}

				prev_check = cur_check;
				first = next;
				next = std::next(first);
			}

			// Copy last element
			if (!prev_check) *dest++ = *first;

			return dest;
		}
	}
}
