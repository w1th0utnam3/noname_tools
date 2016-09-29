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
#include <initializer_list>
#include <cassert>

// TODO: Comparison operators? :/

#ifdef _MSC_VER
#define _NONAME_CONSTEXPR
#else
#define _NONAME_CONSTEXPR constexpr
#endif

#define _NONAME_REQUIRES(...) typename std::enable_if<__VA_ARGS__::value, bool>::type = false

namespace noname
{
	namespace tools
	{
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
			template <typename T>
			struct has_overloaded_addressof
			{
				template <class X>
				constexpr static bool has_overload(...) { return false; }

				template <class X, size_t S = sizeof(std::declval<X&>().operator&()) >
				constexpr static bool has_overload(bool) { return true; }

				constexpr static bool value = has_overload<T>(true);
			};

			template <typename T, _NONAME_REQUIRES(!has_overloaded_addressof<T>)>
			constexpr T* static_addressof(T& ref)
			{
				return &ref;
			}

			template <typename T, _NONAME_REQUIRES(has_overloaded_addressof<T>)>
			T* static_addressof(T& ref)
			{
				return std::addressof(ref);
			}

			// The call to convert<A>(b) has return type A and converts b to type A iff b decltype(b) is implicitly convertible to A  
			template <class U>
			constexpr U convert(U v) 
			{ 
				return v;
			}

		} // namespace _detail

		namespace _detail
		{
			constexpr struct _trivial_init_t {} _trivial_init{};

			template <class T>
			union _optional_storage
			{
				dummy_t _dummy;
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
				dummy_t _dummy;
				T _value;

				constexpr _constexpr_optional_storage(_trivial_init_t) noexcept
					: _dummy()
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
					, _storage(_trivial_init)
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

				template <class U, class... Args, _NONAME_REQUIRES(std::is_constructible<T, std::initializer_list<U>>)>
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

				template <class U, class... Args, _NONAME_REQUIRES(std::is_constructible<T, std::initializer_list<U>>)>
				_NONAME_CONSTEXPR explicit _constexpr_optional_base(in_place_t, std::initializer_list<U> il, Args&&... args)
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

			constexpr bool initialized() const noexcept { return _optional_base_t<T>::_init; }
			typename std::remove_const<T>::type* dataptr() { return std::addressof(_optional_base_t<T>::_storage._value); }
			constexpr const T* dataptr() const { return _detail::static_addressof(_optional_base_t<T>::_storage._value); }

			constexpr const T& contained_val() const& { return _optional_base_t<T>::_storage._value; }

			_NONAME_CONSTEXPR T&& contained_val() && { return std::move(_optional_base_t<T>::_storage._value); }
			_NONAME_CONSTEXPR T& contained_val() & { return _optional_base_t<T>::_storage._value; }

			template <class... Args>
			void initialize(Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...)))
			{
				assert(!_optional_base_t<T>::_init);
				::new (static_cast<void*>(dataptr())) T(std::forward<Args>(args)...);
				_optional_base_t<T>::_init = true;
			}

			template <class U, class... Args>
			void initialize(std::initializer_list<U> il, Args&&... args) noexcept(noexcept(T(il, std::forward<Args>(args)...)))
			{
				assert(!_optional_base_t<T>::_init);
				::new (static_cast<void*>(dataptr())) T(il, std::forward<Args>(args)...);
				_optional_base_t<T>::_init = true;
			}

		public:
			//! Type of the contained value
			typedef T value_type;

			//! Constructs an optional object that does not contain a value.
			constexpr optional() noexcept : _optional_base_t<T>() {};
			//! Constructs an optional object that does not contain a value.
			constexpr optional(nullopt_t) noexcept : _optional_base_t<T>() {};

			//! Copy constructor: If 'other' contains a value, initializes the contained value as if direct-initializing an object of type 'T' with the expression '*other'. If 'other' does not contain a value, constructs an object that does not contain a value.
			optional(const optional& rhs)
				: _optional_base_t<T>()
			{
				if (rhs.initialized()) {
					::new (static_cast<void*>(dataptr())) T(*rhs);
					_optional_base_t<T>::_init = true;
				}
			}
			
			//! Move constructor: If other contains a value, initializes the contained value as if direct-initializing an object of type 'T' with the expression std::move(*other) and does not make other empty: a moved-from optional still contains a value, but the value itself is moved from. If other does not contain a value, constructs an object that does not contain a value.
			optional(optional&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value)
				: _optional_base_t<T>()
			{
				if (rhs.initialized()) {
					::new (static_cast<void*>(dataptr())) T(std::move(*rhs));
					_optional_base_t<T>::_init = true;
				}
			}

			//! Constructs an optional object that contains a value, initialized as if direct-initializing (but not direct-list-initializing) an object of type 'T' with the expression value. This constructor is constexpr if the constructor of T selected by direct-initialization is constexpr.
			constexpr optional(const T& v) : _optional_base_t<T>(v)
			{
			}

			//! Constructs an optional object that contains a value, initialized as if direct - initializing(but not direct-list-initializing) an object of type 'T' with the expression 'std::move(value)'.This constructor is constexpr if the constructor of 'T' selected by direct-initialization is constexpr.
			constexpr optional(T&& v) : _optional_base_t<T>(std::move(v)) 
			{
			}

			//! Constructs an optional object that contains a value, initialized as if direct-initializing (but not direct-list-initializing) an object of type 'T' from the arguments 'std::forward<Args>(args)...'.
			template <class... Args>
			explicit constexpr optional(in_place_t, Args&&... args)
				: _optional_base_t<T>(in_place, std::forward<Args>(args)...) 
			{
			}

