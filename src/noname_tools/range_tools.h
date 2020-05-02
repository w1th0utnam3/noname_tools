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

#include <utility>
#include <type_traits>
#include <iterator>
#include <memory>

#include "functional_tools.h"

// TODO: Tests for index_iterator

namespace noname {
    namespace tools {
        //! Range object with begin() and end() methods to use range-based for loops with any pair of iterators, sentinel version
        template<typename BeginT, typename EndT = void>
        class iterator_range {
        public:
            using begin_type = typename std::decay<BeginT>::type;
            using end_type = typename std::decay<EndT>::type;

            static_assert(
                    std::is_convertible<decltype(std::declval<begin_type>() != std::declval<end_type>()), bool>::value,
                    "Error: Expression [first != last] must return a type convertible to bool in order to use the range object for range based for loops!");

            //! Construct a range with the specified begin and end iterators
            constexpr iterator_range(begin_type first, end_type last)
                    : first(first), last(last) {
            }

            //! Construct a range with the specified begin and end iterators
            constexpr iterator_range(begin_type &&first, end_type &&last)
                    : first(std::move(first)), last(std::move(last)) {
            }

            //! Returns begin of the range
            constexpr begin_type begin() const {
                return first;
            }

            //! Returns end of the range
            constexpr end_type end() const {
                return last;
            }

        private:
            begin_type first;
            end_type last;
        };

        //! Range object with begin() and end() methods to use range-based for loops with any pair of iterators
        template<typename IteratorT>
        class iterator_range<IteratorT, void> {
        public:
            using begin_type = IteratorT;
            using end_type = IteratorT;

            //! Construct a range with the specified begin and end iterators
            constexpr iterator_range(begin_type first, end_type last)
                    : first(std::move(first)), last(std::move(last)) {
            }

            //! Returns begin of the range
            constexpr begin_type begin() const {
                return first;
            }

            //! Returns end of the range
            constexpr end_type end() const {
                return last;
            }

            //! Returns the size of the range
            std::size_t size() const {
                return std::distance(first, last);
            }

        private:
            begin_type first;
            end_type last;
        };

        //! Creates an iterator_range object, deducing the target type from the types of arguments
        template<typename BeginT, typename EndT>
        inline constexpr iterator_range<typename std::decay<BeginT>::type, typename std::decay<EndT>::type>
        make_range(BeginT &&begin, EndT &&end) {
            return iterator_range<typename std::decay<BeginT>::type, typename std::decay<EndT>::type>(
                    std::forward<BeginT>(begin), std::forward<EndT>(end));
        }

        //! Creates an iterator_range object from a statically allocated array
        template<typename T, std::size_t N>
        inline constexpr iterator_range<typename std::add_pointer<T>::type> make_range(T(&array)[N]) {
            return iterator_range<typename std::add_pointer<T>::type>(std::begin(array), std::end(array));
        }

        //! Creates an iterator_range object from an iterator and a range length, deducing the target type from the types of arguments
        template<typename IteratorT, typename DiffT = std::ptrdiff_t>
        inline constexpr iterator_range<typename std::decay<IteratorT>::type>
        make_range_sized(IteratorT &&begin, DiffT range_size) {
            return iterator_range<typename std::decay<IteratorT>::type>(begin, begin + range_size);
        }

        template<typename Func, typename DiffT = std::ptrdiff_t>
        struct index_iterator {
            using difference_type = DiffT;
            using reference = decltype(std::declval<Func>()(DiffT()));
            using value_type = typename std::decay<reference>::type;
            using pointer = typename std::add_pointer<value_type>::type;
            using iterator_category = std::random_access_iterator_tag;

            index_iterator()
                    : dereferencer{[](difference_type) { return value_type(); }}, index{-1} {}

            index_iterator(Func &&f, DiffT i)
                    : dereferencer{std::forward<Func>(f)}, index{i} {}

            index_iterator &operator++() {
                ++index;
                return *this;
            }

            index_iterator &operator--() {
                --index;
                return *this;
            }

            index_iterator &operator+=(difference_type n) {
                index += n;
                return *this;
            }

            difference_type operator-(const index_iterator &b) {
                return index - b.index;
            }

            bool operator==(const index_iterator &b) { return index == b.index; }

            bool operator!=(const index_iterator &b) { return !(*this == b); }

            bool operator<(const index_iterator &b) { return index < b.index; }

            bool operator>(const index_iterator &b) { return index > b.index; }

            bool operator>=(const index_iterator &b) { return !(*this < b); }

            bool operator<=(const index_iterator &b) { return !(*this > b); }

            reference operator*() {
                return dereferencer.callable(index);
            }

            const reference operator*() const {
                return dereferencer.callable(index);
            }

            pointer operator->() {
                return &(*(*this));
            }

            const pointer operator->() const {
                return &(*(*this));
            }

            reference operator[](difference_type n) {
                return *((*this) + n);
            }

        private:
            callable_container<Func> dereferencer;
            difference_type index;
        };

        template<typename Func, typename DiffT = std::ptrdiff_t>
        index_iterator<Func, DiffT> operator++(index_iterator<Func, DiffT> &a, int) {
            auto temp(a);
            ++a;
            return temp;
        }

        template<typename Func, typename DiffT = std::ptrdiff_t>
        index_iterator<Func, DiffT> operator--(index_iterator<Func, DiffT> &a, int) {
            auto temp(a);
            --a;
            return temp;
        }

        template<typename Func, typename DiffT = std::ptrdiff_t>
        index_iterator<Func, DiffT>
        operator+(const index_iterator<Func, DiffT> &a, typename index_iterator<Func, DiffT>::difference_type n) {
            auto temp(a);
            return temp += n;
        }

        template<typename Func, typename DiffT = std::ptrdiff_t>
        index_iterator<Func, DiffT>
        operator+(typename index_iterator<Func, DiffT>::difference_type n, const index_iterator<Func, DiffT> &a) {
            auto temp(a);
            return temp += n;
        }

        template<typename Func, typename DiffT = std::ptrdiff_t>
        index_iterator<Func, DiffT> &
        operator-=(index_iterator<Func, DiffT> &a, typename index_iterator<Func, DiffT>::difference_type n) {
            return a += -n;
        }

        template<typename Func, typename DiffT = std::ptrdiff_t>
        index_iterator<Func, DiffT>
        operator-(const index_iterator<Func, DiffT> &a, typename index_iterator<Func, DiffT>::difference_type n) {
            auto temp(a);
            return temp -= n;
        }

        template<typename Func, typename DiffT = std::ptrdiff_t>
        inline auto make_index_iterator_range(Func f, DiffT n, DiffT i0 = 0) {
            using IteratorType = index_iterator<typename std::decay<Func>::type, typename std::decay<DiffT>::type>;
            return make_range_sized(IteratorType(std::forward<Func>(f), i0), n);
        }
    }
}
