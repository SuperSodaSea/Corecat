/*
 *
 * MIT License
 *
 * Copyright (c) 2016-2018 The Cats Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef CATS_CORECAT_TEXT_CHARSET_DEFAULTCHARSET_HPP
#define CATS_CORECAT_TEXT_CHARSET_DEFAULTCHARSET_HPP


#include <type_traits>

#include "UTF8Charset.hpp"
#include "UTF16Charset.hpp"
#include "UTF32Charset.hpp"


namespace Cats {
namespace Corecat {
inline namespace Text {

namespace Impl {

template <typename T>
struct DefaultCharsetImpl;
template <>
struct DefaultCharsetImpl<char> { using Type = UTF8Charset<>; };
template <>
struct DefaultCharsetImpl<char16_t> { using Type = UTF16Charset<>; };
template <>
struct DefaultCharsetImpl<char32_t> { using Type = UTF32Charset<>; };
template <>
struct DefaultCharsetImpl<wchar_t> {
    
    static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4, "Unknown size of wchar_t");
    using Type = typename std::conditional<sizeof(wchar_t) == 2, UTF16Charset<wchar_t>, UTF32Charset<wchar_t>>::type;
    
};

}
template <typename T>
using DefaultCharset = typename Impl::DefaultCharsetImpl<T>::Type;

}
}
}


#endif
