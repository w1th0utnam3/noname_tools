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

#include <type_traits>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <memory>
#include <array>

#if defined(_MSC_VER) && _MSC_VER < 1910
#define NONAME_CONSTEXPR
#else
#define NONAME_CONSTEXPR constexpr
#endif

#define NONAME_REQUIRES(...) typename std::enable_if<__VA_ARGS__::value, bool>::type = false

// TODO: swap()
// TODO: comparison operators
// TODO: type based in-place constructor
// TODO: SFINAE away copy and move constructor, etc.
// TODO: noexcept according to reference
// TODO: visit() with multiple variants

#pragma warning( push )
#pragma warning( disable : 4396 )

namespace noname
{
	namespace tools
	{
		//! Unit type intended for use as a well-behaved empty alternative in variant.
		struct monostate {};

		//! Exception thrown when get(variant) is called with an index or type that does not match the currently active alternative or visit is called to visit a variant that is valueless_by_exception.
		class bad_variant_access : public std::logic_error
		{
		public:
			explicit bad_variant_access(const std::string& what_arg) : std::logic_error{ what_arg } {}
			explicit bad_variant_access(const char* what_arg) : std::logic_error{ what_arg } {}
		};

		//! This is a special value equal to the largest value representable by the type std::size_t, used as the return type of index() when valueless_by_exception() is true.
		constexpr std::size_t variant_npos = -1;

		// Forward declaration of variant
		template <class... Types>
		class variant;

		template <class T>
		struct variant_size;

		//! Provides access to the number of alternatives in a possibly cv - qualified variant as a compile-time constant expression.
		template <class... Types>
		struct variant_size<variant<Types...>>
			: std::integral_constant<std::size_t, sizeof...(Types)>
		{
		};

		template <class T>
		struct variant_size<const T> : std::integral_constant<std::size_t, variant_size<T>::value> {};

		template <class T>
		struct variant_size<volatile T> : std::integral_constant<std::size_t, variant_size<T>::value> {};

		template <class T>
		struct variant_size<const volatile T> : std::integral_constant<std::size_t, variant_size<T>::value> {};

		template <class T>
		constexpr std::size_t variant_size_v = variant_size<T>::value;

		template <std::size_t I, class T>
		struct variant_alternative;

		//! Provides compile-time indexed access to the types of the alternatives of the possibly cv-qualified variant, combining cv-qualifications of the variant (if any) with the cv-qualifications of the alternative.
		template <std::size_t I, class... Types>
		struct variant_alternative<I, variant<Types...>>
		{
			static_assert(I < sizeof...(Types), "Type index for variant is out of range.");
			using type = nth_element_t<I, Types...>;
		};

		template <std::size_t I, class T>
		struct variant_alternative<I, const T>
		{
			using type = std::add_const_t<typename variant_alternative<I, T>::type>;
		};

		template <std::size_t I, class T>
		struct variant_alternative<I, volatile T>
		{
			using type = std::add_volatile_t<typename variant_alternative<I, T>::type>;
		};

		template <std::size_t I, class T>
		struct variant_alternative<I, const volatile T>
		{
			using type = std::add_cv_t<typename variant_alternative<I, T>::type>;
		};

		template <std::size_t I, class T>
		using variant_alternative_t = typename variant_alternative<I, T>::type;

		namespace _detail
		{
			//! Index value of the specified alternative type or variant_npos if there are several indices for the type.
			template <typename T, class... Types>
			struct _alternative_index : std::conditional_t<(count_element_v<T, Types...> > 1), std::integral_constant<std::size_t, variant_npos>
				, element_index<T, Types...>>
			{
			};

			template <typename T, class... Types>
			constexpr std::size_t _alternative_index_v = _alternative_index<T, Types...>::value;

			//! Specifies whether all types from the pack are constexpr compatible
			template <class... Types>
			struct _is_constexpr_pack : conjunction<std::is_trivially_destructible<Types>...>
			{
			};

			template <class... Types>
			constexpr bool _is_constexpr_pack_v = _is_constexpr_pack<Types...>::value;

			// Forward declaration of _get_if for friend declaration
			template <std::size_t I, class... Types>
			inline constexpr std::enable_if_t<_is_constexpr_pack<Types...>::value, std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>> _get_if(variant<Types...>* var_ptr);

			template <std::size_t I, class... Types>
			inline std::enable_if_t<negation<_is_constexpr_pack<Types...>>::value, std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>> _get_if(variant<Types...>* var_ptr);

			template <std::size_t I, class... Types>
			union _constexpr_variant_storage;

			template <std::size_t I, class T>
			union _constexpr_variant_storage <I, T>
			{
				T _value;

				_constexpr_variant_storage() = default;

				template <class... Args>
				constexpr _constexpr_variant_storage(std::integral_constant<std::size_t, I>, Args&&... args)
					: _value(std::forward<Args>(args)...)
				{
				}

				template <class U, class... Args>
				constexpr _constexpr_variant_storage(std::integral_constant<std::size_t, I>, std::initializer_list<U> il, Args&&... args)
					: _value(il, std::forward<Args>(args)...)
				{
				}

				template <class... Args>
				T& emplace_value(std::integral_constant<std::size_t, I>, Args&&... args)
				{
					_value = T(std::forward<Args>(args)...);
					return _value;
				}

				template <class U, class... Args>
				T& emplace_value(std::integral_constant<std::size_t, I>, std::initializer_list<U> il, Args&&... args)
				{
					_value = T(il, std::forward<Args>(args)...);
					return _value;
				}

