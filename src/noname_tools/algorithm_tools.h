#pragma once

#include <iterator>

namespace noname {

	namespace tools {

		//! Copies the elements from the specified range to dest in such a way that all groups of consecutive equal objects are omitted
		template< class InputIt, class OutputIt, class BinaryPredicate >
		OutputIt strict_unique_copy(InputIt first, InputIt last, OutputIt dest, BinaryPredicate p)
		{
			// Return if input range is empty
			if(first == last) return dest;

			// Variable for result of previous comparison
			bool prev_check = false;
			// Current comparison result
			bool cur_check = false;
			// Next element of the range
			auto next = std::next(first);

			while(next != last) {
				// Compare current and next element
				cur_check = p(*first,*next);
				// Copy if presceding and next elment are not equal to current elemnt
				if(!prev_check && !cur_check) {
					*dest++ = *first;
				}

				// Store current result as previous
				std::swap(prev_check, cur_check);
				// Use next elment as current and get it to next element
				first = next;
				next = std::next(first);
			}

			// Copy last element
			if(!prev_check) *dest++ = *first;

			return dest;
		}

	}

}
