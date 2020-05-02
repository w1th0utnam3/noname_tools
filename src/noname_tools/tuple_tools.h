#pragma once

//	MIT License
//
//	Copyright (c) 2020 Fabian Löschner
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

#include <tuple>
#include <utility>

#include "general_defs.h"

// TODO: Make tuple_for_each constexpr where supported. Maybe just for C++17?

namespace noname {
    namespace tools {
        namespace _detail {
            template<typename Tuple, typename F, std::size_t ...Indices>
            F tuple_for_each(Tuple &&tuple, F f, std::index_sequence<Indices...>) {
                using swallow = int[];
                (void) swallow{
                        1, (f(std::get<Indices>(std::forward<Tuple>(tuple))), void(), int{})...
                };
                return f;
            }
        }

        //! Calls a function for each element of a tuple in order and returns the function
        template<typename Tuple, typename F>
        F tuple_for_each(Tuple &&tuple, F f) {
            // Code from: https://codereview.stackexchange.com/questions/51407/stdtuple-foreach-implementation
            static constexpr const std::size_t N = std::tuple_size<std::remove_reference_t<Tuple>>::value;
            return _detail::tuple_for_each(
                    std::forward<Tuple>(tuple),
                    std::forward<F>(f),
                    std::make_index_sequence<N>{});
        }

        namespace _detail {
            template<typename T, T... I>
            constexpr auto integer_sequence_tuple_impl(std::integer_sequence<T, I...>) {
                return std::make_tuple(std::integral_constant<T, I>{}...);
            }
        }

        //! A tuple containing the N values of the integer type T from `std::integral_constant<T, 0>` to `std::integral_constant<T, N-1>`.
        template<typename T, T N>
        NONAME_INLINE_VARIABLE constexpr auto integer_sequence_tuple = _detail::integer_sequence_tuple_impl(std::make_integer_sequence<T, N>{});
    }
}
