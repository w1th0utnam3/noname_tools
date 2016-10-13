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
#include <stdexcept>

#ifdef _MSC_VER
#define _NONAME_CONSTEXPR
#else
#define _NONAME_CONSTEXPR constexpr
#endif

#define _NONAME_REQUIRES(...) typename std::enable_if<__VA_ARGS__::value, bool>::type = false

// TODO: Empty base optimization
// TODO: Add noexcept according to reference

#pragma warning( push )
#pragma warning( disable : 4396 )

namespace noname
{
	namespace tools
	{
		//! Unit type intended for use as a well-behaved empty alternative in variant.
		struct monostate {};

		//! Exception thrown when get(variant) is called with an index or type that does not match the currently active alternative or visit is called to visit a variant that is valueless_by_exception.
		class bad_variant_access : public std::exception {};

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
			// Forward declaration of _get_if for friend declaration
			template <std::size_t I, class... Types>
			inline constexpr std::enable_if_t<conjunction<std::is_trivially_destructible<Types>...>::value, std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>> _get_if(variant<Types...>* var_ptr);

			template <std::size_t I, class... Types>
			inline constexpr std::enable_if_t<negation<conjunction<std::is_trivially_destructible<Types>...>>::value, std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>> _get_if(variant<Types...>* var_ptr);

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
			};

			template <class... Types>
			struct _variant_storage
			{
				std::aligned_union_t<0, Types...> _memory;

				void* memory_ptr()
				{
					return static_cast<void*>(&_memory);
				}

				template <std::size_t I, class... Args>
				_variant_storage(std::integral_constant<std::size_t, I>, Args&&... args)
				{
					new(memory_ptr()) nth_element_t<I, Types...>(std::forward<Args>(args)...);
				}

				template <class T>
				void destroy()
				{
					(*static_cast<T*>(memory_ptr())).T::~T();
				}
			};

			template <std::size_t N, class... Types>
			struct _variant_base;

			template <class... Types>
			struct _variant_base<0, Types...>
			{
				friend constexpr std::enable_if_t<true, std::add_pointer_t<variant_alternative_t<0, variant<Types...>>>> _get_if<0, Types...>(variant<Types...>* pv);

				_variant_storage<Types...> _storage;
				std::size_t _index;

				_variant_base()
					: _storage(std::integral_constant<std::size_t, 0>(), nth_element_t<0, Types...>())
					, _index(0)
				{
				}

				template <std::size_t I, class... Args>
				explicit _variant_base(in_place_index_t<I>, Args&&... args)
					: _storage(std::integral_constant<std::size_t, I>(), std::forward<Args>(args)...)
					, _index(I)
				{
				}

				~_variant_base()
				{
					if (this->_index == 0) this->_storage.template destroy<nth_element_t<0, Types...>>();
				}
			};

			template <std::size_t N, class... Types>
			struct _variant_base : public _variant_base<N-1, Types...>
			{
				friend constexpr std::enable_if_t<true, std::add_pointer_t<variant_alternative_t<N, variant<Types...>>>> _get_if<N, Types...>(variant<Types...>* pv);

				_variant_base() = default;

				template <std::size_t I, class... Args>
				explicit _variant_base(in_place_index_t<I>, Args&&... args)
					: _variant_base<N-1, Types...>(in_place<I>, std::forward<Args>(args)...)
				{
				}

				~_variant_base()
				{
					if (this->_index == N) this->_storage.template destroy<nth_element_t<N, Types...>>();
				}

			};

			// Only trivially destructible types can be used in constexpr objects
			template <class... Types>
			using _variant_base_t = typename std::conditional<
				conjunction<std::is_trivially_destructible<Types>...>::value,
				_constexpr_variant_base<sizeof...(Types)-1, Types...>,
				_variant_base<sizeof...(Types)-1, Types...>
			>::type;

			//! Index value of the specified alternative type or variant_npos if there are several indices for the type.
			template <typename T, class... Types>
			struct alternative_index : std::conditional_t<(count_element_v<T, Types...> > 1), std::integral_constant<std::size_t, variant_npos>
																							, element_index<T, Types...>>
			{
			};

			template <typename T, class... Types>
			constexpr std::size_t alternative_index_v = alternative_index<T, Types...>::value;

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

			//! Constructs a variant with the alternative 'T_i' specified by the index 'I' and initializes the contained value with the arguments 'std::forward<Args>(args)...'.
			template <std::size_t I, class... Args, _NONAME_REQUIRES(conjunction<bool_constant<I < sizeof...(Types)>
																				 , std::is_constructible<nth_element_t<I, Types...>, Args...>>)>
			constexpr explicit variant(in_place_index_t<I>, Args&&... args)
				: _detail::_variant_base_t<Types...>(in_place<I>, std::forward<Args>(args)...)
			{
			}

			//! Converting constructor.
			template <typename T, typename T_j = best_match<T&&, Types...>
								, typename = typename std::enable_if<	std::is_constructible<T_j,T>::value 
																	&& !std::is_same<std::decay_t<T>, variant>::value>::type>
			constexpr variant(T&& t)
				: _detail::_variant_base_t<Types...>(in_place<_detail::alternative_index_v<T_j, Types...>>, std::forward<T>(t))
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
			inline constexpr std::enable_if_t<conjunction<std::is_trivially_destructible<Types>...>::value, std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>> _get_if(variant<Types...>* var_ptr)
			{
				using value_ptr_t = std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>;
				return (var_ptr != nullptr && var_ptr->index() == I) ? _get_if_constexpr<I,value_ptr_t,decltype(var_ptr->_storage)>(&var_ptr->_storage).ptr : nullptr;
			}

			template <std::size_t I, class... Types>
			inline constexpr std::enable_if_t<negation<conjunction<std::is_trivially_destructible<Types>...>>::value, std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>> _get_if(variant<Types...>* var_ptr)
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
			return _detail::_get_if<_detail::alternative_index_v<T, Types...>>(pv);
		}

		//! Type-based non-throwing accessor: Equivalent to 'get_if<I,Types...>' with 'I' being the zero-based index of 'T' in 'Types...'.
		template <class T, class... Types>
		inline constexpr std::add_pointer_t<const T> get_if(const variant<Types...>* pv)
		{
			return _detail::_get_if<_detail::alternative_index_v<T, Types...>>(const_cast<variant<Types...>*>(pv));
		}
	}
}

#pragma warning( pop )

#undef _NONAME_CONSTEXPR
#undef _NONAME_REQUIRES
