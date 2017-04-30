# noname_tools

This collection of C++ header files contains algorithms and small helper classes that I wrote during several small coding projects I worked on. To use the tools just copy the files from the `src/noname_tools` folder into your project. Including the `tools` file automatically includes all other headers. The repository contains a project with test-cases for the tools. It's based on 
the [Catch](https://github.com/philsquared/Catch) header-only unit-test framework which is licensed under the [Boost Software License](https://github.com/philsquared/Catch/blob/master/LICENSE_1_0.txt) and it is included as a submodule. Compilation was tested with VS"15" Preview Update 4 and GCC 6.1.0. You probably have to turn on C++14 support if this isn't your compiler's default setting. The code of this project that is written by contributors of this repository is licensed under the [MIT License](https://github.com/w1th0utnam3/noname_tools/blob/master/LICENSE). 

## Todos

- Finish variant implementation
- Try to compile with C++11 and clang
- Add missing unit-tests
- Make more awesome and useful tools and port some C++17 features, i.e.
  - C++14 std::string_view, std::any
  - string split iterator
  - replace explicit container usages with iterators

## Included tools

At the moment `noname_tools` contains the following headers:

- [`algorithm_tools.h`](#algorithm_toolsh) - Additional algorithms not present in `<algorithm>`
- [`file_tools.h`](#file_toolsh) - Helper methods to read files to strings
- [`optional_tools.h`](#optional_toolsh) - C++17 `std::optional` for C++14, depends on `type_traits.h` and `utility_tools.h`
- [`range_tools.h`](#range_toolsh) - Basic `iterator_range` type
- [`string_tools.h`](#string_toolsh) - String truncate, split...
- [`tuple_tools.h`](#tuple_toolsh) - Operations on `std::tuple`
- [`type_traits.h`](#type_traitsh) - C++17 and Library fundamentals v2 helpers for C++14 (`void_t`, `is_detected`,...)
- [`utility_tools.h`](#utility_toolsh) - helper types for `optional` (possibly also for `any` and `variant` later)
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

### optional_tools.h

Custom implementation of [std::optional](http://en.cppreference.com/w/cpp/utility/optional) from the C++17 draft for use in C++14. The interface is identical to the reference except for the fact that the comparison operators and std::hash specialization are not yet implemented. All constructors, assignments and access operations are implemented. Unit tested to conform to the reference documentation.
```c++
//! The class template optional manages an optional contained value, i.e. a value that may or may not be present.
class optional<T>;

//! Creates an optional object from value.
template<class T>
constexpr optional<std::decay_t<T>> make_optional(T&& value)
//! Creates an optional object constructed in-place from args....
template<class T, class... Args>
constexpr optional<T> make_optional(Args&&... args)
//! Creates an optional object constructed in-place from il and args....
template<class T, class U, class... Args>
constexpr optional<T> make_optional(std::initializer_list<U> il, Args&&... args)

//! Defines a type of object to be thrown by optional::value when accessing an optional object that does not contain a value.
class bad_optional_access : public std::logic_error;

//! nullopt_t is an empty class type used to indicate optional type with uninitialized state.
struct nullopt_t;

//! Disambiguation tag to construct an optional in-place. Actually calling any of the in_place functions results in undefined behavior.
in_place_tag in_place();
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
struct unique_types
```

### vector_tools.h

```c++
//! Returns a sorted vector constructed from the supplied initializer list
std::vector<T> sorted_vector(std::initializer_list<T> in);
//! Sorts the supplied r-value vector and returns it
std::vector<T> sorted_vector(std::vector<T>&& vector);
```
