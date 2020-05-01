#pragma once

//	MIT License
//
//	Copyright (c) 2017 Fabian Löschner
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

#include "typetraits_tools.h"
#include "utility_tools.h"

#if defined(_MSC_VER)
#define NONAME_EBO __declspec(empty_bases)
#else
#define NONAME_EBO
#endif

// TODO: Add some operations for type lists: concat, zip?

namespace noname {
    namespace tools {
        namespace _detail {
            //! Dumb typelist without any special functionality
            template<typename... Ts>
            struct _plain_typelist {
            };

            //! Base type for the typelist, has every type_identity<T> with T from Ts as a base
            template<typename... Ts>
            struct NONAME_EBO _base_typelist : type_identity<Ts> ... {
            };

            //------

            //! Provides a type alias with a _base_typlist corresponding to the supplied _plain_typelist
            template<typename T>
            struct plain_to_base_typelist;

            template<typename... Ts>
            struct plain_to_base_typelist<_plain_typelist<Ts...>> {
                using type = _base_typelist<Ts...>;
            };

            //------

            // Code to remove duplicates inspired from:
            // https://stackoverflow.com/questions/13827319/eliminate-duplicate-entries-from-c11-variadic-template-arguments

            //! Helper type to check whether T is part of the typelist, has a type alias for std::(true/false)_type
            template<typename T, typename Typelist>
            struct is_in;

            template<typename T, typename Typelist>
            struct is_in {
                // Assume T is not in the list unless proven otherwise
                using type = std::false_type;
            };

            template<typename T, typename... Ts>
            struct is_in<T, _plain_typelist<T, Ts...>> {
                // If it matches the first type, it is definitely in the list
                using type = std::true_type;
            };

            template<typename T, typename Ts_Head, typename... Ts_Tail>
            struct is_in<T, _plain_typelist<Ts_Head, Ts_Tail...>> {
                // If it is not the first element, check the remaining list
                using type = typename is_in<T, _plain_typelist<Ts_Tail...>>::type;
            };

            //------

            //! Append a type to a _plain_typelist unless it is already part of it
            template<typename T, typename Typelist, typename T_is_duplicate = typename is_in<T, Typelist>::type>
            struct add_unique;

            template<typename T, typename... Ts>
            struct add_unique<T, _plain_typelist<Ts...>, std::true_type> {
                // If T is in the list, return the list unmodified
                using type = _plain_typelist<Ts...>;
            };

            template<typename T, typename... Ts>
            struct add_unique<T, _plain_typelist<Ts...>, std::false_type> {
                // If T is not in the list, append it
                using type = _plain_typelist<T, Ts...>;
            };

            //! Provides a type alias for a _plain_typelists which contains only unique T from Ts
            template<typename... Ts>
            struct create_unique_typelist;

            template<>
            struct create_unique_typelist<> {
                using type = _plain_typelist<>;
            };

            template<typename T, typename... Ts>
            struct create_unique_typelist<T, Ts...> {
                using type = typename add_unique<T, typename create_unique_typelist<Ts...>::type>::type;
            };

            //------

            //! Provides a type alias for a _base_typelist which contains only unique T from Ts
            template<typename... Ts>
            struct unique_base_typelist {
                using type = typename plain_to_base_typelist<typename create_unique_typelist<Ts...>::type>::type;
            };
        }

        //! A basic typelist construct
        template<typename... Ts>
        struct NONAME_EBO typelist : _detail::unique_base_typelist<Ts...>::type {
        };

        template<typename T>
        struct typelist_size;

        template<typename... Ts>
        struct typelist_size<typelist<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)> {
        };

        template<typename T>
        constexpr std::size_t typelist_size_v = typelist_size<T>::value;

        template<typename... Ts>
        constexpr std::size_t typelist_size_v<typelist<Ts...>> = sizeof...(Ts);

        template<std::size_t I, typename T>
        struct nth_typelist_element;

        template<std::size_t I, typename... Ts>
        struct nth_typelist_element<I, typelist<Ts...>> {
            using type = nth_element_t<I, Ts...>;
        };

        template<std::size_t I, typename T>
        using nth_typelist_element_t = typename nth_typelist_element<I, T>::type;

        namespace _detail {
            template<typename Typelist, typename F, std::size_t... Indices>
            F typelist_for_each(Typelist &&tl, F f, std::index_sequence<Indices...>) {
                using swallow = int[];
                (void) swallow{
                        1,
                        (f(static_cast<type_identity<nth_typelist_element_t<Indices, typename std::decay<Typelist>::type>>>(tl)), void(), int{})...
                };
                return f;
            }
        }

        //! Calls a function for each element of a typelist in order and returns the function, the function is called with a type_identity<T> for each T from the list
        template<typename Typelist, typename F>
        F typelist_for_each(F f) {
            static constexpr Typelist tl;
            static constexpr std::size_t N(typelist_size_v<std::remove_reference_t<Typelist>>);
            return _detail::typelist_for_each(
                    tl,
                    std::forward<F>(f),
                    std::make_index_sequence<N>{}
            );
        }
    }
}

#undef NONAME_EBO
