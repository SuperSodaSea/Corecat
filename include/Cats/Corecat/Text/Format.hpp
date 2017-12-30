/*
 *
 * MIT License
 *
 * Copyright (c) 2016-2017 The Cats Project
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

#ifndef CATS_CORECAT_TEXT_FORMAT_HPP
#define CATS_CORECAT_TEXT_FORMAT_HPP


#include <cstddef>

#include <array>
#include <type_traits>

#include "String.hpp"
#include "../Util/Sequence.hpp"


namespace Cats {
namespace Corecat {
namespace Text {

namespace Impl {

using namespace Util::Sequence;

template <typename T>
struct DecimalDigit {
    
    static constexpr T get(int x) { return '0' + (x % 2 ? (x / 2) % 10 : (x / 2) / 10); }
    
};
template <typename T>
using DecimalDigitTable = SequenceTable<MapperSequence<DecimalDigit<T>, IndexSequence<int, 0, 199>>>;

template <typename T>
inline void toStringMiddle2(std::uint8_t x, T*& p) {
    
    *reinterpret_cast<std::array<T, 2>*>(p) = reinterpret_cast<const std::array<T, 2>*>(DecimalDigitTable<T>::TABLE)[x], p += 2;
    
}
template <typename T>
inline void toStringBegin2(std::uint8_t x, T*& p) {
    
    if(x < 10) *p++ = '0' + x;
    else toStringMiddle2(x, p);
    
}
template <typename T>
inline void toStringMiddle4(std::uint16_t x, T*& p) {
    
    std::uint8_t a = x / 100, b = x % 100;
    toStringMiddle2(a, p); toStringMiddle2(b, p);
    
}
template <typename T>
inline void toStringBegin4(std::uint16_t x, T*& p) {
    
    if(x < 100) toStringBegin2(x, p);
    else { std::uint8_t a = x / 100, b = x % 100; toStringBegin2(a, p), toStringMiddle2(b, p); }
    
}
template <typename T>
inline void toStringMiddle8(std::uint32_t x, T*& p) {
    
    std::uint16_t a = x / 10000, b = x % 10000;
    toStringMiddle4(a, p), toStringMiddle4(b, p);
    
}
template <typename T>
inline void toStringBegin8(std::uint32_t x, T*& p) {
    
    if(x < 10000) toStringBegin4(x, p);
    else { std::uint16_t a = x / 10000, b = x % 10000; toStringBegin4(a, p), toStringMiddle4(b, p); }
    
}
template <typename T>
inline void toStringMiddle16(std::uint64_t x, T*& p) {
    
    std::uint32_t a = x / 100000000, b = x % 100000000;
    toStringMiddle8(a, p), toStringMiddle8(b, p);
    
}

}

template <typename T>
inline T* u32ToString(std::uint32_t x, T* p) {
    
    if(x < 100000000) Impl::toStringBegin8(x, p);
    else {
        std::uint32_t a = x / 100000000, b = x % 100000000;
        Impl::toStringBegin2(a, p), Impl::toStringMiddle8(b, p);
    }
    return p;
    
}
template <typename T>
inline T* i32ToString(std::int32_t x, T* p) {
    
    std::uint32_t t;
    if(x >= 0) t = x;
    else *p++ = '-', t = -std::uint32_t(x);
    return u32ToString(t, p);
    
}
template <typename T>
inline T* u64ToString(std::uint64_t x, T* p) {
    
    if(x < 4294967296) return u32ToString(x, p);
    else if(x < 10000000000000000) {
        std::uint32_t a = x / 100000000, b = x % 100000000;
        Impl::toStringBegin8(a, p), Impl::toStringMiddle8(b, p);
    } else {
        std::uint64_t a = x / 10000000000000000, b = x % 10000000000000000;
        Impl::toStringBegin4(a, p), Impl::toStringMiddle16(b, p);
    }
    return p;
    
}
template <typename T>
inline T* i64ToString(std::int64_t x, T* p) {
    
    std::uint64_t t;
    if(x >= 0) t = x;
    else *p++ = '-', t = -std::uint64_t(x);
    return u64ToString(t, p);
    
}


template <typename C, typename T>
String<C> toString(T t, typename std::enable_if<std::is_unsigned<T>::value && sizeof(T) <= 4>::type* = 0) {
    
    String<C> s;
    s.setLength(u32ToString(t, s.getData()) - s.getData());
    return s;
    
}
template <typename C, typename T>
String<C> toString(T t, typename std::enable_if<std::is_signed<T>::value && sizeof(T) <= 4>::type* = 0) {
    
    String<C> s;
    s.setLength(i32ToString(t, s.getData()) - s.getData());
    return s;
    
}
template <typename C, typename T>
String<C> toString(T t, typename std::enable_if<(std::is_unsigned<T>::value && sizeof(T) > 4 && sizeof(T) <= 8)>::type* = 0) {
    
    String<C> s;
    s.setLength(u64ToString(t, s.getData()) - s.getData());
    return s;
    
}
template <typename C, typename T>
String<C> toString(T t, typename std::enable_if<(std::is_signed<T>::value && sizeof(T) > 4 && sizeof(T) <= 8)>::type* = 0) {
    
    String<C> s;
    s.setLength(i64ToString(t, s.getData()) - s.getData());
    return s;
    
}

template <typename T>
String8 toString8(T t) { return toString<Charset::UTF8Charset<>>(t); }
template <typename T>
String16 toString16(T t) { return toString<Charset::UTF16Charset<>>(t); }
template <typename T>
String32 toString32(T t) { return toString<Charset::UTF32Charset<>>(t); }

}
}
}


#endif
