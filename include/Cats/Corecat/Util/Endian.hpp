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

#ifndef CATS_CORECAT_UTIL_ENDIAN_HPP
#define CATS_CORECAT_UTIL_ENDIAN_HPP


#include <cstdlib>

#include <type_traits>
#include <utility>

#include "Byte.hpp"
#include "../System/Compiler.hpp"


namespace Cats {
namespace Corecat {
inline namespace Util {

enum class Endian {
    
    LITTLE,
    BIG,
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    NATIVE = LITTLE,
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    NATIVE = BIG,
#else
#   error Unknown endian
#endif
    
};

template <typename T>
inline std::enable_if_t<sizeof(T) == 1, T> reverseEndian(T t) { return t; }
template <typename T>
inline std::enable_if_t<std::is_integral<T>::value && sizeof(T) == 2, T> reverseEndian(T t) {
#if defined(CORECAT_COMPILER_CLANG) || defined(CORECAT_COMPILER_GCC)
    return __builtin_bswap16(t);
#elif defined(CORECAT_COMPILER_MSVC)
    return _byteswap_ushort(t);
#else
    auto p = reinterpret_cast<Byte*>(&t);
    Byte x;
    x = p[0], p[0] = p[1], p[1] = x;
    return t;
#endif
}
template <typename T>
inline std::enable_if_t<std::is_integral<T>::value && sizeof(T) == 4, T> reverseEndian(T t) {
#if defined(CORECAT_COMPILER_CLANG) || defined(CORECAT_COMPILER_GCC)
    return __builtin_bswap32(t);
#elif defined(CORECAT_COMPILER_MSVC)
    return _byteswap_ulong(t);
#else
    auto p = reinterpret_cast<Byte*>(&t);
    Byte x;
    x = p[0], p[0] = p[3], p[3] = x;
    x = p[1], p[1] = p[2], p[2] = x;
    return t;
#endif
    
}
template <typename T>
inline std::enable_if_t<std::is_integral<T>::value && sizeof(T) == 8, T> reverseEndian(T t) {
#if defined(CORECAT_COMPILER_CLANG) || defined(CORECAT_COMPILER_GCC)
    return __builtin_bswap64(t);
#elif defined(CORECAT_COMPILER_MSVC)
    return _byteswap_uint64(t);
#else
    auto p = reinterpret_cast<Byte*>(&t);
    Byte x;
    x = p[0], p[0] = p[7], p[7] = x;
    x = p[1], p[1] = p[6], p[6] = x;
    x = p[2], p[2] = p[5], p[5] = x;
    x = p[3], p[3] = p[4], p[4] = x;
    return t;
#endif
}

template <Endian E1, Endian E2, typename T>
std::enable_if_t<E1 == E2, T> convertEndian(T t) { return t; }
template <Endian E1, Endian E2, typename T>
std::enable_if_t<E1 != E2, T> convertEndian(T t) { return reverseEndian(t); }

template <typename T>
T convertNativeToLittle(T t) { return convertEndian<Endian::NATIVE, Endian::LITTLE>(t); }
template <typename T>
T convertLittleToNative(T t) { return convertEndian<Endian::LITTLE, Endian::NATIVE>(t); }
template <typename T>
T convertNativeToBig(T t) { return convertEndian<Endian::NATIVE, Endian::BIG>(t); }
template <typename T>
T convertBigToNative(T t) { return convertEndian<Endian::BIG, Endian::NATIVE>(t); }

}
}
}


#endif