				template <class U>
				void assign_value_conversion(std::integral_constant<std::size_t, I>, U&& u)
				{
					_value = T(u);
				}
			};

			template <std::size_t I, class T, class... Types>
			union _constexpr_variant_storage<I, T, Types...>
			{
				T _value;
				_constexpr_variant_storage<I + 1, Types...> _subvalues;

				_constexpr_variant_storage() = default;

				template <class... Args>
				constexpr _constexpr_variant_storage(std::integral_constant<std::size_t, I>, Args&&... args)
					: _value(std::forward<Args>(args)...)
				{
				}

				template <std::size_t V, class... Args>
				constexpr _constexpr_variant_storage(std::integral_constant<std::size_t, V> val, Args&&... args)
					: _subvalues(val, std::forward<Args>(args)...)
				{
				}

				template <std::size_t V, class U, class... Args>
				constexpr _constexpr_variant_storage(std::integral_constant<std::size_t, V> val, std::initializer_list<U> il, Args&&... args)
					: _subvalues(val, il, std::forward<Args>(args)...)
				{
				}

				template <class... Args>
				T& emplace_value(std::integral_constant<std::size_t, I>, Args&&... args)
				{
					_value = T(std::forward<Args>(args)...);
					return _value;
				}

				template <class U, class... Args>
				T& emplace_value(std::integral_constant<std::size_t, I>, std::initializer_list<U> il, Args&&... args)
				{
					_value = T(il, std::forward<Args>(args)...);
					return _value;
				}

				template <std::size_t V, class... Args>
				nth_element_t<V - I - 1, Types...>& emplace_value(std::integral_constant<std::size_t, V> val, Args&&... args)
				{
					return _subvalues.emplace_value(val, std::forward<Args>(args)...);
				}

				template <std::size_t V, class U, class... Args>
				nth_element_t<V - I - 1, Types...>& emplace_value(std::integral_constant<std::size_t, V> val, std::initializer_list<U> il, Args&&... args)
				{
					return _subvalues.emplace_value(val, il, std::forward<Args>(args)...);
				}

				template <class U>
				void assign_value_conversion(std::integral_constant<std::size_t, I>, U&& u)
				{
					_value = T(u);
				}

				template <std::size_t V, class U>
				void assign_value_conversion(std::integral_constant<std::size_t, V> val, U&& u)
				{
					_subvalues.assign_value_conversion(val, std::forward<U>(u));
				}
			};

			template <std::size_t N, class... Types>
			struct _constexpr_variant_impl;

			template <class... Types>
			struct _constexpr_variant_impl<0, Types...>
			{
				friend constexpr std::enable_if_t<true, std::add_pointer_t<variant_alternative_t<0, variant<Types...>>>> _get_if<0, Types...>(variant<Types...>* pv);

				_constexpr_variant_storage<0, Types...> _storage;
				std::size_t _index;
				bool _valueless;

				constexpr _constexpr_variant_impl()
					: _storage(std::integral_constant<std::size_t, 0>(), nth_element_t<0, Types...>())
					, _index(0)
					, _valueless(false)
				{
				}

				template <std::size_t I, class... Args>
				constexpr explicit _constexpr_variant_impl(in_place_index_t<I>, Args&&... args)
					: _storage(std::integral_constant<std::size_t, I>(), std::forward<Args>(args)...)
					, _index(I)
					, _valueless(false)
				{
				}

				template <std::size_t I, class U, class... Args>
				constexpr explicit _constexpr_variant_impl(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
					: _storage(std::integral_constant<std::size_t, I>(), il, std::forward<Args>(args)...)
					, _index(I)
					, _valueless(false)
				{
				}

				void destroy_safe() {};

				template <std::size_t I, class... Args>
				nth_element_t<I, Types...>& emplace(Args&&... args)
				{
					this->_index = I;
					return this->_storage.emplace_value(std::integral_constant<std::size_t, I>(), std::forward<Args>(args)...);
				}

				template <std::size_t I, class U, class... Args>
				nth_element_t<I, Types...>& emplace(std::initializer_list<U> il, Args&&... args)
				{
					this->_index = I;
					return this->_storage.emplace_value(std::integral_constant<std::size_t, I>(), il, std::forward<Args>(args)...);
				}

				template <class T_j, class T>
				void assign_conversion(T&& t)
				{
					this->_storage.assign_value_conversion(std::integral_constant<std::size_t, _alternative_index_v<T_j, Types...>>(), std::forward<T>(t));
				}
			};

			template <std::size_t N, class... Types>
			struct _constexpr_variant_impl : _constexpr_variant_impl<N - 1, Types...>
			{
				friend constexpr std::enable_if_t<true, std::add_pointer_t<variant_alternative_t<N, variant<Types...>>>> _get_if<N, Types...>(variant<Types...>* pv);

				constexpr _constexpr_variant_impl() = default;

				template <std::size_t I, class... Args>
				constexpr explicit _constexpr_variant_impl(in_place_index_t<I>, Args&&... args)
					: _constexpr_variant_impl<N - 1, Types...>(in_place<I>, std::forward<Args>(args)...)
				{
				}

				template <std::size_t I, class U, class... Args>
				constexpr explicit _constexpr_variant_impl(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
					: _constexpr_variant_impl<N - 1, Types...>(in_place<I>, il, std::forward<Args>(args)...)
				{
				}
			};

			template <class... Types>
			struct _variant_storage
			{
				std::aligned_union_t<0, Types...> _memory;

				void* memory_ptr() const
				{
					return const_cast<void*>(static_cast<const void*>(&_memory));
				}

