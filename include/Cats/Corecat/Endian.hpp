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

#ifndef CATS_CORECAT_ENDIAN_HPP
#define CATS_CORECAT_ENDIAN_HPP


#include <cstdlib>

#include <type_traits>
#include <utility>


namespace Cats {
namespace Corecat {
namespace Endian {

namespace Impl {

template <typename T, typename = void>
struct SwapBytes;
template <typename T>
struct SwapBytes<T, typename std::enable_if<sizeof(T) == 1>::type> {
    
    static T swapBytes(T t) { return t; }
    
};
template <typename T>
struct SwapBytes<T, typename std::enable_if<sizeof(T) == 2>::type> {
    
    static T swapBytes(T t) {
        
        auto p = reinterpret_cast<char*>(&t);
        std::swap(p[0], p[1]);
        return t;
        
    }
    
};
template <typename T>
struct SwapBytes<T, typename std::enable_if<sizeof(T) == 4>::type> {
    
    static T swapBytes(T t) {
        
        auto p = reinterpret_cast<char*>(&t);
        std::swap(p[0], p[3]);
        std::swap(p[1], p[2]);
        return t;
        
    }
    
};
template <typename T>
struct SwapBytes<T, typename std::enable_if<sizeof(T) == 8>::type> {
    
    static T swapBytes(T t) {
        
        auto p = reinterpret_cast<char*>(&t);
        std::swap(p[0], p[7]);
        std::swap(p[1], p[6]);
        std::swap(p[2], p[5]);
        std::swap(p[3], p[4]);
        return t;
        
    }
    
};

}

template <typename T>
inline T swapBytes(T t) { return Impl::SwapBytes<T>::swapBytes(t); }

}
}
}


#endif
