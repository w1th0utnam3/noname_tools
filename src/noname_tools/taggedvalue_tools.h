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

#include <tuple>
#include <type_traits>

#include "typetraits_tools.h"
#include "utility_tools.h"

namespace noname
{
	namespace tools
	{
		//! Simple aggregate of one T value, with a tag_type alias for the supplied TagT
		template <typename TagT, typename T>
		struct tagged_value
		{
			//! Alias for the tag type
			using tag_type = TagT;
			//! Alias fo the value type
			using value_type = T;

			//! Contained value of type T
			T value;

			//! Default constructor
			constexpr tagged_value() = default;
			//! Copy constructor from tagged_value with convertible value
			template<typename U>
			constexpr tagged_value(const tagged_value<TagT, U>& u) : value(u.value) {}
			//! Move constructor from tagged_value with convertible value
			template<typename U>
			constexpr tagged_value(const tagged_value<TagT, U>&& u) : value(std::move(u)) {}
			//! Copy constructor from a T value
			constexpr tagged_value(const T& v) : value(v) {}
			//! Move constructor from a T value
			constexpr tagged_value(T&& v) noexcept(std::is_nothrow_move_constructible<T>::value) : value(std::move(v)) {}

			//! Implicit conversion to T
			constexpr operator const T&() const { return value; }
			//! Implicit conversion to T
			//constexpr operator T() { return value; }
			//! Implicit conversion to T reference
			constexpr operator T&() { return value; }

			//! Copy assignment from T
			constexpr tagged_value& operator=(const T& v) {
				value = v;
				return *this;
			}

			//! Move assignment from T
			constexpr tagged_value& operator=(T&& v) noexcept(std::is_nothrow_move_assignable<T>::value) {
				value = std::move(v);
				return *this;
			}

			//! Returns a pointer to the contained value
			constexpr const T* operator->() const { return &value; }
			//! Returns a pointer to the contained value
			constexpr T* operator->() { return &value; }

			//! Returns a reference to the contained value
			constexpr const T& operator*() const& { return value; }
			//! Returns a reference to the contained value
			constexpr T& operator*() & { return value; }
			//! Returns a reference to the contained value
			constexpr const T&& operator*() const&& { return value; }
			//! Returns a reference to the contained value
			constexpr T&& operator*() && { return value; }

			//! Swaps the contents with those of other.
			void swap(tagged_value& other) noexcept(std::is_nothrow_move_constructible<T>::value &&
													noexcept(std::swap(std::declval<T&>(), std::declval<T&>())))
			{
				using std::swap;
				swap(**this, *other);
			}
		};

		template <typename TagT, typename T, typename U>
		constexpr bool operator==(const tagged_value<TagT, T>& tval, const U& value) { return *tval == value; }
		template <typename TagT, typename T, typename U>
		constexpr bool operator==(const U& value, const tagged_value<TagT, T>& tval) { return value == *tval; }
		template <typename TagT, typename T, typename U>
		constexpr bool operator!=(const tagged_value<TagT, T>& tval, const U& value) { return *tval != value; }
		template <typename TagT, typename T, typename U>
		constexpr bool operator!=(const U& value, const tagged_value<TagT, T>& tval) { return value != *tval; }
		template <typename TagT, typename T, typename U>
		constexpr bool operator<(const tagged_value<TagT, T>& tval, const U& value) { return *tval < value; }
		template <typename TagT, typename T, typename U>
		constexpr bool operator<(const U& value, const tagged_value<TagT, T>& tval) { return value < *tval; }
		template <typename TagT, typename T, typename U>
		constexpr bool operator<=(const tagged_value<TagT, T>& tval, const U& value) { return *tval <= value; }
		template <typename TagT, typename T, typename U>
		constexpr bool operator<=(const U& value, const tagged_value<TagT, T>& tval) { return value <= *tval; }
		template <typename TagT, typename T, typename U>
		constexpr bool operator>(const tagged_value<TagT, T>& tval, const U& value) { return *tval > value; }
		template <typename TagT, typename T, typename U>
		constexpr bool operator>(const U& value, const tagged_value<TagT, T>& tval) { return value > *tval; }
		template <typename TagT, typename T, typename U>
		constexpr bool operator>=(const tagged_value<TagT, T>& tval, const U& value) { return *tval >= value; }
		template <typename TagT, typename T, typename U>
		constexpr bool operator>=(const U& value, const tagged_value<TagT, T>& tval) { return value >= *tval; }

