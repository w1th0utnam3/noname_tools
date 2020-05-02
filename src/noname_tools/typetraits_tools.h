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
#include <algorithm>
#include <type_traits>

#include "general_defs.h"

namespace noname {
    namespace tools {
        //! Provides the member typedef type that names T (i.e., the identity transformation).
        template<class T>
        struct type_identity {
            using type = T;
        };

        //! A type which is always false_type and can be used in static_assert to always trigger an assertion.
        template<class T>
        struct dependent_false : std::false_type {
        };

        namespace _detail {
            //! Helper used for void_t and required by MSVC
            template<class...>
            struct _make_void {
                using type = void;
            };
        } // namespace _detail

        using namespace _detail;

        //! Utility metafunction that maps a sequence of any types to the type void.
        template<typename... T>
        using void_t = typename _detail::_make_void<T...>::type;

        //! Helper alias template for std::integral_constant for the common case where T is bool.
        template<bool B>
        using bool_constant = std::integral_constant<bool, B>;

        //! Forms the logical negation of the type trait B.
        template<class B>
        struct negation : bool_constant<!B::value> {
        };

        //! Forms the logical conjunction of the type traits B..., effectively performing a logical AND on the sequence of traits.
        template<class...>
        struct conjunction : std::true_type {
        };
        template<class B1>
        struct conjunction<B1> : B1 {
        };
        template<class B1, class... Bn>
        struct conjunction<B1, Bn...> : std::conditional_t<B1::value != false, conjunction<Bn...>, B1> {
        };

        //! Forms the logical disjunction of the type traits B..., effectively performing a logical OR on the sequence of traits.
        template<class...>
        struct disjunction : std::false_type {
        };
        template<class B1>
        struct disjunction<B1> : B1 {
        };
        template<class B1, class... Bn>
        struct disjunction<B1, Bn...> : std::conditional_t<B1::value != false, B1, disjunction<Bn...>> {
        };

        // The is_detected implementation is equivalent to the reference implementation from http://en.cppreference.com/w/cpp/experimental/is_detected

        namespace _detail {
            template<class Default, class AlwaysVoid, template<class...> class Op, class... Args>
            struct _detector {
                using value_t = std::false_type;
                using type = Default;
            };

            template<class Default, template<class...> class Op, class... Args>
            struct _detector<Default, void_t<Op<Args...>>, Op, Args...> {
                using value_t = std::true_type;
                using type = Op<Args...>;
            };
        } // namespace _detail

        //! Class type used by detected_t to indicate detection failure.
        struct nonesuch {
            nonesuch() = delete;

            ~nonesuch() = delete;

            nonesuch(nonesuch const &) = delete;

            void operator=(nonesuch const &) = delete;
        };

        //! Alias for std::true_type if the template-id Op<Args...> is valid; otherwise it is an alias for std::false_type.
        template<template<class...> class Op, class... Args>
        using is_detected = typename _detail::_detector<nonesuch, void, Op, Args...>::value_t;

        //! Alias for Op<Args...> if that template-id is valid; otherwise it is an alias for the class nonesuch.
        template<template<class...> class Op, class... Args>
        using detected_t = typename _detail::_detector<nonesuch, void, Op, Args...>::type;

        //! If the template-id Op<Args...> is valid, then value_t is an alias for std::true_type, and type is an alias for Op<Args...>; Otherwise, value_t is an alias for std::false_type and type is an alias for Default.
        template<class Default, template<class...> class Op, class... Args>
        using detected_or = _detail::_detector<Default, void, Op, Args...>;

        //! Checks whether detected_t<Op, Args...> is Expected.
        template<class Expected, template<class...> class Op, class... Args>
        using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

        //! Checks whether detected_t<Op, Args...> is convertible to To.
        template<class To, template<class...> class Op, class... Args>
        using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;

        namespace _detail {
            //! Unevaluated reference T&
            template<class T>
            using _reference_t = decltype(std::declval<T &>());

            //! Unevaluated swap(T&,U&)
            template<class T, class U>
            using _swap_with_overload = decltype(swap(std::declval<T &>(), std::declval<U &>()));

            //! Unevaluated std::swap(T&,U&)
            template<class T, class U>
            using _swap_with_std = decltype(std::swap(std::declval<T &>(), std::declval<U &>()));

            //! Checks if swap(std::declval<T>(), std::declval<U>()) is a valid expression
            template<class T, class U>
            using _is_swappable_with_overload = is_detected<_swap_with_overload, T, U>;

            //! Checks if std::swap(std::declval<T>(), std::declval<U>()) is a valid expression
            template<class T, class U>
            using _is_swappable_with_std = is_detected<_swap_with_std, T, U>;

            //! Checks if swap(std::declval<T>(), std::declval<U>()) or std::swap(std::declval<T>(), std::declval<U>()) is a valid expression
            template<class T, class U>
            using _is_swappable_with_unidirectional = disjunction<_is_swappable_with_overload<T, U>, _is_swappable_with_std<T, U>>;
        } // namespace _detail

        //! Checks if the supplied type is referenceable, i.e. whether T& is a well-formed type.
        template<class T>
        using is_referenceable = is_detected<_detail::_reference_t, T>;

        template<class T>
        NONAME_INLINE_VARIABLE constexpr bool is_referenceable_v = is_referenceable<T>::value;

        //! Checks if the expressions swap(std::declval<T>(), std::declval<U>()) and swap(std::declval<U>(), std::declval<T>()) are both well formed after "using std::swap".
        template<class T, class U>
        using is_swappable_with = conjunction<_detail::_is_swappable_with_unidirectional<T, U>, _detail::_is_swappable_with_unidirectional<U, T>>;

        //! Checks if a type is referenceable and whether std::is_swappable_with<T&, T&>::value is true.
        template<class T>
        using is_swappable = conjunction<is_referenceable<T>, is_swappable_with<T, T>>;

        template<class T, class U>
        NONAME_INLINE_VARIABLE constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;

        template<class T>
        NONAME_INLINE_VARIABLE constexpr bool is_swappable_v = is_swappable<T>::value;

        //! Combines std::remove_cv and std::remove_reference.
        template<class T>
        struct remove_cvref {
            typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type type;
        };

        //! Helper for remove_cvref, defined as remove_cvref::type.
        template<class T>
        using remove_cvref_t = typename remove_cvref<T>::type;
    }
}