				_variant_storage() = default;

				template <std::size_t I, class... Args>
				_variant_storage(std::integral_constant<std::size_t, I>, Args&&... args)
				{
					new(memory_ptr()) nth_element_t<I, Types...>(std::forward<Args>(args)...);
				}

				template <std::size_t I, class U, class... Args>
				_variant_storage(std::integral_constant<std::size_t, I>, std::initializer_list<U> il, Args&&... args)
				{
					new(memory_ptr()) nth_element_t<I, Types...>(il, std::forward<Args>(args)...);
				}

				template <class T>
				void copy_value(const void* other_ptr)
				{
					new(memory_ptr()) T(*static_cast<const T*>(other_ptr));
				}

				template <class T>
				void move_value(void* other_ptr)
				{
					new(memory_ptr()) T(std::move(*static_cast<T*>(other_ptr)));
				}

				template <class T>
				void destroy_value()
				{
					(*static_cast<T*>(memory_ptr())).T::~T();
				}

				template <class T, class... Args>
				T& emplace_value(Args&&... args)
				{
					new(memory_ptr()) T(std::forward<Args>(args)...);
					return (*static_cast<T*>(memory_ptr()));
				}

				template <class T, class U, class... Args>
				T& emplace_value(std::initializer_list<U> il, Args&&... args)
				{
					new(memory_ptr()) T(il, std::forward<Args>(args)...);
					return (*static_cast<T*>(memory_ptr()));
				}

				template <class T>
				T& emplace_value(const _variant_storage& other)
				{
					new(memory_ptr()) T(*static_cast<T*>(other.memory_ptr()));
					return (*static_cast<T*>(memory_ptr()));
				}

				template <class T_j, class T>
				void assign_value_conversion(T&& t)
				{
					(*static_cast<T_j*>(memory_ptr())) = std::forward<T>(t);
				}

				template <class T>
				void assign_value(const _variant_storage& other)
				{
					(*static_cast<T*>(memory_ptr())) = (*static_cast<T*>(other.memory_ptr()));
				}

				template <class T>
				void assign_value(_variant_storage&& other)
				{
					(*static_cast<T*>(memory_ptr())) = std::move(*static_cast<T*>(other.memory_ptr()));
				}
			};

			template <std::size_t N, class... Types>
			struct _variant_impl_base;

			template <class... Types>
			struct _variant_impl_base<0, Types...>
			{
				friend std::enable_if_t<true, std::add_pointer_t<variant_alternative_t<0, variant<Types...>>>> _get_if<0, Types...>(variant<Types...>* pv);

				_variant_storage<Types...> _storage;
				std::size_t _index;
				bool _valueless;

				_variant_impl_base()
					: _storage(std::integral_constant<std::size_t, 0>(), nth_element_t<0, Types...>())
					, _index(0)
					, _valueless(false)
				{
				}

				_variant_impl_base(const _variant_impl_base& other)
					: _index(other._index)
					, _valueless(other._valueless)
				{
					if (!other._valueless) this->_storage.template copy_value<nth_element_t<0, Types...>>(other._storage.memory_ptr());
				}

				_variant_impl_base(_variant_impl_base&& other)
					: _index(other._index)
					, _valueless(other._valueless)
				{
					if (!other._valueless) this->_storage.template move_value<nth_element_t<0, Types...>>(other._storage.memory_ptr());
				}

				template <std::size_t I, class... Args>
				explicit _variant_impl_base(in_place_index_t<I>, Args&&... args)
					: _storage(std::integral_constant<std::size_t, I>(), std::forward<Args>(args)...)
					, _index(I)
					, _valueless(false)
				{
				}

				template <std::size_t I, class U, class... Args>
				explicit _variant_impl_base(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
					: _storage(std::integral_constant<std::size_t, I>(), il, std::forward<Args>(args)...)
					, _index(I)
					, _valueless(false)
				{
				}

				~_variant_impl_base()
				{
					if (this->_index == 0 && !this->_valueless) this->_storage.template destroy_value<nth_element_t<0, Types...>>();
				}

			protected:
				void _destroy()
				{
					if (this->_index == 0) this->_storage.template destroy_value<nth_element_t<0, Types...>>();
				}

			public:
				void destroy_safe()
				{
					if (this->_index == 0 && !this->_valueless) this->_storage.template destroy_value<nth_element_t<0, Types...>>();
				}

				template <std::size_t I, class... Args>
				nth_element_t<I, Types...>& emplace(Args&&... args)
				{
					this->_index = I;
					this->_valueless = false;

					try {
						return this->_storage.template emplace_value<nth_element_t<I, Types...>>(std::forward<Args>(args)...);
					} catch(...) {
						this->_valueless = true;
						throw;
					}
				}

				template <std::size_t I, class U, class... Args>
				nth_element_t<I, Types...>& emplace(std::initializer_list<U> il, Args&&... args)
				{
					this->_index = I;
					this->_valueless = false;

					try {
						return this->_storage.template emplace_value<nth_element_t<I, Types...>>(il, std::forward<Args>(args)...);
					} catch (...) {
						this->_valueless = true;
						throw;
					}
				}

				template <class T_j, class T>
				void assign_conversion(T&& t)
				{
					this->_storage.template assign_value_conversion<T_j>(std::forward<T>(t));
					this->_valueless = false;
				}

				void assign_same_index(const _variant_impl_base& other)
				{
					this->_storage.template assign_value<nth_element_t<0, Types...>>(other._storage);
					this->_valueless = false;
				}

				void assign_same_index(_variant_impl_base&& other)
				{
					this->_storage.template assign_value<nth_element_t<0, Types...>>(std::forward<_variant_storage<Types...>>(other._storage));
					this->_valueless = false;
				}

				void assign_change_index(const _variant_impl_base& other)
				{
					if (std::is_nothrow_copy_constructible<nth_element_t<0, Types...>>::value || !std::is_nothrow_move_constructible<nth_element_t<0, Types...>>::value) {
						this->_index = 0;
						this->_storage.template emplace_value<nth_element_t<0, Types...>>(other._storage);
						this->_valueless = false;
					} else {
						_variant_impl_base<0, Types...>::assign_change_index(_variant_impl_base<0, Types...>(other));
					}
				}

				void assign_change_index(_variant_impl_base&& other)
				{
					this->_index = 0;
					this->_valueless = false;

					try {
						this->_storage.template move_value<nth_element_t<0, Types...>>(other._storage.memory_ptr());
					} catch (...) {
						this->_valueless = true;
						throw;
					}
				}				
			};