			//! Constructs an optional object that contains a value, initialized as if direct-initializing (but not direct-list-initializing) an object of type 'T' from the arguments 'ilist, std::forward<Args>(args)...'.
			template <class U, class... Args, _NONAME_REQUIRES(std::is_constructible<T, std::initializer_list<U>>)>
			explicit optional(in_place_t, std::initializer_list<U> il, Args&&... args)
				: _optional_base_t<T>(in_place, il, std::forward<Args>(args)...) 
			{
			}

			//! Calls the constructor of the contained value if it is not trivially destructible.
			~optional() = default;

			//! If '*this' contains a value, destroy that value as if by 'value().T::~T()'. Otherwise, there are no effects. '*this' does not contain a value after this call.
			void reset() noexcept 
			{
				if (initialized()) dataptr()->T::~T();
				_optional_base_t<T>::_init = false;
			}

			//! If '*this' contains a value before the call, the contained value is destroyed by calling its destructor.
			optional& operator=(nullopt_t) noexcept
			{
				reset();
				return *this;
			}

			//! Replaces contents of '*this' with the contents of 'rhs'.
			optional& operator=(const optional& rhs)
			{
				if (initialized() == true && rhs.initialized() == false) reset();
				else if (initialized() == false && rhs.initialized() == true)  initialize(*rhs);
				else if (initialized() == true && rhs.initialized() == true)  contained_val() = *rhs;
				return *this;
			}

			//! Replaces contents of '*this' with the contents of 'rhs'.
			optional& operator=(optional&& rhs)
				noexcept(std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_constructible<T>::value)
			{
				if (initialized() == true && rhs.initialized() == false) reset();
				else if (initialized() == false && rhs.initialized() == true)  initialize(std::move(*rhs));
				else if (initialized() == true && rhs.initialized() == true)  contained_val() = std::move(*rhs);
				return *this;
			}

			//! Replaces contents of '*this' with 'value'.
			template <class U>
			auto operator=(U&& v)
				-> typename std::enable_if<std::is_same<typename std::decay<U>::type, T>::value, optional&>::type
			{
				if (initialized()) { contained_val() = std::forward<U>(v); }
				else { initialize(std::forward<U>(v)); }
				return *this;
			}

			//! Constructs the contained value in-place. If '*this' already contains a value before the call, the contained value is destroyed by calling its destructor.
			template <class... Args>
			void emplace(Args&&... args)
			{
				reset();
				initialize(std::forward<Args>(args)...);
			}

			//! Constructs the contained value in-place. If '*this' already contains a value before the call, the contained value is destroyed by calling its destructor.
			template <class U, class... Args>
			void emplace(std::initializer_list<U> il, Args&&... args)
			{
				reset();
				initialize<U, Args...>(il, std::forward<Args>(args)...);
			}

			//! Swaps the contents with those of 'rhs'.
			void swap(optional<T>& rhs) noexcept(std::is_nothrow_move_constructible<T>::value && noexcept(std::swap(std::declval<T&>(), std::declval<T&>())))
			{
				if (initialized() == true && rhs.initialized() == false) {
					rhs.initialize(std::move(**this)); reset();
				} else if (initialized() == false && rhs.initialized() == true) {
					initialize(std::move(*rhs)); rhs.reset();
				} else if (initialized() == true && rhs.initialized() == true) {
					using std::swap; swap(**this, *rhs);
				}
			}

			//! Checks whether '*this' contains a value.
			explicit constexpr operator bool() const noexcept
			{
				return initialized();
			}

			//! Checks whether '*this' contains a value.
			constexpr bool has_value() const noexcept
			{
				return initialized();
			}

			//! Accesses the contained value. Returns const-pointer.
			constexpr T const* operator->() const
			{
				assert(initialized());
				return dataptr();
			}

			//! Accesses the contained value. Returns pointer.
			_NONAME_CONSTEXPR T* operator->()
			{
				assert(initialized());
				return dataptr();
			}

			//! Accesses the contained value. Returns const-ref.
			constexpr T const& operator*() const&
			{
				assert(initialized());
				return contained_val();
			}

			//! Accesses the contained value. Returns ref.
			_NONAME_CONSTEXPR T& operator*() &
			{
				assert(initialized());
				return contained_val();
			}

			//! Accesses the contained value.
			_NONAME_CONSTEXPR T&& operator*() &&
			{
				assert(initialized());
				return std::move(contained_val());
			}

			//! If '*this' contains a value, returns a const-reference to the contained value.
			constexpr T const& value() const& {
				return initialized() ? contained_val() : (throw bad_optional_access("bad optional access"), contained_val());
			}

			//! If '*this' contains a value, returns a reference to the contained value.
			_NONAME_CONSTEXPR T& value() & {
				return initialized() ? contained_val() : (throw bad_optional_access("bad optional access"), contained_val());
			}

			//! If '*this' contains a value, returns a reference to the contained value.
			_NONAME_CONSTEXPR T&& value() && {
				if (!initialized()) throw bad_optional_access("bad optional access");
				return std::move(contained_val());
			}

			//! Returns the contained value if '*this' has a value, otherwise returns 'default_value'.
			template <class V>
			constexpr T value_or(V&& v) const&
			{
				return *this ? **this : _detail::convert<T>(std::forward<V>(v));
			}

			//! Returns the contained value if '*this' has a value, otherwise returns 'default_value'.
			template <class V>
			_NONAME_CONSTEXPR T value_or(V&& v) &&
			{
				return *this ? std::move(const_cast<optional<T>&>(*this).contained_val()) : _detail::convert<T>(std::forward<V>(v));
			}
		};

		//! Overloads the std::swap algorithm for optional.
		template <class T>
		void swap(optional<T>& x, optional<T>& y) noexcept(noexcept(x.swap(y)))
		{
			x.swap(y);
		}

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

#undef _NONAME_CONSTEXPR
#undef _NONAME_REQUIRES
