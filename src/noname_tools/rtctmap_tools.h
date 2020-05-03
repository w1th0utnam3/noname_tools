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
#include <utility>

#include "general_defs.h"
#include "functional_tools.h"
#include "typelist_tools.h"
#include "typetraits_tools.h"

#ifdef NONAME_CPP17
#include NONAME_OPTIONAL_INCLUDE
#endif

// TODO: Add a range map that can also contain negative values
// TODO: For the sequence map: check inside of the mapper if the runtime value is even in range
// TODO: By using helper types instead of lambdas, this could be made compatible with C++14
// TODO: Check if this can be made constexpr (not super useful though)

namespace noname {
    namespace tools {
        namespace rtct_map {
#ifdef NONAME_CPP17
            namespace _detail {

                /// Wraps a NTTP `auto V` in a `std::integral_constant<decltype(V), V>`
                template<auto Value>
                using integral_constant_t = std::integral_constant<decltype(Value), Value>;

                /// Wraps a pack of NTTPs `auto... Vs` in a typelist of `std::integral_constant<decltype(Vs), Vs>...`
                template<auto... Values>
                using integral_constant_typelist_t = typelist<integral_constant_t<Values>...>;

                /// Calls the callable `f` with a pack of NTTPs `auto... Vs` wrapped into `std::integral_constant<decltype(Vs), Vs>...`
                template<typename F, auto... Values>
                constexpr decltype(auto)
                apply_integral_constant_typelist(F&& f, integral_constant_typelist_t<Values...>) {
                    return f(integral_constant_t<Values>{}...);
                }

                /// Helper function to create an `integral_constant_typelist_t` instance containing all values from the given NTTP pack `T... Vs` wrapped into `std::integral_constant`s`
                template<typename T, T... Values>
                constexpr auto make_integral_constant_typelist() {
                    return integral_constant_typelist_t<Values...>{};
                }

                /// Helper function to create an `integral_constant_typelist_t` instance containing the elements of a `std::integer_sequence<T, N>`
                template<typename T, T N>
                constexpr auto make_integer_sequence_typelist() {
                    auto construct_sequence = [](auto... Is) {
                        return integral_constant_typelist_t<static_cast<T>(Is)...>{};
                    };

                    return apply_integer_sequence<T, N>(construct_sequence);
                }

                /// Helper function to create an `integral_constant_typelist_t` containing all values from the given `std::array` wrapped into `std::integral_constant`s
                template<typename T, std::size_t N, const std::array<T, N>& Values>
                constexpr auto make_integral_constant_typelist_from_array() {
                    auto from_array = [](auto... Is) {
                        return integral_constant_typelist_t<Values[Is]...>{};
                    };

                    return apply_index_sequence<N>(from_array);
                }

                /// Calls a callable with a run-time argument converted to a compile-time parameter. Returns if the callable was actually called.
                ///
                /// Calls a callable `f` with the run-time parameter `value` converted into the corresponding `std::integral_constant<T, value>` if it is present in the compile-time `Values` NTTPs pack.
                /// Returns whether it was actually called (i.e. if `value` was present in `Values...` pack). The return value of `f` is discarded. Note that `f` must be invokable with all possible
                /// types `std::integral_constant<decltype(V), V>` for every `V` in `Values`.
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

                /// The `std::common_type_t` of all invocations of the given callable `F` with each one of the NTTP values wrapped into a `std::integral_constant`
                template<typename F, auto... Values>
                using common_return_t = std::common_type_t<std::invoke_result_t<F, integral_constant_t<Values>>...>;

                /// The type `common_return_t` wrapped into an `optional`
                template<typename F, auto... Values>
                using optional_return_t = NONAME_OPTIONAL_T<common_return_t<F, Values...>>;

                /// Calls a callable with a run-time argument converted to a compile-time parameter. Returns the callable's return value wrapped into an optional if it was actually called.
                ///
                /// Calls a callable `f` with the run-time parameter `value` converted into the corresponding `std::integral_constant<T, value>` if it is present in the compile-time `Values` NTTPs pack.
                /// Returns the return value of `f` wrapped into an `optional`. The `optional` is empty if `f` was not invoked (i.e. if `value` was NOT present in the `Values` pack).
                /// Note that `f` must be invokable with all possible types `std::integral_constant<decltype(V), V>` for every `V` in `Values`. In addition, the return values of all these invocations
                /// must have a common type to which they can be implicitly converted to.
                template<typename F, typename T, auto... Values>
                auto rtct_map_transform(F&& f, T value, integral_constant_typelist_t<Values...> allowedValues) {
                    const auto recursivelyCheckOverloads = [value, &f](auto&& g, auto currentValue,
                                                                       auto... possibleValues) -> optional_return_t<F, Values...> {
                        if (currentValue() == value) {
                            return NONAME_OPTIONAL_T{f(currentValue)};
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

                template<typename TypelistT>
                struct rtct_mapper {
                    template<typename F, typename T>
                    bool map(F&& f, T&& value) const {
                        return rtct_map(std::forward<F>(f), std::forward<T>(value), TypelistT{});
                    }

                    template<typename F, typename T>
                    decltype(auto) map_transform(F&& f, T&& value) const {
                        return rtct_map_transform(std::forward<F>(f), std::forward<T>(value), TypelistT{});
                    }
                };
            }

            template<typename T, T... Values>
            auto make_map() {
                using ValueT = remove_cvref_t<T>;
                return _detail::rtct_mapper<decltype(
                        _detail::make_integral_constant_typelist<ValueT,
                        static_cast<ValueT>(Values)...>())>{};
            }

            template<typename T, T N>
            auto make_sequence_map() {
                return _detail::rtct_mapper<decltype(_detail::make_integer_sequence_typelist<remove_cvref_t<T>, N>())>{};
            }

            template<const auto& ValueArray>
            auto make_array_map() {
                using ArrayT = remove_cvref_t<decltype(ValueArray)>;
                using ValueT = typename ArrayT::value_type;
                static constexpr const std::size_t N = ValueArray.size();
                return _detail::rtct_mapper<decltype(_detail::make_integral_constant_typelist_from_array<ValueT, N, ValueArray>())>{};
            }
#endif
        }
    }
}
