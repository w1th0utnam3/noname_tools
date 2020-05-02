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

#include <type_traits>

#include "general_defs.h"
#include "typelist_tools.h"
#include "functional_tools.h"

#ifdef NONAME_CPP17
#include NONAME_OPTIONAL_INCLUDE
#endif

// TODO: By using helper types instead of lambdas, this could be made compatible with C++14
// TODO: Check if this can be made constexpr (not super useful though)

namespace noname {
    namespace tools {
#ifdef NONAME_CPP17
        namespace _detail {

            //! Wraps a NTTP `T v` in an `std::integral_constant<T, v>`
            template <auto Value>
            using integral_constant_t = std::integral_constant<decltype(Value), Value>;

            //! Wraps a pack of NTTPs `Ts... v` in a typelist of `std::integral_constant<Ts, v>...`
            template <auto... Values>
            using integral_constant_typelist_t = typelist<integral_constant_t<Values>...>;

            //! Calls the callable `f` with a pack of NTTPs `Ts... v` wrapped into `std::integral_constant<Ts, v>...`
            template<typename F, auto... Values>
            constexpr decltype(auto) apply_integral_constant_typelist(F&& f, integral_constant_typelist_t<Values...>) {
                return f(integral_constant_t<Values>{}...);
            }

            template <typename T, T... Values>
            constexpr auto make_integral_constant_typelist() {
                return integral_constant_typelist_t<Values...>{};
            }

            template <typename T, T N>
            constexpr auto make_integer_sequence_typelist() {
                auto construct_sequence = [](auto... Is) {
                    return integral_constant_typelist_t<static_cast<T>(Is)...>{};
                };

                return apply_integer_sequence<T, N>(construct_sequence);
            }

            template <typename T, std::size_t N, const std::array<T, N>& Values>
            constexpr auto make_integral_constant_typelist_from_array() {
                auto from_array = [](auto... Is) {
                    return integral_constant_typelist_t<Values[Is]...>{};
                };

                return apply_index_sequence<N>(from_array);
            }

            template<typename F, typename T, auto... Values>
            bool rtct_map(F&& f, T value, integral_constant_typelist_t<Values...> allowedValues) {
                // Invoke `f` if the given value matches the run-time value
                const auto checkOverload = [value, &f](auto possibleValue) -> bool {
                    if (possibleValue() == value) {
                        (void) f(possibleValue);
                        return true;
                    }
                    return false;
                };

                return apply_integral_constant_typelist([=](const auto& ... is) {
                    // Fold over all possible allowed values
                    return (checkOverload(is) || ...);
                }, allowedValues);
            }

            template <typename F, auto... Values>
            using common_return_t = std::common_type_t<std::invoke_result_t<F, integral_constant_t<Values>>...>;

            template <typename F, auto... Values>
            using optional_return_t = NONAME_OPTIONAL_T<common_return_t<F, Values...>>;

            template<typename F, typename T, auto... Values>
            auto rtct_map_transform(F&& f, T value, integral_constant_typelist_t<Values...> allowedValues) {
                const auto recursivelyCheckOverloads = [value, &f](auto&& g, auto currentValue, auto... possibleValues) -> optional_return_t<F, Values...> {
                    if (currentValue() == value) {
                        return NONAME_OPTIONAL_T { f(currentValue) };
                    } else {
                        if constexpr (sizeof...(possibleValues) == 0) {
                            // Suppress `g is unused` warnings
                            (void) g;
                            // Return empty optional
                            return {};
                        } else {
                            return g(g, possibleValues...);
                        }
                    }
                };

                return apply_integral_constant_typelist([=](const auto& ... is) {
                    // Fold over all possible allowed values
                    return recursivelyCheckOverloads(recursivelyCheckOverloads, is...);
                }, allowedValues);
            }
        }
#endif
    }
}