			template <std::size_t N, class... Types>
			struct _variant_impl_base : _variant_impl_base<N-1, Types...>
			{
				friend std::enable_if_t<true, std::add_pointer_t<variant_alternative_t<N, variant<Types...>>>> _get_if<N, Types...>(variant<Types...>* pv);

				_variant_impl_base() = default;

				_variant_impl_base(const _variant_impl_base& other)
					: _variant_impl_base<N-1, Types...>(other)
				{
					if (other._index == N && !other._valueless) this->_storage.template copy_value<nth_element_t<N, Types...>>(other._storage.memory_ptr());
				}

				_variant_impl_base(_variant_impl_base&& other)
					: _variant_impl_base<N-1, Types...>(std::forward<_variant_impl_base>(other))
				{
					if (other._index == N && !other._valueless) this->_storage.template move_value<nth_element_t<N, Types...>>(other._storage.memory_ptr());
				}

				template <std::size_t I, class... Args>
				explicit _variant_impl_base(in_place_index_t<I>, Args&&... args)
					: _variant_impl_base<N-1, Types...>(in_place<I>, std::forward<Args>(args)...)
				{
				}

				template <std::size_t I, class U, class... Args>
				explicit _variant_impl_base(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
					: _variant_impl_base<N-1, Types...>(in_place<I>, il, std::forward<Args>(args)...)
				{
				}

				~_variant_impl_base()
				{
					if (this->_index == N) this->_storage.template destroy_value<nth_element_t<N, Types...>>();
				}

				_variant_impl_base& operator=(const _variant_impl_base& rhs)
				{
					if (this->_valueless && rhs._valueless) return *this;
					if (rhs._valueless) {
						this->_destroy();
						this->_valueless = true;

						return *this;
					}

					if (this->_index == rhs._index) {
						this->assign_same_index(rhs);
					} else {
						this->destroy_safe();
						this->assign_change_index(rhs);
					}

					return *this;
				}

				_variant_impl_base& operator=(_variant_impl_base&& rhs) noexcept(conjunction<std::is_nothrow_move_constructible<Types>...,
																				   std::is_nothrow_move_assignable<Types>...>::value)
				{
					if (this->_valueless && rhs._valueless) return *this;
					if (rhs._valueless) {
						if (!this->_valueless) this->_destroy();
						this->_valueless = true;

						return *this;
					}

					if (this->_index == rhs._index) {
						this->assign_same_index(std::forward<_variant_impl_base>(rhs));
					} else {
						this->destroy_safe();
						this->assign_change_index(std::forward<_variant_impl_base>(rhs));
					}

					return *this;
				}

			protected:
				void _destroy()
				{
					if (this->_index == N)
						this->_storage.template destroy_value<nth_element_t<N, Types...>>();
					else
						_variant_impl_base<N - 1, Types...>::_destroy();
				}

			public:
				void destroy_safe()
				{
					if (!this->_valueless) this->_destroy();
				}

				void assign_same_index(const _variant_impl_base& other)
				{
					if (other._index == N) {
						this->_storage.template assign_value<nth_element_t<N, Types...>>(other._storage);
						this->_valueless = false;
					} else {
						_variant_impl_base<N - 1, Types...>::assign_same_index(other);
					}
				}

				void assign_same_index(_variant_impl_base&& other)
				{
					if (other._index == N) {
						this->_storage.template assign_value<nth_element_t<N, Types...>>(std::forward<_variant_storage<Types...>>(other._storage));
						this->_valueless = false;
					} else {
						_variant_impl_base<N - 1, Types...>::assign_same_index(std::forward<_variant_impl_base>(other));
					}
				}

				void assign_change_index(const _variant_impl_base& other)
				{
					if (other._index == N) {
						if (std::is_nothrow_copy_constructible<nth_element_t<N, Types...>>::value || !std::is_nothrow_move_constructible<nth_element_t<N, Types...>>::value) {
							this->_index = N;
							this->_storage.template emplace_value<nth_element_t<N, Types...>>(other._storage);
							this->_valueless = false;
						} else {
							_variant_impl_base<N, Types...>::assign_change_index(_variant_impl_base<N, Types...>(other));
						}
					} else {
						_variant_impl_base<N - 1, Types...>::assign_change_index(other);
					}
				}

				void assign_change_index(_variant_impl_base&& other)
				{
					if (other._index == N) {
						this->_index = N;
						this->_valueless = false;

						try {
							this->_storage.template move_value<nth_element_t<N, Types...>>(other._storage.memory_ptr());
						} catch (...) {
							this->_valueless = true;
							throw;
						}
					} else {
						_variant_impl_base<N - 1, Types...>::assign_change_index(std::forward<_variant_impl_base>(other));
					}
				}
			};

