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
#define _NONAME_CONSTEXPR
#else
#define _NONAME_CONSTEXPR constexpr
#endif

#define _NONAME_REQUIRES(...) typename std::enable_if<__VA_ARGS__::value, bool>::type = false

// TODO: visit with multiple variants
// TODO: Own implementation of array to support constexpr in MSVC 2015
// TODO: Proper support for MSVC 2015, i.e. find out what works/doesn't work with constexpr
// TODO: Missing constructors and other functions
// TODO: Implement exception handling, valueless_by_exception
// TODO: SFINAE away copy and move constructor, etc.
// TODO: Operators
// TODO: noexcept according to reference

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
			};

			template <std::size_t I, class T, class... Types>
			union _constexpr_variant_storage<I, T, Types...>
			{
				T _value;
				_constexpr_variant_storage<I + 1, Types...> _subvalues;

				template <class... Args>
				constexpr _constexpr_variant_storage(std::integral_constant<std::size_t, I>, Args&&... args)
					: _value(std::forward<Args>(args)...)
				{
				}

				template <std::size_t V, class... Args>
				constexpr _constexpr_variant_storage(std::integral_constant<std::size_t, V>, Args&&... args)
					: _subvalues(std::integral_constant<std::size_t, V>(), std::forward<Args>(args)...)
				{
				}

				template <std::size_t V, class U, class... Args>
				constexpr _constexpr_variant_storage(std::integral_constant<std::size_t, V>, std::initializer_list<U> il, Args&&... args)
					: _subvalues(std::integral_constant<std::size_t, V>(), il, std::forward<Args>(args)...)
				{
				}
			};

			template <std::size_t N, class... Types>
			struct _constexpr_variant_base;

			template <class... Types>
			struct _constexpr_variant_base<0, Types...>
			{
				friend constexpr std::enable_if_t<true, std::add_pointer_t<variant_alternative_t<0, variant<Types...>>>> _get_if<0, Types...>(variant<Types...>* pv);

				_constexpr_variant_storage<0, Types...> _storage;
				std::size_t _index;

				constexpr _constexpr_variant_base()
					: _storage(std::integral_constant<std::size_t, 0>(), nth_element_t<0, Types...>())
					, _index(0)
				{
				}

				template <std::size_t I, class... Args>
				constexpr explicit _constexpr_variant_base(in_place_index_t<I>, Args&&... args)
					: _storage(std::integral_constant<std::size_t, I>(), std::forward<Args>(args)...)
					, _index(I)
				{
				}

				template <std::size_t I, class U, class... Args>
				constexpr explicit _constexpr_variant_base(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
					: _storage(std::integral_constant<std::size_t, I>(), il, std::forward<Args>(args)...)
					, _index(I)
				{
				}
			};

			template <std::size_t N, class... Types>
			struct _constexpr_variant_base : public _constexpr_variant_base<N - 1, Types...>
			{
				friend constexpr std::enable_if_t<true, std::add_pointer_t<variant_alternative_t<N, variant<Types...>>>> _get_if<N, Types...>(variant<Types...>* pv);

				constexpr _constexpr_variant_base() = default;

				template <std::size_t I, class... Args>
				constexpr explicit _constexpr_variant_base(in_place_index_t<I>, Args&&... args)
					: _constexpr_variant_base<N - 1, Types...>(in_place<I>, std::forward<Args>(args)...)
				{
				}

				template <std::size_t I, class U, class... Args>
				constexpr explicit _constexpr_variant_base(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
					: _constexpr_variant_base<N - 1, Types...>(in_place<I>, il, std::forward<Args>(args)...)
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
			};

			template <std::size_t N, class... Types>
			struct _variant_base;

			template <class... Types>
			struct _variant_base<0, Types...>
			{
				friend std::enable_if_t<true, std::add_pointer_t<variant_alternative_t<0, variant<Types...>>>> _get_if<0, Types...>(variant<Types...>* pv);

				_variant_storage<Types...> _storage;
				std::size_t _index;

				_variant_base()
					: _storage(std::integral_constant<std::size_t, 0>(), nth_element_t<0, Types...>())
					, _index(0)
				{
				}

				_variant_base(const _variant_base& other)
					: _index(other._index)
				{
					if (other._index == 0) this->_storage.template copy_value<nth_element_t<0, Types...>>(other._storage.memory_ptr());
				}

				_variant_base(_variant_base&& other)
					: _index(other._index)
				{
					if (other._index == 0) this->_storage.template move_value<nth_element_t<0, Types...>>(other._storage.memory_ptr());
				}

				template <std::size_t I, class... Args>
				explicit _variant_base(in_place_index_t<I>, Args&&... args)
					: _storage(std::integral_constant<std::size_t, I>(), std::forward<Args>(args)...)
					, _index(I)
				{
				}

				template <std::size_t I, class U, class... Args>
				explicit _variant_base(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
					: _storage(std::integral_constant<std::size_t, I>(), il, std::forward<Args>(args)...)
					, _index(I)
				{
				}

				~_variant_base()
				{
					if (this->_index == 0) this->_storage.template destroy_value<nth_element_t<0, Types...>>();
				}
			};

			template <std::size_t N, class... Types>
			struct _variant_base : public _variant_base<N-1, Types...>
			{
				friend std::enable_if_t<true, std::add_pointer_t<variant_alternative_t<N, variant<Types...>>>> _get_if<N, Types...>(variant<Types...>* pv);

				_variant_base() = default;

				_variant_base(const _variant_base& other)
					: _variant_base<N-1, Types...>(other)
				{
					if (other._index == N) this->_storage.template copy_value<nth_element_t<N, Types...>>(other._storage.memory_ptr());
				}

				_variant_base(_variant_base&& other)
					: _variant_base<N-1, Types...>(std::forward<_variant_base>(other))
				{
					if (other._index == N) this->_storage.template move_value<nth_element_t<N, Types...>>(other._storage.memory_ptr());
				}

				template <std::size_t I, class... Args>
				explicit _variant_base(in_place_index_t<I>, Args&&... args)
					: _variant_base<N-1, Types...>(in_place<I>, std::forward<Args>(args)...)
				{
				}

				template <std::size_t I, class U, class... Args>
				explicit _variant_base(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
					: _variant_base<N-1, Types...>(in_place<I>, il, std::forward<Args>(args)...)
				{
				}

				~_variant_base()
				{
					if (this->_index == N) this->_storage.template destroy_value<nth_element_t<N, Types...>>();
				}

			};

			// Only trivially destructible types can be used in constexpr objects
			template <class... Types>
			using _variant_base_t = typename std::conditional<
				_is_constexpr_pack<Types...>::value,
				_constexpr_variant_base<sizeof...(Types)-1, Types...>,
				_variant_base<sizeof...(Types)-1, Types...>
			>::type;

			//! Index value of the specified alternative type or variant_npos if there are several indices for the type.
			template <typename T, class... Types>
			struct _alternative_index : std::conditional_t<(count_element_v<T, Types...> > 1), std::integral_constant<std::size_t, variant_npos>
																							 , element_index<T, Types...>>
			{
			};

			template <typename T, class... Types>
			constexpr std::size_t _alternative_index_v = _alternative_index<T, Types...>::value;

		} // namespace _detail

		//! Type-safe union. An instance of variant at any given time either holds a value of one of its alternative types, or it holds no value.
		template <class... Types>
		class variant : public _detail::_variant_base_t<Types...>
		{
		public:
			//! Default constructor. Constructs a variant holding the value-initialized value of the first alternative ('index()' is zero).
			constexpr variant() 
				: _detail::_variant_base_t<Types...>()
			{
			}

			//! Copy constructor. If other is not 'valueless_by_exception', constructs a variant holding the same alternative as 'other' and direct-initializes the contained value with 'std::get<other.index()>(other)'.
			variant(const variant& other)
				: _detail::_variant_base_t<Types...>(other)
			{
			}

			//! Move constructor. If other is not 'valueless_by_exception', constructs a variant holding the same alternative as 'other' and direct-initializes the contained value with 'std::get<other.index()>(std::move(other))'. 
			variant(variant&& other)
				: _detail::_variant_base_t<Types...>(std::forward<variant>(other))
			{
			}

			//! Converting constructor.
			template <typename T, typename T_j = best_match<T&&, Types...>
								, typename = typename std::enable_if<	std::is_constructible<T_j,T>::value 
																	&& !std::is_same<std::decay_t<T>, variant>::value>::type>
			constexpr variant(T&& t)
				: _detail::_variant_base_t<Types...>(in_place<_detail::_alternative_index_v<T_j, Types...>>, std::forward<T>(t))
			{
			}

			//! Constructs a variant with the specified alternative 'T' and initializes the contained value with the arguments 'std::forward<Args>(args)...'.
			template <class T, class... Args, _NONAME_REQUIRES(conjunction<bool_constant<_detail::_alternative_index_v<T, Types...> != variant_npos>
																		   , std::is_constructible<nth_element_t<_detail::_alternative_index_v<T, Types...>, Types...>, Args...>>)>
			constexpr explicit variant(in_place_type_t<T>, Args&&... args)
				: _detail::_variant_base_t<Types...>(in_place<_detail::_alternative_index_v<T, Types...>>, std::forward<Args>(args)...)
			{
			}

			//! Constructs a variant with the specified alternative 'T' and initializes the contained value with the arguments 'il, std::forward<Args>(args)...'.
			template <class T, class U, class... Args, _NONAME_REQUIRES(conjunction<bool_constant<_detail::_alternative_index_v<T, Types...> != variant_npos>
																					, std::is_constructible<nth_element_t<_detail::_alternative_index_v<T, Types...>, Types...>, Args...>>)>
			constexpr explicit variant(in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
				: _detail::_variant_base_t<Types...>(in_place<_detail::_alternative_index_v<T, Types...>>, il, std::forward<Args>(args)...)
			{
			}

			//! Constructs a variant with the alternative 'T_i' specified by the index 'I' and initializes the contained value with the arguments 'std::forward<Args>(args)...'.
			template <std::size_t I, class... Args, _NONAME_REQUIRES(conjunction<bool_constant<I < sizeof...(Types)>
																				 , std::is_constructible<nth_element_t<I, Types...>, Args...>>)>
			constexpr explicit variant(in_place_index_t<I>, Args&&... args)
				: _detail::_variant_base_t<Types...>(in_place<I>, std::forward<Args>(args)...)
			{
			}

			//! Constructs a variant with the alternative 'T_i' specified by the index 'I' and initializes the contained value with the arguments 'il, std::forward<Args>(args)...'.
			template <std::size_t I, class U, class... Args, _NONAME_REQUIRES(conjunction<bool_constant<I < sizeof...(Types)>
																						  , std::is_constructible<nth_element_t<I, Types...>, Args...>>)>
			constexpr explicit variant(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
				: _detail::_variant_base_t<Types...>(in_place<I>, il, std::forward<Args>(args)...)
			{
			}

			//! Returns false if and only if the variant holds a value.
			constexpr bool valueless_by_exception() const
			{
				return false;
			}

			//! Returns the zero-based index of the alternative that is currently held by the variant.
			constexpr std::size_t index() const
			{
				return (!valueless_by_exception()) ? this->_index : variant_npos;
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
			inline constexpr std::enable_if_t<_is_constexpr_pack<Types...>::value
											  , std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>> 
				_get_if(variant<Types...>* var_ptr)
			{
				using value_ptr_t = std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>;
				return (var_ptr != nullptr && var_ptr->index() == I) ? _get_if_constexpr<I, value_ptr_t, decltype(var_ptr->_storage)>(&var_ptr->_storage).ptr : nullptr;
			}

			template <std::size_t I, class... Types>
			inline std::enable_if_t<negation<_is_constexpr_pack<Types...>>::value
											  , std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>>
				_get_if(variant<Types...>* var_ptr)
			{
				using value_ptr_t = std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>;
				return (var_ptr != nullptr && var_ptr->index() == I) ? static_cast<value_ptr_t>(var_ptr->_storage.memory_ptr()) : nullptr;
			}
		} // namespace _detail

		//! Index-based non-throwing accessor: If 'pv' is not a null pointer and 'pv->index() == I', returns a pointer to the value stored in the variant pointed to by 'pv'. Otherwise, returns a null pointer value.
		template <std::size_t I, class... Types>
		inline constexpr std::add_pointer_t<variant_alternative_t<I, variant<Types...>>> get_if(variant<Types...>* pv)
		{
			return _detail::_get_if<I>(pv);
		}

		//! Index-based non-throwing accessor: If 'pv' is not a null pointer and 'pv->index() == I', returns a const pointer to the value stored in the variant pointed to by 'pv'. Otherwise, returns a null pointer value.
		template <std::size_t I, class... Types>
		inline constexpr std::add_pointer_t<const variant_alternative_t<I, variant<Types...>>> get_if(const variant<Types...>* pv)
		{
			return _detail::_get_if<I>(const_cast<variant<Types...>*>(pv));
		}

		//! Type-based non-throwing accessor: Equivalent to 'get_if<I,Types...>' with 'I' being the zero-based index of 'T' in 'Types...'.
		template <class T, class... Types>
		inline constexpr std::add_pointer_t<T> get_if(variant<Types...>* pv)
		{
			return _detail::_get_if<_detail::_alternative_index_v<T, Types...>>(pv);
		}

		//! Type-based non-throwing accessor: Equivalent to 'get_if<I,Types...>' with 'I' being the zero-based index of 'T' in 'Types...'.
		template <class T, class... Types>
		inline constexpr std::add_pointer_t<const T> get_if(const variant<Types...>* pv)
		{
			return _detail::_get_if<_detail::_alternative_index_v<T, Types...>>(const_cast<variant<Types...>*>(pv));
		}

		//! Index-based value accessor: If 'v.index() == I', returns a reference to the value stored in 'v'. Otherwise, throws 'bad_variant_access'.
		template<std::size_t I, class... Types>
		inline _NONAME_CONSTEXPR variant_alternative_t<I, variant<Types...>>& get(variant<Types...>& v)
		{
			using result_ptr_t = std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>;
			result_ptr_t ptr = get_if<I, Types...>(&v);
			return *((ptr != nullptr) ? ptr : (throw bad_variant_access("bad_variant_access"), ptr));
		}

		//! Index-based value accessor: If 'v.index() == I', returns a reference to the value stored in 'v'. Otherwise, throws 'bad_variant_access'.
		template<std::size_t I, class... Types, _NONAME_REQUIRES(_detail::_is_constexpr_pack<Types...>)>
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
		template<std::size_t I, class... Types, _NONAME_REQUIRES(negation<_detail::_is_constexpr_pack<Types...>>)>
		inline variant_alternative_t<I, variant<Types...>> const& get(const variant<Types...>& v)
		{
			using result_ptr_t = std::add_pointer_t<const variant_alternative_t<I, variant<Types...>>>;
			result_ptr_t ptr = get_if<I, Types...>(&v);
			return *((ptr != nullptr) ? ptr : (throw bad_variant_access("bad_variant_access"), ptr));
		}

		//! Index-based value accessor: If 'v.index() == I', returns a reference to the value stored in 'v'. Otherwise, throws 'bad_variant_access'.
		template<std::size_t I, class... Types>
		inline _NONAME_CONSTEXPR variant_alternative_t<I, variant<Types...>>&& get(variant<Types...>&& v)
		{
			using result_ptr_t = std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>;
			result_ptr_t ptr = get_if<I, Types...>(&v);
			return std::move(*((ptr != nullptr) ? ptr : (throw bad_variant_access("bad_variant_access"), ptr)));
		}

		//! Type-based value accessor: If v holds the alternative T, returns a reference to the value stored in v. 
		template <class T, class... Types>
		inline _NONAME_CONSTEXPR T& get(variant<Types...>& v)
		{
			return get<_detail::_alternative_index_v<T, Types...>>(v);
		}

		//! Type-based value accessor: If v holds the alternative T, returns a reference to the value stored in v. 
		template <class T, class... Types, _NONAME_REQUIRES(_detail::_is_constexpr_pack<Types...>)>
		inline constexpr const T& get(const variant<Types...>& v)
		{
			return get<_detail::_alternative_index_v<T, Types...>>(v);
		}

		//! Type-based value accessor: If v holds the alternative T, returns a reference to the value stored in v. 
		template <class T, class... Types, _NONAME_REQUIRES(negation<_detail::_is_constexpr_pack<Types...>>)>
		inline const T& get(const variant<Types...>& v)
		{
			return get<_detail::_alternative_index_v<T, Types...>>(v);
		}

		//! Type-based value accessor: If v holds the alternative T, returns a reference to the value stored in v. 
		template <class T, class... Types>
		inline _NONAME_CONSTEXPR T&& get(variant<Types...>&& v)
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
				using Indices = std::make_index_sequence<variant_size_v<std::decay<Variant>::type>>;

				return _make_callarray_impl<Visitor, Variant>(Indices());
			}
		} // namespace _detail

		template <class Visitor, class Variant>
		static inline constexpr decltype(auto) visit(Visitor&& visitor, Variant&& var)
		{
			if (var.valueless_by_exception()) throw bad_variant_access("bad_variant_access");
			const std::size_t currentIndex = var.index();

			constexpr auto call_array = _detail::_make_callarray<Visitor&&, Variant>();
			return call_array[currentIndex](std::forward<Visitor>(visitor), std::forward<Variant>(var));
		}
	}
}

#pragma warning( pop )

#undef _NONAME_CONSTEXPR
#undef _NONAME_REQUIRES
