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

#ifndef CATS_CORECAT_TEXT_CHARSET_UTF16CHARSET_HPP
#define CATS_CORECAT_TEXT_CHARSET_UTF16CHARSET_HPP


#include "Charset.hpp"


namespace Cats {
namespace Corecat {
inline namespace Text {

template <typename T = char16_t>
struct UTF16Charset : public Charset<T> {
    
    static constexpr const char* NAME = "UTF-16";
    static constexpr std::size_t MAX_CODE_UNIT = 8;
    
    static char32_t decode(const T*& p, const T* q) {
        
        std::ptrdiff_t size = q - p;
        if(size < 1) return 0xFFFFFFFF;
        char32_t h = char16_t(*p);
        if(h - 0xD800 >= 0x0800) { ++p; return h; }
        else if(h <= 0xDBFF) {
            
            if(size < 2) return 0xFFFFFFFF;
            ++p;
            char32_t l = char16_t(*p++);
            if(l - 0xDC00 < 0x0400) return 0x10000 + (((h & 0x3FF) << 10) | (l & 0x3FF));
            else return 0xFFFD;
            
        } else { ++p; return 0xFFFD; }
        
    }
    static bool encode(T*& p, T* q, char32_t codepoint) {
        
        std::ptrdiff_t size = q - p;
        if(codepoint <= 0xFFFF) {
            
            if(size < 1) return false;
            if(codepoint - 0xD800 >= 0x0800) *p++ = char16_t(codepoint);
            else *p++ = 0xFFFD;
            return true;
            
        } else if(codepoint <= 0x10FFFF) {
            
            if(size < 2) return false;
            *p++ = T(0xD800 | ((codepoint - 0x10000) >> 10));
            *p++ = T(0xDC00 | ((codepoint - 0x10000) & 0x3FF));
            return true;
            
        } else { *p++ = 0xFFFD; return true; }
        
    }
    
};

}
}
}


#endif
