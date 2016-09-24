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

#include "typetraits_tools.h"
#include "utility_tools.h"

#include <stdexcept>
#include <type_traits>
#include <utility>
#include <new>

// TODO: Reimplement without placement new, so that constexpr actually works
// TODO: Trivial destructor
// TODO: Empty base optimization
// TODO: Check exception gurantees on copy construction/assignment
// TODO: Add noexcept according to reference
// TODO: Comparison operators? :/

#ifdef _MSC_VER
#define NONAME_CONSTEXPR_
#else
#define NONAME_CONSTEXPR_ constexpr
#endif

namespace noname
{
	namespace tools
	{
		# define REQUIRES(...) typename std::enable_if<__VA_ARGS__::value, bool>::type = false

		//! nullopt_t is an empty class type used to indicate optional type with uninitialized state.
		struct nullopt_t
		{
			struct init {};
			constexpr explicit nullopt_t(init) {}
		};

		//! nullopt is a constant of type nullopt_t that is used to indicate optional type with uninitialized state.
		constexpr nullopt_t nullopt{ nullopt_t::init() };

		//! Defines a type of object to be thrown by optional::value when accessing an optional object that does not contain a value.
		class bad_optional_access : public std::logic_error
		{
		public:
			explicit bad_optional_access(const std::string& what_arg) : std::logic_error{ what_arg } {}
			explicit bad_optional_access(const char* what_arg) : std::logic_error{ what_arg } {}
		};

		namespace _detail
		{
			constexpr struct _trivial_init_t {} _trivial_init{};

			template <class T>
			union _optional_storage
			{
				unsigned char _dummy;
				T _value;

				constexpr _optional_storage(_trivial_init_t) noexcept
					: _dummy() 
				{
				}

				template <class... Args>
				constexpr _optional_storage(Args&&... args)
					: _value(std::forward<Args>(args)...)
				{
				}

				~_optional_storage() 
				{
				}
			};

			template <class T>
			union _constexpr_optional_storage
			{
				unsigned char _dummy;
				T _value;

				constexpr _constexpr_optional_storage(_trivial_init_t) noexcept : _dummy()
				{
				};

				template <class... Args>
				constexpr _constexpr_optional_storage(Args&&... args)
					: _value(std::forward<Args>(args)...)
				{
				}

				~_constexpr_optional_storage() = default;
			};

			template <class T>
			struct _optional_base
			{
				bool _init;
				_optional_storage<T> _storage;

				constexpr _optional_base() noexcept 
					: _init(false)
					, _storage(_trivial_init_t)
				{
				}

				explicit constexpr _optional_base(const T& v)
					: _init(true)
					, _storage(v) 
				{
				}

				explicit constexpr _optional_base(T&& v)
					: _init(true)
					, _storage(std::move(v))
				{
				}

				template <class... Args> explicit _optional_base(in_place_t, Args&&... args)
					: _init(true)
					, _storage(std::forward<Args>(args)...)
				{
				}

				template <class U, class... Args, REQUIRES(std::is_constructible<T, std::initializer_list<U>>)>
				explicit _optional_base(in_place_t, std::initializer_list<U> il, Args&&... args)
					: _init(true)
					, _storage(il, std::forward<Args>(args)...)
				{
				}

				~_optional_base() 
				{ 
					if (_init) _storage._value.T::~T();
				}
			};

			template <class T>
			struct _constexpr_optional_base
			{
				bool _init;
				_constexpr_optional_storage<T> _storage;

				constexpr _constexpr_optional_base() noexcept
					: _init(false)
					, _storage(_trivial_init) 
				{
				};

				explicit constexpr _constexpr_optional_base(const T& v)
					: _init(true)
					, _storage(v) 
				{
				}

				explicit constexpr _constexpr_optional_base(T&& v)
					: _init(true)
					, _storage(std::move(v)) 
				{
				}

