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

#include <utility>

#include "tuple_tools.h"

namespace noname {
    namespace tools {
        namespace _detail {
            template<typename F, std::size_t ...Is>
            constexpr decltype(auto) apply_index_sequence(F&& fn, std::index_sequence<Is...>) {
                return fn(std::integral_constant<std::size_t, Is>{}...);
            }
        }

        //! Calls the callable F with the N arguments given by `std::integral_constant<std::size_t, 0>` to `std::integral_constant<std::size_t, N-1>`.
        template<std::size_t N, typename F>
        constexpr decltype(auto) apply_index_sequence(F&& fn) {
            return _detail::apply_index_sequence(std::forward<F>(fn), std::make_index_sequence<N>{});
        }

        //! Container to allow copy assignment of callable objects
        template<typename Func>
        struct callable_container {
            Func callable;

            // Inspired by: https://stackoverflow.com/questions/12545072/what-does-it-mean-for-an-allocator-to-be-stateless
            callable_container& operator=(const callable_container& other) {
                this->callable.~Func();
                new(&this->callable) Func{other.callable};
                return *this;
            }
        };
    }
}