			template <bool isDefaultConstructible, class... Types>
			struct _variant_default_constructor_layer;

			template <class... Types>
			struct _variant_default_constructor_layer<true, Types...> : _variant_impl_base<sizeof...(Types)-1, Types...>
			{
				using base = _variant_impl_base<sizeof...(Types)-1, Types...>;

				_variant_default_constructor_layer() = default;

				template <typename T>
				constexpr _variant_default_constructor_layer(T&& t)
					: base(in_place<_alternative_index_v<best_match<T&&, Types...>, Types...>>, std::forward<T>(t))
				{
				}

				template <class T, class... Args>
				constexpr explicit _variant_default_constructor_layer(in_place_type_t<T>, Args&&... args)
					: base(in_place<_alternative_index_v<T, Types...>>, std::forward<Args>(args)...)
				{
				}

				template <class T, class U, class... Args>
				constexpr explicit _variant_default_constructor_layer(in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
					: base(in_place<_alternative_index_v<T, Types...>>, il, std::forward<Args>(args)...)
				{
				}

				template <std::size_t I, class... Args>
				constexpr explicit _variant_default_constructor_layer(in_place_index_t<I>, Args&&... args)
					: base(in_place<I>, std::forward<Args>(args)...)
				{
				}

				template <std::size_t I, class U, class... Args>
				constexpr explicit _variant_default_constructor_layer(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
					: base(in_place<I>, il, std::forward<Args>(args)...)
				{
				}
			};

			template <class... Types>
			struct _variant_default_constructor_layer<false, Types...> : _variant_impl_base<sizeof...(Types)-1, Types...>
			{
				using base = _variant_impl_base<sizeof...(Types)-1, Types...>;

				_variant_default_constructor_layer() = delete;

				template <typename T>
				constexpr _variant_default_constructor_layer(T&& t)
					: base(in_place<_alternative_index_v<best_match<T&&, Types...>, Types...>>, std::forward<T>(t))
				{
				}

				template <class T, class... Args>
				constexpr explicit _variant_default_constructor_layer(in_place_type_t<T>, Args&&... args)
					: base(in_place<_alternative_index_v<T, Types...>>, std::forward<Args>(args)...)
				{
				}

				template <class T, class U, class... Args>
				constexpr explicit _variant_default_constructor_layer(in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
					: base(in_place<_alternative_index_v<T, Types...>>, il, std::forward<Args>(args)...)
				{
				}

				template <std::size_t I, class... Args>
				constexpr explicit _variant_default_constructor_layer(in_place_index_t<I>, Args&&... args)
					: base(in_place<I>, std::forward<Args>(args)...)
				{
				}

				template <std::size_t I, class U, class... Args>
				constexpr explicit _variant_default_constructor_layer(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
					: base(in_place<I>, il, std::forward<Args>(args)...)
				{
				}
			};

			template <class... Types>
			struct _variant_impl_composer : _variant_default_constructor_layer<std::is_default_constructible<nth_element_t<0, Types...>>::value, Types...>
			{
				using base = _variant_default_constructor_layer<std::is_default_constructible<nth_element_t<0, Types...>>::value, Types...>;

				_variant_impl_composer() = default;

				template <typename T>
				constexpr _variant_impl_composer(T&& t)
					: base(in_place<_alternative_index_v<best_match<T&&, Types...>, Types...>>, std::forward<T>(t))
				{}

				template <class T, class... Args>
				constexpr explicit _variant_impl_composer(in_place_type_t<T>, Args&&... args)
					: base(in_place<_alternative_index_v<T, Types...>>, std::forward<Args>(args)...) {}

				template <class T, class U, class... Args>
				constexpr explicit _variant_impl_composer(in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
					: base(in_place<_alternative_index_v<T, Types...>>, il, std::forward<Args>(args)...) {}

				template <std::size_t I, class... Args>
				constexpr explicit _variant_impl_composer(in_place_index_t<I>, Args&&... args)
					: base(in_place<I>, std::forward<Args>(args)...) {}

				template <std::size_t I, class U, class... Args>
				constexpr explicit _variant_impl_composer(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
					: base(in_place<I>, il, std::forward<Args>(args)...) {}
			};

			// Only trivially destructible types can be used in constexpr objects
			template <class... Types>
			using _variant_impl_t = typename std::conditional<
				_is_constexpr_pack<Types...>::value,
				_constexpr_variant_impl<sizeof...(Types)-1, Types...>,
				_variant_impl_composer<Types...>
			>::type;

		} // namespace _detail

		//! Type-safe union. An instance of variant at any given time either holds a value of one of its alternative types, or it holds no value.
		template <class... Types>
		class variant
		{
			template <std::size_t I, class... Ts>
			friend inline constexpr std::enable_if_t<_is_constexpr_pack<Ts...>::value, std::add_pointer_t<variant_alternative_t<I, variant<Ts...>>>> _detail::_get_if(variant<Ts...>* var_ptr);

			template <std::size_t I, class... Ts>
			friend inline std::enable_if_t<negation<_is_constexpr_pack<Ts...>>::value, std::add_pointer_t<variant_alternative_t<I, variant<Ts...>>>> _detail::_get_if(variant<Ts...>* var_ptr);

			static_assert(sizeof...(Types) > 0, "Instantiation of variant<> is not allowed");
			static_assert(conjunction<negation<std::is_array<Types>>...>::value, "Arrays are not allowed in variant");
			static_assert(conjunction<negation<std::is_reference<Types>>...>::value, "References are not allowed in variant");
			static_assert(conjunction<negation<std::is_void<Types>>...>::value, "Void is not allowed in variant");

			_detail::_variant_impl_t<Types...> _impl;

		public:
			//! Default constructor. Constructs a variant holding the value-initialized value of the first alternative ('index()' is zero).
			constexpr variant() = default;
			//! Copy constructor. If other is not 'valueless_by_exception', constructs a variant holding the same alternative as 'other' and direct-initializes the contained value with 'std::get<other.index()>(other)'.
			variant(const variant& other) = default;
			//! Move constructor. If other is not 'valueless_by_exception', constructs a variant holding the same alternative as 'other' and direct-initializes the contained value with 'std::get<other.index()>(std::move(other))'. 
			variant(variant&& other) = default;

			//! Converting constructor.
			template <typename T, NONAME_REQUIRES(conjunction<std::is_constructible<best_match<T&&, Types...>, T>, 
															  negation<std::is_same<std::decay_t<T>, variant>>>)>
			constexpr variant(T&& t)
				: _impl(in_place<_detail::_alternative_index_v<best_match<T&&, Types...>, Types...>>, std::forward<T>(t))
			{
			}

			//! Constructs a variant with the specified alternative 'T' and initializes the contained value with the arguments 'std::forward<Args>(args)...'.
			template <class T, class... Args, NONAME_REQUIRES(conjunction<bool_constant<_detail::_alternative_index_v<T, Types...> != variant_npos>,
																		  std::is_constructible<nth_element_t<_detail::_alternative_index_v<T, Types...>, Types...>, Args...>>)>
			constexpr explicit variant(in_place_type_t<T>, Args&&... args)
				: _impl(in_place<_detail::_alternative_index_v<T, Types...>>, std::forward<Args>(args)...)
			{
			}

			//! Constructs a variant with the specified alternative 'T' and initializes the contained value with the arguments 'il, std::forward<Args>(args)...'.
			template <class T, class U, class... Args, NONAME_REQUIRES(conjunction<bool_constant<_detail::_alternative_index_v<T, Types...> != variant_npos>,
																				   std::is_constructible<nth_element_t<_detail::_alternative_index_v<T, Types...>, Types...>, Args...>>)>
			constexpr explicit variant(in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
				: _impl(in_place<_detail::_alternative_index_v<T, Types...>>, il, std::forward<Args>(args)...)
			{
			}

			//! Constructs a variant with the alternative 'T_i' specified by the index 'I' and initializes the contained value with the arguments 'std::forward<Args>(args)...'.
			template <std::size_t I, class... Args, NONAME_REQUIRES(conjunction<bool_constant<I < sizeof...(Types)>,
																			    std::is_constructible<nth_element_t<I, Types...>, Args...>>)>
			constexpr explicit variant(in_place_index_t<I>, Args&&... args)
				: _impl(in_place<I>, std::forward<Args>(args)...)
			{
			}

			//! Constructs a variant with the alternative 'T_i' specified by the index 'I' and initializes the contained value with the arguments 'il, std::forward<Args>(args)...'.
			template <std::size_t I, class U, class... Args, NONAME_REQUIRES(conjunction<bool_constant<I < sizeof...(Types)>,
																						 std::is_constructible<nth_element_t<I, Types...>, Args...>>)>
			constexpr explicit variant(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
				: _impl(in_place<I>, il, std::forward<Args>(args)...)
			{
			}

			//! Destructor
			~variant() = default;

			//! Returns false if and only if the variant holds a value.
			constexpr bool valueless_by_exception() const
			{
				return this->_impl._valueless;
			}

			//! Returns the zero-based index of the alternative that is currently held by the variant.
			constexpr std::size_t index() const
			{
				return (!valueless_by_exception()) ? this->_impl._index : variant_npos;
			}

			//! Creates a new value in-place, in an existing variant object using the supplied arguments 'args'.
			template <class T, class... Args, NONAME_REQUIRES(conjunction<bool_constant<_detail::_alternative_index_v<T, Types...> != variant_npos>, 
																		  std::is_constructible<nth_element_t<_detail::_alternative_index_v<T, Types...>, Types...>, Args...>>)>
			T& emplace(Args&&... args)
			{
				this->_impl.destroy_safe();
				return this->_impl.template emplace<_detail::_alternative_index_v<T, Types...>>(std::forward<Args>(args)...);
			}

			//! Creates a new value in-place, in an existing variant object using the supplied initializer list 'il' and arguments 'args'.
			template <class T, class U, class... Args, NONAME_REQUIRES(conjunction<bool_constant<_detail::_alternative_index_v<T, Types...> != variant_npos>, 
																				   std::is_constructible<nth_element_t<_detail::_alternative_index_v<T, Types...>, Types...>, std::initializer_list<U>, Args...>>)>
			T& emplace(std::initializer_list<U> il, Args&&... args)
			{
				this->_impl.destroy_safe();
				return this->_impl.template emplace<_detail::_alternative_index_v<T, Types...>>(il, std::forward<Args>(args)...);
			}

			//! Creates a new value in-place, in an existing variant object using the supplied arguments 'args'.
			template <std::size_t I, class... Args, NONAME_REQUIRES(std::is_constructible<nth_element_t<I, Types...>, Args...>)>
			variant_alternative_t<I, variant<Types...>>& emplace(Args&&... args)
			{
				this->_impl.destroy_safe();
				return this->_impl.template emplace<I>(std::forward<Args>(args)...);
			}

			//! Creates a new value in-place, in an existing variant object using the supplied initializer list 'il' and arguments 'args'.
			template <std::size_t I, class U, class... Args, NONAME_REQUIRES(std::is_constructible<nth_element_t<I, Types...>, std::initializer_list<U>, Args...>)>
			variant_alternative_t<I, variant<Types...>>& emplace(std::initializer_list<U> il, Args&&... args)
			{
				this->_impl.destroy_safe();
				return this->_impl.template emplace<I>(il, std::forward<Args>(args)...);
			}

			//! Copy-assignment.
			variant& operator=(const variant& rhs) = default;
			
			//! Move-assignment.
			variant& operator=(variant&& rhs) = default;

			//! Converting assignment.
			template <class T, NONAME_REQUIRES(conjunction<negation<std::is_same<typename std::decay<T>::type, variant>>, 
														   std::is_assignable<best_match<T&&, Types...>&, T>,
														   std::is_constructible<best_match<T&&, Types...>, T>>)>
			variant& operator=(T&& t) noexcept(std::is_nothrow_assignable<best_match<T&&, Types...>&, T>::value && std::is_nothrow_constructible<best_match<T&&, Types...>, T>::value)
			{
				using T_j = best_match<T&&, Types...>;
				if(this->_impl._index == _detail::_alternative_index_v<T_j, Types...>) {
					this->_impl.template assign_conversion<T_j>(std::forward<T>(t));
				} else {
					this->_impl.destroy_safe();
					this->_impl.template emplace<_detail::_alternative_index_v<T_j, Types...>>(std::forward<T>(t));
				}
				return *this;
			}
		};

		namespace _detail
		{
			template <std::size_t I, class PtrT, class StorageT>
			struct _get_if_constexpr;

			template <class PtrT, class StorageT>
			struct _get_if_constexpr<0, PtrT, StorageT>
			{
				PtrT ptr;

				constexpr _get_if_constexpr(StorageT* str_ptr)
					: ptr(static_cast<PtrT>(&str_ptr->_value))
				{
				}
			};

			template <std::size_t I, class PtrT, class StorageT>
			struct _get_if_constexpr
			{
				PtrT ptr;

				constexpr _get_if_constexpr(StorageT* str_ptr)
					: ptr(_get_if_constexpr<I - 1, PtrT, decltype(str_ptr->_subvalues)>(&str_ptr->_subvalues).ptr)
				{
				}
			};

			template <std::size_t I, class... Types>
			inline constexpr std::enable_if_t<_is_constexpr_pack<Types...>::value,
											  std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>> 
				_get_if(variant<Types...>* var_ptr)
			{
				using value_ptr_t = std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>;
				return (var_ptr != nullptr && var_ptr->index() == I) ? _get_if_constexpr<I, value_ptr_t, decltype(var_ptr->_impl._storage)>(&var_ptr->_impl._storage).ptr : nullptr;
			}

			template <std::size_t I, class... Types>
			inline std::enable_if_t<negation<_is_constexpr_pack<Types...>>::value,
									std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>>
				_get_if(variant<Types...>* var_ptr)
			{
				using value_ptr_t = std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>;
				return (var_ptr != nullptr && var_ptr->index() == I) ? static_cast<value_ptr_t>(var_ptr->_impl._storage.memory_ptr()) : nullptr;
			}
		} // namespace _detail

		//! Index-based non-throwing accessor: If 'pv' is not a null pointer and 'pv->index() == I', returns a pointer to the value stored in the variant pointed to by 'pv'. Otherwise, returns a null pointer value.
		template <std::size_t I, class... Types>
		inline constexpr std::add_pointer_t<variant_alternative_t<I, variant<Types...>>> get_if(variant<Types...>* pv) noexcept
		{
			return _detail::_get_if<I>(pv);
		}

		//! Index-based non-throwing accessor: If 'pv' is not a null pointer and 'pv->index() == I', returns a const pointer to the value stored in the variant pointed to by 'pv'. Otherwise, returns a null pointer value.
		template <std::size_t I, class... Types>
		inline constexpr std::add_pointer_t<const variant_alternative_t<I, variant<Types...>>> get_if(const variant<Types...>* pv) noexcept
		{
			return _detail::_get_if<I>(const_cast<variant<Types...>*>(pv));
		}

		//! Type-based non-throwing accessor: Equivalent to 'get_if<I,Types...>' with 'I' being the zero-based index of 'T' in 'Types...'.
		template <class T, class... Types>
		inline constexpr std::add_pointer_t<T> get_if(variant<Types...>* pv) noexcept
		{
			return _detail::_get_if<_detail::_alternative_index_v<T, Types...>>(pv);
		}

		//! Type-based non-throwing accessor: Equivalent to 'get_if<I,Types...>' with 'I' being the zero-based index of 'T' in 'Types...'.
		template <class T, class... Types>
		inline constexpr std::add_pointer_t<const T> get_if(const variant<Types...>* pv) noexcept
		{
			return _detail::_get_if<_detail::_alternative_index_v<T, Types...>>(const_cast<variant<Types...>*>(pv));
		}

		//! Index-based value accessor: If 'v.index() == I', returns a reference to the value stored in 'v'. Otherwise, throws 'bad_variant_access'.
		template<std::size_t I, class... Types>
		inline NONAME_CONSTEXPR variant_alternative_t<I, variant<Types...>>& get(variant<Types...>& v)
		{
			using result_ptr_t = std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>;
			result_ptr_t ptr = get_if<I, Types...>(&v);
			return *((ptr != nullptr) ? ptr : (throw bad_variant_access("bad_variant_access"), ptr));
		}

		//! Index-based value accessor: If 'v.index() == I', returns a reference to the value stored in 'v'. Otherwise, throws 'bad_variant_access'.
		template<std::size_t I, class... Types, NONAME_REQUIRES(_detail::_is_constexpr_pack<Types...>)>
		inline constexpr variant_alternative_t<I, variant<Types...>> const& get(const variant<Types...>& v)
		{
			using result_ptr_t = std::add_pointer_t<const variant_alternative_t<I, variant<Types...>>>;

#if defined(_MSC_VER) && _MSC_VER < 1910
			return *(get_if<I, Types...>(std::addressof(v)));			
#else
			result_ptr_t ptr = get_if<I, Types...>(&v);
			return *((ptr != nullptr) ? ptr : (throw bad_variant_access("bad_variant_access"), ptr));
#endif
		}

		//! Index-based value accessor: If 'v.index() == I', returns a reference to the value stored in 'v'. Otherwise, throws 'bad_variant_access'.
		template<std::size_t I, class... Types, NONAME_REQUIRES(negation<_detail::_is_constexpr_pack<Types...>>)>
		inline variant_alternative_t<I, variant<Types...>> const& get(const variant<Types...>& v)
		{
			using result_ptr_t = std::add_pointer_t<const variant_alternative_t<I, variant<Types...>>>;
			result_ptr_t ptr = get_if<I, Types...>(&v);
			return *((ptr != nullptr) ? ptr : (throw bad_variant_access("bad_variant_access"), ptr));
		}

		//! Index-based value accessor: If 'v.index() == I', returns a reference to the value stored in 'v'. Otherwise, throws 'bad_variant_access'.
		template<std::size_t I, class... Types>
		inline NONAME_CONSTEXPR variant_alternative_t<I, variant<Types...>>&& get(variant<Types...>&& v)
		{
			using result_ptr_t = std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>;
			result_ptr_t ptr = get_if<I, Types...>(&v);
			return std::move(*((ptr != nullptr) ? ptr : (throw bad_variant_access("bad_variant_access"), ptr)));
		}

		//! Type-based value accessor: If v holds the alternative T, returns a reference to the value stored in v. 
		template <class T, class... Types>
		inline NONAME_CONSTEXPR T& get(variant<Types...>& v)
		{
			return get<_detail::_alternative_index_v<T, Types...>>(v);
		}

		//! Type-based value accessor: If v holds the alternative T, returns a reference to the value stored in v. 
		template <class T, class... Types, NONAME_REQUIRES(_detail::_is_constexpr_pack<Types...>)>
		inline constexpr const T& get(const variant<Types...>& v)
		{
			return get<_detail::_alternative_index_v<T, Types...>>(v);
		}

		//! Type-based value accessor: If v holds the alternative T, returns a reference to the value stored in v. 
		template <class T, class... Types, NONAME_REQUIRES(negation<_detail::_is_constexpr_pack<Types...>>)>
		inline const T& get(const variant<Types...>& v)
		{
			return get<_detail::_alternative_index_v<T, Types...>>(v);
		}

		//! Type-based value accessor: If v holds the alternative T, returns a reference to the value stored in v. 
		template <class T, class... Types>
		inline NONAME_CONSTEXPR T&& get(variant<Types...>&& v)
		{
			return get<_detail::_alternative_index_v<T, Types...>>(std::forward<variant<Types...>>(v));
		}

		namespace _detail
		{
			template <std::size_t I>
			struct _dispatcher_t
			{
				template <class Visitor, class Variant>
				static inline constexpr decltype(auto) _dispatch(Visitor visitor, Variant variant)
				{
					return visitor(get<I>(variant));
				}
			};

			template <class Visitor, class Variant, std::size_t I>
			static inline constexpr auto _make_dispatcher()
			{
				return _dispatcher_t<I>::template _dispatch<Visitor, Variant>;
			}

			template <class... Fs>
			static inline constexpr auto _make_array(Fs&&... fs)
			{
				using array_t = std::array<typename std::common_type<typename std::decay<Fs>::type...>::type, sizeof...(fs)>;
				return array_t{ std::forward<Fs>(fs)... };
			}

			template <class Visitor, class Variant, std::size_t... Is>
			static inline constexpr auto _make_callarray_impl(std::index_sequence<Is...>)
			{
				return _make_array(_make_dispatcher<Visitor, Variant, Is>()...);
			}

			template <class Visitor, class Variant>
			static inline constexpr auto _make_callarray()
			{
				using Indices = std::make_index_sequence<variant_size_v<typename std::decay<Variant>::type>>;

				return _make_callarray_impl<Visitor, Variant>(Indices());
			}
		} // namespace _detail

		template <class Visitor, class Variant>
		static inline constexpr decltype(auto) visit(Visitor&& visitor, Variant&& var)
		{
			if (var.valueless_by_exception()) throw bad_variant_access("bad_variant_access");
#if defined(_MSC_VER) && _MSC_VER < 1910
			return _detail::_make_callarray<Visitor&&, Variant>()[var.index()](std::forward<Visitor>(visitor), std::forward<Variant>(var));
#else
			const std::size_t currentIndex = var.index();

			constexpr auto call_array = _detail::_make_callarray<Visitor&&, Variant>();
			return call_array[currentIndex](std::forward<Visitor>(visitor), std::forward<Variant>(var));
#endif
		}
	}
}

#pragma warning( pop )

#undef NONAME_CONSTEXPR
#undef NONAME_REQUIRES
