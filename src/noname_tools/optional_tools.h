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

#pragma once

#include <stdexcept>
#include <type_traits>
#include <utility>
#include <new>

// TODO: Implement copy-swap idiom?
// TODO: Add noexcept according to reference
// TODO: Comparison operators? :/
// TODO: Empty base optimization?
// TODO: As soon as MSVC has better constexpr support, all value() methods can be marked as constexpr

namespace noname
{
	namespace tools
	{
		//! nullopt_t is an empty class type used to indicate optional type with uninitialized state.
		struct nullopt_t {
			constexpr nullopt_t(decltype(nullptr)) {}
		};

		//! nullopt is a constant of type nullopt_t that is used to indicate optional type with uninitialized state.
		constexpr nullopt_t nullopt{ nullptr };

		//! Defines a type of object to be thrown by optional::value when accessing an optional object that does not contain a value.
		class bad_optional_access : public std::logic_error 
		{
		public:
			bad_optional_access() : std::logic_error("Optional does not contain a value.") {}
		};

		//! The class template optional manages an optional contained value, i.e. a value that may or may not be present.
		template <class T>
		class optional {
		public:
			//! Type of the contained value
			typedef T value_type;

			//! Constructs an optional object that does not contain a value.
			constexpr optional() : _hasValue(false), _memory() {}

			//! Constructs an optional object that does not contain a value.
			constexpr optional(nullopt_t) : _hasValue(false), _memory() {}

			//! Copy constructor: If other contains a value, initializes the contained value as if direct-initializing an object of type T with the expression *other. If other does not contain a value, constructs an object that does not contain a value.
			optional(const optional& other)
				: _hasValue(other._hasValue)
			{
				static_assert(std::is_copy_constructible<T>::value, "The value type must meet the requirements of CopyConstructible in order to use construction by copy.");
				if (_hasValue) new(_memory) T(*other);
			}

			//! Move constructor: If other contains a value, initializes the contained value as if direct-initializing an object of type T with the expression std::move(*other) and does not make other empty: a moved-from optional still contains a value, but the value itself is moved from. If other does not contain a value, constructs an object that does not contain a value.
			optional(optional&& other)
				: _hasValue(other._hasValue)
			{
				static_assert(std::is_move_constructible<T>::value, "The value type must meet the requirements of MoveConstructible in order to use construction by move.");
				if (_hasValue) new(_memory) T(std::move(*other));
			}

			//! Constructs an optional object that contains a value, initialized as if direct-initializing (but not direct-list-initializing) an object of type T with the expression value. This constructor is constexpr if the constructor of T selected by direct-initialization is constexpr.
			constexpr optional(const T& value)
				: _hasValue(true), _memory()
			{
				static_assert(std::is_copy_constructible<T>::value, "The value type must meet the requirements of CopyConstructible in order to use construction by copy.");
				new(_memory) T(value);
			}

			//!  Constructs an optional object that contains a value, initialized as if direct-initializing (but not direct-list-initializing) an object of type T with the expression std::move(value). This constructor is constexpr if the constructor of T selected by direct-initialization is constexpr.
			constexpr optional(T&& value)
				: _hasValue(true), _memory()
			{
				static_assert(std::is_move_constructible<T>::value, "The value type must meet the requirements of MoveConstructible in order to use construction by move.");
				new(_memory) T(std::move(value));
			}

			//! Constructs an optional object that contains a value, initialized as if direct-initializing (but not direct-list-initializing) an object of type T from the arguments std::forward<Args>(args)...
			template< class... Args >
			constexpr explicit optional(tools::in_place_t, Args&&... args)
				: _hasValue(true), _memory()
			{
				static_assert(std::is_constructible<T, Args&&...>::value, "The value type must be constructible from the supplied Args.");
				new(_memory) T(std::forward<Args>(args)...);
			}

			//! Constructs an optional object that contains a value, initialized as if direct-initializing (but not direct-list-initializing) an object of type T from the arguments ilist, std::forward<Args>(args)....
			template< class U, class... Args >
			constexpr explicit optional(tools::in_place_t, std::initializer_list<U> ilist, Args&&... args)
				: _hasValue(true), _memory()
			{
				static_assert(std::is_constructible<T, std::initializer_list<U>&, Args&&...>::value, "The value type must be constructible from std::initializer_list and supplied Args.");
				new(_memory) T(ilist, std::forward<Args>(args)...);
			}

			//! Calls the constructor of the contained value if it is not trivially destructible.
			~optional()
			{
				if (_hasValue) (**this).~T();
			}

			//! If *this contains a value before the call, the contained value is destroyed by calling its destructor.
			optional& operator=(nullopt_t)
			{
				if (_hasValue) {
					(**this).~T();
					_hasValue = false;
				}
				return *this;
			}

			//! Replaces contents of *this with the contents of other.
			optional& operator=(const optional& other)
			{
				static_assert(std::is_copy_constructible<T>::value && std::is_copy_assignable<T>::value, "The value type must meet the requirements of CopyConstructible and CopyAssignable in order to use construction by move.");

				if (_hasValue && other._hasValue) {
					(**this) = *other;
				}
				else if (_hasValue) {
					(**this).~T();
					_hasValue = false;
				}
				else if (other._hasValue) {
					new(_memory) T(*other);
					_hasValue = true;
				}
				return *this;
			}

			//! Replaces contents of *this with the contents of other.
			optional& operator=(optional&& other)
			{
				static_assert(std::is_move_constructible<T>::value && std::is_move_assignable<T>::value, "The value type must meet the requirements of MoveConstructible and MoveAssignable in order to use construction by move.");

				if (_hasValue && other._hasValue) {
					(**this) = std::move(*other);
				}
				else if (_hasValue) {
					(**this).~T();
					_hasValue = false;
				}
				else if (other._hasValue) {
					new(_memory) T(std::move(*other));
					_hasValue = true;
				}
				return *this;
			}

