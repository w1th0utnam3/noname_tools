# noname_tools

This collection of C++ header files contains algorithms and small helper classes that I wrote
during several small coding projects I worked on. To use the tools just copy the files from
the `src/noname_tools` folder into your project. Including the `tools` file automatically includes
all other headers. The repository contains a project with test-cases for the tools. It's based on 
the [Catch](https://github.com/philsquared/Catch) header-only unit-test framework which is
licensed under the [Boost Software License](https://github.com/philsquared/Catch/blob/master/LICENSE_1_0.txt)
and the required header is included in the project. Compilation was tested with VS 2015 Update 2
and GCC 6.1.0. You probably have to turn on C++14 support if this isn't your compiler's default setting.
The code of this project that is written by contributors of this repository is licensed under the 
[MIT License](https://github.com/w1th0utnam3/noname_tools/blob/master/LICENSE). 

## Todos

- Try to compile with C++11 and clang
- Add missing unit-tests
- Make more awesome and useful tools

## Included tools

At the moment `noname_tools` contains the following headers:

- `algorithm_tools.h`
- `file_tools.h`
- `range_tools.h`
- `string_tools.h`
- `vector_tools.h`

Below is a list of all methods from these headers. All declarations are in the `noname::tools` namespace.

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

### vector_tools.h

```c++
//! Returns a sorted vector constructed from the supplied initializer list
std::vector<T> sorted_vector(std::initializer_list<T> in);
//! Sorts the supplied r-value vector and returns it
std::vector<T> sorted_vector(std::vector<T>&& vector);
```
