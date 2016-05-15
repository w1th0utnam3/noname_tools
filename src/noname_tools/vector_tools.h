#pragma once

#include <vector>
#include <initializer_list>
#include <algorithm>

namespace noname {

	namespace tools {

		//! Returns a sorted vector constructed from the supplied initializer list
		template<typename T>
		std::vector<T> sorted_vector(std::initializer_list<T> in)
		{
			std::vector<T> vector(in);
			std::sort(vector.begin(), vector.end());
			return vector;
		}

		//! Sorts the supplied r-value vector and returns it
		template<typename T>
		std::vector<T> sorted_vector(std::vector<T>&& vector)
		{
			std::sort(vector.begin(), vector.end());
			return vector;
		}

	}

}