			//! Replaces contents of *this with value.
			template< class U >
			optional& operator=(U&& value)
			{
				static_assert(std::is_same<std::decay_t<U>, T>::value, "The supplied value must be of the same type as the optional value.");

				if (_hasValue) {
					(**this) = std::move(value);
				}
				else {
					new(_memory) T(std::move(value));
					_hasValue = true;
				}
				return *this;
			}

			//! Accesses the contained value. Returns const-pointer.
			constexpr const T* operator->() const
			{
				return static_cast<const T*>(static_cast<const void*>(&_memory));
			}

			//! Accesses the contained value. Returns pointer.
			T* operator->()
			{
				return static_cast<T*>(static_cast<void*>(&_memory));
			}

			//! Accesses the contained value. Returns const-ref.
			constexpr const T& operator*() const&
			{
				return *static_cast<const T*>(static_cast<const void*>(&_memory));
			}

			//! Accesses the contained value. Returns ref.
			T& operator*() &
			{
				return *static_cast<T*>(static_cast<void*>(&_memory));
			}

			//! Accesses the contained value.
			T&& operator*() &&
			{
				return std::move(*static_cast<T*>(static_cast<void*>(&_memory)));
			}

			//! Checks whether *this contains a value.
			constexpr explicit operator bool() const { return _hasValue; }

			//! Checks whether *this contains a value.
			constexpr bool has_value() const { return _hasValue; }

			//! If *this contains a value, returns a reference to the contained value.
			T& value() &
			{
				if (_hasValue) return **this; throw bad_optional_access();
			}

			//! If *this contains a value, returns a const-reference to the contained value.
			constexpr const T & value() const &
			{
				if (_hasValue) return **this; throw bad_optional_access();
			}

			//! If *this contains a value, returns a reference to the contained value.
			T&& value() &&
			{
				if (_hasValue) return std::move(**this); throw bad_optional_access();
			}

			//! Returns the contained value if *this has a value, otherwise returns default_value.
			template<class U>
			constexpr T value_or(U&& default_value) const&
			{
				static_assert(std::is_copy_constructible<T>::value, "The value type must meet the requirements of CopyConstructible in order to use this value_or overload.");
				static_assert(std::is_convertible<U, T>::value, "The supplied default value must be convertible to the value type of the optional.");

				return bool(*this) ? **this : static_cast<T>(std::forward<U>(default_value));
			}

			//! Returns the contained value if *this has a value, otherwise returns default_value.
			template<class U>
			constexpr T value_or(U&& default_value) &&
			{
				static_assert(std::is_move_constructible<T>::value, "The value type must meet the requirements of MoveConstructible in order to use this value_or overload.");
				static_assert(std::is_convertible<U, T>::value, "The supplied default value must be convertible to the value type of the optional.");

				return bool(*this) ? std::move(**this) : static_cast<T>(std::forward<U>(default_value));
			}

			//! Swaps the contents with those of other.
			void swap(optional& other)
			{
				static_assert(tools::is_swappable<T>::value, "The value type must meet the requirements of Swappable.");

				if (_hasValue && other._hasValue) {
					using std::swap;
					swap(**this, *other);
				}
				else if (_hasValue) {
					new(other._memory) T(std::move(**this));
					(**this).~T();
					std::swap(this->_hasValue, other._hasValue);
				}
				else if (other._hasValue) {
					new(_memory) T(std::move(*other));
					other->T::~T();
					std::swap(this->_hasValue, other._hasValue);
				}
			}

			//! If *this contains a value, destroy that value as if by value().T::~T(). Otherwise, there are no effects. *this does not contain a value after this call.
			void reset()
			{
				if (_hasValue) (**this).~T();
				_hasValue = false;
			}

			//! Constructs the contained value in-place. If *this already contains a value before the call, the contained value is destroyed by calling its destructor.
			template< class... Args >
			void emplace(Args&&... args)
			{
				static_assert(std::is_constructible<T, Args&&...>::value, "The value type must be constructible from the supplied Args.");
				if (_hasValue) (**this).~T();
				new(_memory) T(std::forward<Args>(args)...);
				_hasValue = true;
			}

			//! Constructs the contained value in-place. If *this already contains a value before the call, the contained value is destroyed by calling its destructor.
			template< class U, class... Args >
			void emplace(std::initializer_list<U> ilist, Args&&... args)
			{
				static_assert(std::is_constructible<T, std::initializer_list<U>&, Args&&...>::value, "The value type must be constructible from std::initializer_list and supplied Args.");
				if (_hasValue) (**this).~T();
				new(_memory) T(ilist, std::forward<Args>(args)...);
				_hasValue = true;
			}

		private:
			bool _hasValue;
			char _memory[sizeof(T)];
		};

		//! Creates an optional object from value.
		template< class T >
		constexpr optional<std::decay_t<T>> make_optional(T&& value)
		{
			return optional<std::decay_t<T>>(std::forward<T>(value));
		}

		//! Creates an optional object constructed in-place from args....
		template< class T, class... Args >
		constexpr optional<T> make_optional(Args&&... args)
		{
			return optional<T>(in_place, std::forward<Args>(args)...);
		}
		
		//! Creates an optional object constructed in-place from il and args....
		template< class T, class U, class... Args >
		constexpr optional<T> make_optional(std::initializer_list<U> il, Args&&... args)
		{
			return optional<T>(in_place, il, std::forward<Args>(args)...);
		}
	}
}