		//! Creates a tagged_value from the supplied value
		template <typename TagT, typename T>
		constexpr tagged_value<TagT, typename std::decay<T>::type> make_tagged_value(T&& value)
		{
			return tagged_value<TagT, typename std::decay<T>::type>(std::forward<T>(value));
		}

		namespace _detail
		{
			template <typename TagT, typename T>
			struct _initializer
			{
				TagT tag;
				T value;
			};
		}

		//! An array/tuple like type that maps types Ts to a value of type T each
		template <typename ValueT, typename... TagTs>
		class tagged_array
		{
		public:
			static_assert(unique_elements_v<TagTs...>, "Tag types have to be unique");

			//! Construct a default initialized tagged array
			constexpr tagged_array() = default;

			//! Construct a tagged array from values
			/* 
			 * Only takes part in overload resolution if the value type is constructible from all arguments 
			 * and the number of arguments matches the number of tags
			 */
			template <typename... ValueTs, 
					  typename = typename std::enable_if<conjunction<std::is_constructible<ValueT, ValueTs>...>::value 
														 && sizeof...(ValueTs) == sizeof...(TagTs)>::type>
			constexpr tagged_array(ValueTs... args)
				: values(args...)
			{
			}

			//! Construct a tagged array from corresponding tagged values
			constexpr explicit tagged_array(tagged_value<TagTs, ValueT>... args)
				: values(args.value...)
			{}

			//! Construct a tagged array from a list of initializer objects
			/*
			 * Can be used with initializer lists containing a tag instance and a value, e.g.:
			 *     tagged_array({tag1(), value1}, {tag2(), value2}, {tag3(), value3})
			 */
			constexpr tagged_array(_detail::_initializer<TagTs, ValueT>... args)
				: values(args.value...)
			{}

			//! Returns the number of values in the array
			constexpr std::size_t size() const { return sizeof...(TagTs); }

		private:
			std::tuple<tagged_value<TagTs, ValueT>...> values;

			template <std::size_t I, typename _ValueT, typename... _TagTs>
			friend constexpr _ValueT& get(tagged_array<_ValueT, _TagTs...>& t) noexcept;

			template <std::size_t I, typename _ValueT, typename... _TagTs>
			friend constexpr _ValueT&& get(tagged_array<_ValueT, _TagTs...>&& t) noexcept;

			template <std::size_t I, typename _ValueT, typename... _TagTs>
			friend constexpr const _ValueT& get(const tagged_array<_ValueT, _TagTs...>& t) noexcept;

			template <typename TagT, typename _ValueT, typename... _TagTs>
			friend constexpr _ValueT& get(tagged_array<_ValueT, _TagTs...>& t) noexcept;

			template <typename TagT, typename _ValueT, typename... _TagTs>
			friend constexpr _ValueT&& get(tagged_array<_ValueT, _TagTs...>&& t) noexcept;

			template <typename TagT, typename _ValueT, typename... _TagTs>
			friend constexpr const _ValueT& get(const tagged_array<_ValueT, _TagTs...>& t) noexcept;
		};

		template <std::size_t I, typename ValueT, typename... TagTs>
		constexpr ValueT& get(tagged_array<ValueT, TagTs...>& t) noexcept
		{
			return std::get<I>(t.values);
		}

		template <std::size_t I, typename ValueT, typename... TagTs>
		constexpr ValueT&& get(tagged_array<ValueT, TagTs...>&& t) noexcept
		{
			return std::move(std::get<I>(std::move(t.values)).value);
		}
		
		template <std::size_t I, typename ValueT, typename... TagTs>
		constexpr const ValueT& get(const tagged_array<ValueT, TagTs...>& t) noexcept
		{
			return std::get<I>(t.values);
		}

		template <typename TagT, typename ValueT, typename... TagTs>
		constexpr ValueT& get(tagged_array<ValueT, TagTs...>& t) noexcept
		{
			return std::get<tagged_value<TagT, ValueT>>(t.values);
		}

		template <typename TagT, typename ValueT, typename... TagTs>
		constexpr ValueT&& get(tagged_array<ValueT, TagTs...>&& t) noexcept
		{
			return std::move(std::get<tagged_value<TagT, ValueT>>(std::move(t.values)).value);
		}

		template <typename TagT, typename ValueT, typename... TagTs>
		constexpr const ValueT& get(const tagged_array<ValueT, TagTs...>& t) noexcept
		{
			return std::get<tagged_value<TagT, ValueT>>(t.values);
		}
	}
}
