# noname_tools [![Build Status](https://github.com/w1th0utnam3/noname_tools/workflows/Build%20and%20test%20master/badge.svg)](https://github.com/w1th0utnam3/noname_tools/actions?query=workflow%3A%22Build+and+test+master%22)

This collection of C++ header files contains algorithms and small helper classes that accumulated 
from several small coding projects of mine. 
The code of this project that is written by contributors of this repository is licensed under 
the [MIT License](https://github.com/w1th0utnam3/noname_tools/blob/master/LICENSE). 

## Usage

To use the tools just copy the files from the `src/noname_tools` folder into your project. 
Including the `tools` file automatically includes all other headers. 
The repository contains a project with test-cases for the tools. 
It's based on the [Catch](https://github.com/philsquared/Catch) header-only unit-test framework 
which is licensed under the [Boost Software License](https://github.com/philsquared/Catch/blob/master/LICENSE_1_0.txt). 
Catch is included as a submodule. 
Use `git clone --recurse-submodules` to check out this repository including its submodules. 
Compilation is tested using [GitHub Actions](https://github.com/w1th0utnam3/noname_tools/actions?query=workflow%3A%22Build+and+test+master%22) 
with VS2019, GCC 7.x, 8.x and 9.x, Clang 6.x, 8.x and 9.x in C++14 and C++17 mode of all compilers.   

To use the headers from this repo, you have to turn on C++14/C++17 support if this isn't your compiler's default setting. 
Note that you have to explicitly define the macro `NONAME_CPP14` before including any `noname_tools` header to 
make sure that no C++17 feature is used by accident in the implementation.

## Todos

- Update this file with all implemented features
- Add missing unit-tests
- Additional features:
  - string split iterator
  - replace explicit container usages with iterators
- Make `typelist_for_each` and `tuple_for_each` constexpr where possible

## Included tools

At the moment `noname_tools` contains the following headers:

- [`algorithm_tools.h`](#algorithm_toolsh) - Additional algorithms not present in `<algorithm>`
- [`file_tools.h`](#file_toolsh) - Helper methods to read files to strings
- `functional_tools.h` - Helpers related to callables (`apply_index_sequence`, `callable_container`...)
- [`range_tools.h`](#range_toolsh) - Basic `iterator_range` type
- `rtctmap_tools` - Functions to construct mappings for any type that can be used as a NTTP from a run-time argument to its corresponding value from a compile compile-time specified list of possible values
- [`string_tools.h`](#string_toolsh) - String truncate, split...
- [`tuple_tools.h`](#tuple_toolsh) - Operations on `std::tuple`
- `typelist_tools.h` - Type and associated helpers to store and pass around lists of types, more lightweight alternative to using `std::tuple` in template metaprogramming
- [`typetraits_tool.h`](#typetraits_toolssh) - C++20, C++17 and _Library fundamentals v2_ helpers for C++14 (`void_t`, `is_detected`,...)
- [`utility_tools.h`](#utility_toolsh) - helper types
- [`vector_tools.h`](#vector_toolsh) - Operations on `std::vector`

Below is a list of all types and functions from these headers. All declarations are in the `noname::tools` namespace.

### algorithm_tools.h

```c++
//! Returns the first element in the specified range that is unequal to its predecessor, uses not-equal (!=) operator for comparison
InputIt find_unequal_successor(InputIt first, InputIt last);
//! Returns the first element in the specified range that is unequal to its predecessor, uses p to compare two elements for inequality
InputIt find_unequal_successor(InputIt first, InputIt last, BinaryPredicate p);

//! Applies the given function object to every element and its successor, returns copy/move of functor
Func for_each_and_successor(InputIt first, InputIt last, Func f);

//! Divides a range in n (nearly) equal sized subranges and writes every subrange's begin- and end-iterator into dest without duplicates (i.e. dest will have n+1 entries)
void n_subranges(InputIt first, InputIt last, OutputIt dest, std::size_t n);

//! Copies the elements from the specified range to dest in such a way that all groups of consecutive equal objects are omitted, uses equal operator for comparison
OutputIt strict_unique_copy(InputIt first, InputIt last, OutputIt dest);
//! Copies the elements from the specified range to dest in such a way that all groups of consecutive equal objects are omitted, uses p to compare elements for equality
OutputIt strict_unique_copy(InputIt first, InputIt last, OutputIt dest, BinaryPredicate p);
```

### file_tools.h

```c++
//! Reads all lines from the specified file to a vector
inline std::vector<std::string> read_all_lines(const std::string& file_path);

//! Reads the specified number of lines from a file or reads the whole file if number of lines is zero
inline std::vector<std::string> read_lines(const std::string& file_path, size_t number_of_lines = 0);
```

### range_tools.h

```c++
//! Range object with begin() and end() methods to use range-based for loops with any pair of iterators, sentinel version (begin and end may be of different type)
class iterator_range<begin_t, end_t = void>
//! Range object with begin() and end() methods to use range-based for loops with any pair of iterators
class iterator_range<iterator_t>

//! Creates an iterator_range object, deducing the target type from the types of arguments
constexpr iterator_range<typename std::decay<begin_t>::type, typename std::decay<end_t>::type> make_range(begin_t&& begin, end_t&& end);
```

### string_tools.h

```c++
//! Truncates a string at the first occurrence of the specified character or returns the full string if the character was not found
StringT truncate_string(const StringT& str, CharT ch);

//! Returns a vector of substrings of the original string, split at every occurrence of the specified character
std::vector<StringT> split_string(const StringT& str, CharT ch);
```

### tuple_tools.h

```c++
//! Calls a function for each element of a tuple in order and returns the function
F tuple_for_each(Tuple&& tuple, F f);
```

### typetraits_tools.h

Some type traits and helper types from the C++17 draft for use in C++14. Also includes the [detection idiom](http://en.cppreference.com/w/cpp/experimental/is_detected) alias templates from Library fundamentals v2.
```c++
//! Provides the member typedef type that names T (i.e., the identity transformation).
template< class T >
struct type_identity;

//! Utility metafunction that maps a sequence of any types to the type void
template <typename... T>
using void_t = ...;

//! Helper alias template for std::integral_constant for the common case where T is bool.
template <bool B>
using bool_constant = ...;

//! Forms the logical negation of the type trait B.
template<class B>
struct negation : ...;
//! Forms the logical conjunction of the type traits B..., effectively performing a logical AND on the sequence of traits.
template<class B1, class... Bn>
struct conjunction<B1, Bn...> : ...;
//! Forms the logical disjunction of the type traits B..., effectively performing a logical or on the sequence of traits.
template<class B1, class... Bn>
struct disjunction<B1, Bn...> : ...;

//! Class type used by detected_t to indicate detection failure. 
struct nonesuch;
//! Alias for std::true_type if the template-id Op<Args...> is valid; otherwise it is an alias for std::false_type. 
template <template<class...> class Op, class... Args>
using is_detected = ...;
//! Alias for Op<Args...> if that template-id is valid; otherwise it is an alias for the class nonesuch. 
template <template<class...> class Op, class... Args>
using detected_t = ...;
//! If the template-id Op<Args...> is valid, then value_t is an alias for std::true_type, and type is an alias for Op<Args...>; Otherwise, value_t is an alias for std::false_type and type is an alias for Default.
template <class Default, template<class...> class Op, class... Args>
using detected_or = ...;
//! Checks whether detected_t<Op, Args...> is Expected.  
template <class Expected, template<class...> class Op, class... Args>
using is_detected_exact = ...;
//! Checks whether detected_t<Op, Args...> is convertible to To.
template <class To, template<class...> class Op, class... Args>
using is_detected_convertible = ...;

//! Checks if the supplied type is referenceable, i.e. whether T& is a well-formed type
template<class T>
using is_referenceable = ...;

//! Checks if the expressions swap(std::declval<T>(), std::declval<U>()) and swap(std::declval<U>(), std::declval<T>()) are both well formed after "using std::swap"
template<class T, class U>
using is_swappable_with = ...;

//! Checks if a type is referenceable and whether std::is_swappable_with<T&, T&>::value is true
template<class T>
using is_swappable = ...;

//! Combines std::remove_cv and std::remove_reference
template<class T>
struct remove_cvref;
//! Helper for remove_cvref, defined as remove_cvref::type.
template<class T>
using remove_cvref_t = typename remove_cvref<T>::type;
```

### utility_tools.h
```c++
//! Alias for the 'I'-th element of 'Ts'.
template <std::size_t I, typename ...Ts>
struct nth_element : ...;
//! Returns the index of the first occurrence of 'T' in 'Ts...' or element_not_found.
template <typename T, typename... Ts>
struct element_index : ...;
//! Counts the number of occurrences of 'T' in 'Ts...'.
template<typename T, typename... Ts>
struct count_element : ...;
//! Checks whether every element occurs only once in 'Ts'.
template <typename... Ts>
struct unique_elements
```

### vector_tools.h

```c++
//! Returns a sorted vector constructed from the supplied initializer list
std::vector<T> sorted_vector(std::initializer_list<T> in);
//! Sorts the supplied r-value vector and returns it
std::vector<T> sorted_vector(std::vector<T>&& vector);
//! Initializes a vector by moving all supplied elements into it
std::vector<...> move_construct_vector(Ts&&... elements);
```
