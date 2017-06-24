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

#ifndef CATS_CORECAT_CHARSET_UTF32CHARSET_HPP
#define CATS_CORECAT_CHARSET_UTF32CHARSET_HPP


#include "Charset.hpp"


namespace Cats {
namespace Corecat {
namespace Charset {

template <typename T = char32_t>
struct UTF32Charset : public Charset<T> {
    
    static const char* getName() noexcept { return "UTF-32"; }
    
    static char32_t decode(const T*& p, const T* q) {
        
        if(p == q) return 0xFFFFFFFF;
        char32_t codepoint = *p++;
        if(codepoint <= 0x10FFFF && codepoint - 0xD800 >= 0x0800) return codepoint;
        else return 0xFFFD;
        
    }
    static bool encode(T*& p, T* q, char32_t codepoint) {
        
        if(p == q) return false;
        if(codepoint <= 0x10FFFF && codepoint - 0xD800 >= 0x0800) *p++ = codepoint;
        else *p++ = 0xFFFD;
        return true;
        
    }
    
};

}
}
}


#endif
