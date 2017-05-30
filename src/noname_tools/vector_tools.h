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

#include <vector>
#include <initializer_list>
#include <algorithm>

#include "typetraits_tools.h"
#include "utility_tools.h"

namespace noname
{
	namespace tools
	{
		//! Returns a sorted vector constructed from the supplied initializer list
		template <typename T>
		std::vector<T> sorted_vector(std::initializer_list<T> in)
		{
			std::vector<T> vector(in);
			std::sort(vector.begin(), vector.end());
			return vector;
		}

		//! Sorts the supplied r-value vector and returns it
		template <typename T>
		std::vector<T> sorted_vector(std::vector<T>&& vector)
		{
			std::sort(vector.begin(), vector.end());
			return vector;
		}

		namespace _detail
		{
			template <typename VecT, typename T, typename... Ts>
			struct move_construct_helper;

			template <typename VecT, typename T>
			struct move_construct_helper<VecT, T>
			{
				move_construct_helper(VecT& vector, T&& element)
				{
					vector.emplace_back(std::move(element));
				}
			};

			template <typename VecT, typename T, typename... Ts>
			struct move_construct_helper
			{
				move_construct_helper(VecT& vector, T&& element, Ts&&... remaining)
				{
					vector.emplace_back(std::move(element));
					move_construct_helper<VecT, Ts...> temp(vector, std::forward<Ts>(remaining)...);
				}
			};

			template <typename VecT, typename... Ts>
			void move_construct_helper_fun(VecT& vector, Ts&&... elements)
			{
				move_construct_helper<VecT, Ts...> temp(vector, std::forward<Ts>(elements)...);
			}
		}

		//! Initializes a vector by moving all supplied elements into it
		template <typename... Ts>
		std::vector<nth_element_t<0, Ts...>> move_construct_vector(Ts&&... elements)
		{
			static_assert(conjunction<std::is_same<Ts, nth_element_t<0, Ts...>>...>::value, "All supplied elements have to be of the same type.");
			std::vector<nth_element_t<0, Ts...>> vector;
			vector.reserve(sizeof...(elements));
			move_construct_helper_fun(vector, std::forward<Ts>(elements)...);
			return vector;
		}
	}
}
