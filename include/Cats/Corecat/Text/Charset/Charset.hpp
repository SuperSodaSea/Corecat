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

#ifndef CATS_CORECAT_TEXT_CHARSET_CHARSET_HPP
#define CATS_CORECAT_TEXT_CHARSET_CHARSET_HPP


#include <type_traits>


namespace Cats {
namespace Corecat {
inline namespace Text {

template <typename T>
struct Charset {
    
    using CharType = T;
    
    static const char* getName();
    
    static char32_t decode(const T*& p, const T* q);
    static bool encode(T*& p, T* q, char32_t codepoint);
    
    static int compare(const T* begin1, const T* end1, const T* begin2, const T* end2) noexcept {
        
        using U = std::make_unsigned_t<T>;
        for(auto p = begin1, q = begin2; ; ++p, ++q) {
            
            bool m1 = p == end1, m2 = q == end2;
            if(m1 || m2) return m2 - m1;
            T c1 = *p, c2 = *q;
            if(c1 != c2) return U(c1) < U(c2) ? -1 : 1;
            
        }
        
    }
    
    static std::size_t getLength(const T* str) noexcept {
        
        const T* end = str;
        while(*end != T()) ++end;
        return end - str;
        
    }
    
};

}
}
}


#endif