				template <class... Args> explicit constexpr _constexpr_optional_base(in_place_t, Args&&... args)
					: _init(true)
					, _storage(std::forward<Args>(args)...)
				{
				}

				template <class U, class... Args, REQUIRES(std::is_constructible<T, std::initializer_list<U>>)>
				NONAME_CONSTEXPR_ explicit _constexpr_optional_base(in_place_t, std::initializer_list<U> il, Args&&... args)
					: _init(true), _storage(il, std::forward<Args>(args)...)
				{
				}

				~_constexpr_optional_base() = default;
			};

			template <class T>
			using _optional_base_t = typename std::conditional<
				std::is_trivially_destructible<T>::value,
				_constexpr_optional_base<typename std::remove_const<T>::type>,
				_optional_base<typename std::remove_const<T>::type>
			>::type;

		} // namespace _detail

		//! The class template optional manages an optional contained value, i.e. a value that may or may not be present.
		template <class T>
		class optional : private _optional_base_t<T>
		{
			static_assert(!std::is_same<typename std::decay<T>::type, nullopt_t>::value, "bad T");
			static_assert(!std::is_same<typename std::decay<T>::type, in_place_t>::value, "bad T");

			std::aligned_storage_t<sizeof(T) + sizeof(bool)> _memory;

			//! Returns reference to the _hasValue flag
			NONAME_CONSTEXPR_ bool& _hasValueRef()
			{
				// Return reference of the _hasValue flag as last bool in the aligned storage
				return *(static_cast<bool*>(static_cast<void*>(((&_memory)+1)))-1);
			}

			//! Returns const reference to the _hasValue flag
			constexpr const bool& _hasValueRefConst() const
			{
				// Return reference of the _hasValue flag as last bool in the aligned storage
				return *(static_cast<const bool*>(static_cast<const void*>(((&_memory)+1)))-1);
			}

			//! Returns pointer to the address of the contained value
			NONAME_CONSTEXPR_ void* _valueMemoryPtr()
			{
				// Return address of the value memory as the address of the aligned storage
				return static_cast<void*>(&_memory);
			}

			//! Returns pointer to the address of the contained value
			constexpr const void* _valueMemoryPtrConst() const
			{
				// Return address of the value memory as the address of the aligned storage
				return static_cast<const void*>(&_memory);
			}

		public:
			//! Type of the contained value
			typedef T value_type;

			//! Constructs an optional object that does not contain a value.
			constexpr optional() : _memory() { _hasValueRef() = false; }

			//! Constructs an optional object that does not contain a value.
			constexpr optional(nullopt_t) : _memory() { _hasValueRef() = false; }

			//! Copy constructor: If other contains a value, initializes the contained value as if direct-initializing an object of type T with the expression *other. If other does not contain a value, constructs an object that does not contain a value.
			optional(const optional& other)
				: _memory()
			{
				static_assert(std::is_copy_constructible<T>::value, "The value type must meet the requirements of CopyConstructible in order to use construction by copy.");

				_hasValueRef() = other._hasValueRefConst();
				if (_hasValueRef()) new(_valueMemoryPtr()) T(*other);
			}

			//! Move constructor: If other contains a value, initializes the contained value as if direct-initializing an object of type T with the expression std::move(*other) and does not make other empty: a moved-from optional still contains a value, but the value itself is moved from. If other does not contain a value, constructs an object that does not contain a value.
			optional(optional&& other)
				: _memory()
			{
				static_assert(std::is_move_constructible<T>::value, "The value type must meet the requirements of MoveConstructible in order to use construction by move.");

				_hasValueRef() = other._hasValueRef();
				if (_hasValueRef()) new(_valueMemoryPtr()) T(std::move(*other));
			}

