#pragma once

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

#include <iterator>
#include <algorithm>

#include "functional_tools.h"

namespace noname {
    namespace tools {
        // TODO: Implement method to split range into unique and non-unique elements

        //! Divides a range in n (nearly) equal sized subranges and writes every subrange's begin and end iterator into dest without duplicates (i.e. dest will have n+1 entries)
        template<typename InputIt, typename OutputIt>
        void n_subranges(InputIt first, InputIt last, OutputIt dest, std::size_t n) {
            // Inspired from: https://codereview.stackexchange.com/questions/106773/dividing-a-range-into-n-sub-ranges

            static_assert(std::is_same<InputIt, typename OutputIt::container_type::value_type>::value,
                          "Error: The output iterator value type has to be the same as the input iterator type!");

            if (n == 0) return;
            if (first == last) return;

            const auto dist = std::distance(first, last);
            n = std::min<size_t>(n, dist);
            const auto chunk = dist / n;
            auto remainder = dist % n;

            *dest++ = first;

            for (size_t i = 0; i < n - 1; i++) {
                first = std::next(first, chunk + (remainder ? 1 : 0));
                *dest++ = first;

                if (remainder) remainder -= 1;
            }

            *dest++ = last;
        }

        //! Applies the given function object to every element and its successor, returns copy/move of functor
        template<typename InputIt, typename Func>
        Func for_each_and_successor(InputIt first, InputIt last, Func f) {
            if (first != last) {
                auto next = std::next(first);
                for (; next != last; ++next) {
                    f(*first, *next);
                    first = next;
                }
            }

            return std::move(f);
        }

        //! Returns the first element in the specified range that is unequal to its predecessor, uses not-equal (!=) operator for comparison
        template<typename InputIt>
        InputIt find_unequal_successor(InputIt first, InputIt last) {
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

        //! Returns the first element in the specified range that is unequal to its predecessor, uses p to compare two elements for inequality
        template<typename InputIt, typename BinaryPredicate>
        InputIt find_unequal_successor(InputIt first, InputIt last, BinaryPredicate p) {
            if (first != last) {
                InputIt next = std::next(first);
                while (next != last) {
                    if (p(*first, *next)) return next;
                    first = next;
                    ++next;
                }
            }
            return last;
        }

        //! Copies the elements from the specified range to dest in such a way that all groups of consecutive equal objects are omitted
        template<typename InputIt, typename OutputIt>
        OutputIt strict_unique_copy(InputIt first, InputIt last, OutputIt dest) {
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
        template<typename InputIt, typename OutputIt, typename BinaryPredicate>
        OutputIt strict_unique_copy(InputIt first, InputIt last, OutputIt dest, BinaryPredicate p) {
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

        namespace _detail {
            //! Output iterator adapter which forwards to a callable
            template<typename Func>
            struct _output_iterator_adapter {
                using value_type = void;
                using difference_type = void;
                using pointer = void;
                using reference = void;
                using iterator_category = std::output_iterator_tag;

                //! The callable used for the output iterator
                callable_container<Func> f;

                //! Assignment operator to emulate output iterators, forwards its argument to the stored callable
                template<typename T,
                        /* Use SFINAE to avoid confusion with copy-assignment operator */
                        typename = typename std::enable_if<!std::is_same<typename std::decay<T>::type, _output_iterator_adapter>::value>::type>
                _output_iterator_adapter &operator=(const T &value) {
                    f.callable(value);
                    return *this;
                }

                //! Assignment operator to emulate output iterators, forwards its argument to the stored callable
                template<typename T,
                        /* Use SFINAE to avoid confusion with copy-assignment operator */
                        typename = typename std::enable_if<!std::is_same<typename std::decay<T>::type, _output_iterator_adapter>::value>::type>
                _output_iterator_adapter &operator=(T &&value) {
                    f.callable(std::forward<T>(value));
                    return *this;
                }

                //! No-op, function is provided to satisfy the requirements of OutputIterator
                _output_iterator_adapter &operator*() { return *this; }

                //! No-op, function is provided to satisfy the requirements of OutputIterator
                _output_iterator_adapter &operator++() { return *this; }

                //! No-op, function is provided to satisfy the requirements of OutputIterator
                _output_iterator_adapter &operator++(int) { return *this; }
            };
        }

        //! Returns an OutputIterator like type which forwards output assignments to the supplied callable.
        /*
        * Construction, copy construction, destruction etc. of the callable may not have any side effects.
        */
        template<typename Func>
        auto make_output_iterator_adapter(Func f) {
            return _detail::_output_iterator_adapter<typename std::decay<Func>::type>{std::forward<Func>(f)};
        }
    }
}
