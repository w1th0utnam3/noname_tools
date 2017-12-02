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

#pragma once

#include <type_traits>

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
	}
}