			//! Constructs an optional object that contains a value, initialized as if direct-initializing (but not direct-list-initializing) an object of type T with the expression value. This constructor is constexpr if the constructor of T selected by direct-initialization is constexpr.
			constexpr optional(const T& value)
				: _memory()
			{
				static_assert(std::is_copy_constructible<T>::value, "The value type must meet the requirements of CopyConstructible in order to use construction by copy.");
				
				_hasValueRef() = true;
				new(_valueMemoryPtr()) T(value);
			}

			//!  Constructs an optional object that contains a value, initialized as if direct-initializing (but not direct-list-initializing) an object of type T with the expression std::move(value). This constructor is constexpr if the constructor of T selected by direct-initialization is constexpr.
			constexpr optional(T&& value)
				: _memory()
			{
				static_assert(std::is_move_constructible<T>::value, "The value type must meet the requirements of MoveConstructible in order to use construction by move.");
				
				_hasValueRef() = true;
				new(_valueMemoryPtr()) T(std::move(value));
			}

			//! Constructs an optional object that contains a value, initialized as if direct-initializing (but not direct-list-initializing) an object of type T from the arguments std::forward<Args>(args)...
			template< class... Args >
			constexpr explicit optional(tools::in_place_t, Args&&... args)
				: _memory()
			{
				static_assert(std::is_constructible<T, Args&&...>::value, "The value type must be constructible from the supplied Args.");

				_hasValueRef() = true;
				new(_valueMemoryPtr()) T(std::forward<Args>(args)...);
			}

			//! Constructs an optional object that contains a value, initialized as if direct-initializing (but not direct-list-initializing) an object of type T from the arguments ilist, std::forward<Args>(args)....
			template< class U, class... Args >
			constexpr explicit optional(tools::in_place_t, std::initializer_list<U> ilist, Args&&... args)
				: _memory()
			{
				static_assert(std::is_constructible<T, std::initializer_list<U>&, Args&&...>::value, "The value type must be constructible from std::initializer_list and supplied Args.");

				_hasValueRef() = true;
				new(_valueMemoryPtr()) T(ilist, std::forward<Args>(args)...);
			}

			//! Calls the constructor of the contained value if it is not trivially destructible.
			~optional()
			{
				if (_hasValueRefConst()) (**this).~T();
			}

			//! If *this contains a value before the call, the contained value is destroyed by calling its destructor.
			optional& operator=(nullopt_t)
			{
				if (_hasValueRefConst()) {
					(**this).~T();
					_hasValueRef() = false;
				}
				return *this;
			}

			//! Replaces contents of *this with the contents of other.
			optional& operator=(const optional& other)
			{
				static_assert(std::is_copy_constructible<T>::value && std::is_copy_assignable<T>::value, "The value type must meet the requirements of CopyConstructible and CopyAssignable in order to use construction by move.");

				if (_hasValueRefConst() && other._hasValueRefConst()) {
					(**this) = *other;
				}
				else if (_hasValueRefConst()) {
					(**this).~T();
					_hasValueRef() = false;
				}
				else if (other._hasValueRefConst()) {
					new(_valueMemoryPtr()) T(*other);
					_hasValueRef() = true;
				}
				return *this;
			}

			//! Replaces contents of *this with the contents of other.
			optional& operator=(optional&& other)
			{
				static_assert(std::is_move_constructible<T>::value && std::is_move_assignable<T>::value, "The value type must meet the requirements of MoveConstructible and MoveAssignable in order to use construction by move.");

				if (_hasValueRefConst() && other._hasValueRefConst()) {
					(**this) = std::move(*other);
				}
				else if (_hasValueRefConst()) {
					(**this).~T();
					_hasValueRef() = false;
				}
				else if (other._hasValueRefConst()) {
					new(_valueMemoryPtr()) T(std::move(*other));
					_hasValueRef() = true;
				}
				return *this;
			}

