#if false
//	MIT License
//
//	Copyright (c) 2020 Fabian Löschner
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
#endif

#define NONAME_ASSERT(x, m) ((x) ? (void)0 : (std::fprintf(stderr, "%s\n", m), std::abort()))

#ifndef NONAME_CPP14

#ifndef NONAME_CPP17
#define NONAME_CPP17
#endif

#ifndef NONAME_OPTIONAL_INCLUDE
#define NONAME_OPTIONAL_INCLUDE <optional>
#endif

#ifndef NONAME_OPTIONAL_T
#define NONAME_OPTIONAL_T std::optional
#endif

#if __cplusplus >= 201703L
#define NONAME_INLINE_VARIABLE inline
#define NONAME_INVOKE_RESULT std::invoke_result
#define NONAME_INVOKE_RESULT_T std::invoke_result_t
#endif

#if __cplusplus < 201703L && _MSC_VER >= 1912
#define NONAME_INLINE_VARIABLE inline
#endif

#if __cplusplus < 201703L && _MSC_VER >= 1911
#define NONAME_INVOKE_RESULT std::invoke_result
#define NONAME_INVOKE_RESULT_T std::invoke_result_t
#endif

#endif

#ifndef NONAME_INLINE_VARIABLE
#define NONAME_INLINE_VARIABLE
#endif

#ifndef NONAME_INVOKE_RESULT
#define NONAME_INVOKE_RESULT std::result_of
#endif

#ifndef NONAME_INVOKE_RESULT_T
#define NONAME_INVOKE_RESULT_T std::result_of_t
#endif