			//! Replaces contents of *this with value.
			template< class U >
			optional& operator=(U&& value)
			{
				static_assert(std::is_same<std::decay_t<U>, T>::value, "The supplied value must be of the same type as the optional value.");

				if (_hasValueRefConst()) {
					(**this) = std::move(value);
				}
				else {
					new(_valueMemoryPtr()) T(std::move(value));
					_hasValueRef() = true;
				}
				return *this;
			}

			//! Accesses the contained value. Returns const-pointer.
			constexpr const T* operator->() const
			{
				return static_cast<const T*>(_valueMemoryPtrConst());
			}

			//! Accesses the contained value. Returns pointer.
			NONAME_CONSTEXPR_ T* operator->()
			{
				return static_cast<T*>(_valueMemoryPtr());
			}

			//! Accesses the contained value. Returns const-ref.
			constexpr const T& operator*() const&
			{
				return *static_cast<const T*>(_valueMemoryPtrConst());
			}

			//! Accesses the contained value. Returns ref.
			NONAME_CONSTEXPR_ T& operator*() &
			{
				return *static_cast<T*>(_valueMemoryPtr());
			}

			//! Accesses the contained value.
			NONAME_CONSTEXPR_ T&& operator*() &&
			{
				return std::move(*static_cast<T*>(_valueMemoryPtr()));
			}

			//! Checks whether *this contains a value.
			constexpr explicit operator bool() const { return _hasValueRefConst(); }

			//! Checks whether *this contains a value.
			constexpr bool has_value() const { return _hasValueRefConst(); }

			//! If *this contains a value, returns a reference to the contained value.
			NONAME_CONSTEXPR_ T& value() &
			{
				if (_hasValueRefConst()) return **this; throw bad_optional_access("Optional is empty.");
			}

			//! If *this contains a value, returns a const-reference to the contained value.
			constexpr const T & value() const &
			{
				if (_hasValueRefConst()) return **this; throw bad_optional_access("Optional is empty.");
			}

			//! If *this contains a value, returns a reference to the contained value.
			NONAME_CONSTEXPR_ T&& value() &&
			{
				if (_hasValueRefConst()) return std::move(**this); throw bad_optional_access("Optional is empty.");
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

				if (_hasValueRefConst() && other._hasValueRefConst()) {
					using std::swap;
					swap(**this, *other);
				}
				else if (_hasValueRefConst()) {
					new(other._valueMemoryPtr()) T(std::move(**this));
					(**this).~T();
					std::swap(this->_hasValueRef(), other._hasValueRef());
				}
				else if (other._hasValueRefConst()) {
					new(_valueMemoryPtr()) T(std::move(*other));
					other->T::~T();
					std::swap(this->_hasValueRef(), other._hasValueRef());
				}
			}

			//! If *this contains a value, destroy that value as if by value().T::~T(). Otherwise, there are no effects. *this does not contain a value after this call.
			void reset()
			{
				if (_hasValueRefConst()) (**this).~T();
				_hasValueRef() = false;
			}

			//! Constructs the contained value in-place. If *this already contains a value before the call, the contained value is destroyed by calling its destructor.
			template< class... Args >
			void emplace(Args&&... args)
			{
				static_assert(std::is_constructible<T, Args&&...>::value, "The value type must be constructible from the supplied Args.");
				if (_hasValueRefConst()) (**this).~T();
				new(_valueMemoryPtr()) T(std::forward<Args>(args)...);
				_hasValueRef() = true;
			}

			//! Constructs the contained value in-place. If *this already contains a value before the call, the contained value is destroyed by calling its destructor.
			template< class U, class... Args >
			void emplace(std::initializer_list<U> ilist, Args&&... args)
			{
				static_assert(std::is_constructible<T, std::initializer_list<U>&, Args&&...>::value, "The value type must be constructible from std::initializer_list and supplied Args.");
				if (_hasValueRefConst()) (**this).~T();
				new(_valueMemoryPtr()) T(ilist, std::forward<Args>(args)...);
				_hasValueRef() = true;
			}
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

#undef NONAME_CONSTEXPR_
