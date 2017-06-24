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

#ifndef CATS_CORECAT_CHARSET_UTF8CHARSET_HPP
#define CATS_CORECAT_CHARSET_UTF8CHARSET_HPP


#include <cstddef>

#include "Charset.hpp"


namespace Cats {
namespace Corecat {
namespace Charset {

template <typename T = char>
struct UTF8Charset : public Charset<T> {
    
    static const char* getName() noexcept { return "UTF-8"; }
    
    static char32_t decode(const T*& p, const T* q)  {
        
        std::ptrdiff_t size = q - p;
        if(!size) return 0xFFFFFFFF;
        char32_t a = static_cast<unsigned char>(*p++);
        if(a <= 0x7F) return a;
        else if(a <= 0xBF) return 0xFFFD;
        else if(a <= 0xDF) {
            
            if(size < 2) return 0xFFFFFFFF;
            char32_t b = static_cast<unsigned char>(*p++); if((b & 0xC0) != 0x80) return 0xFFFD;
            return ((a & 0x1F) << 6) | (b & 0x3F);
            
        } else if(a <= 0xEF) {
            
            if(size < 3) return 0xFFFFFFFF;
            char32_t b = static_cast<unsigned char>(*p++); if((b & 0xC0) != 0x80) return 0xFFFD;
            char32_t c = static_cast<unsigned char>(*p++); if((c & 0xC0) != 0x80) return 0xFFFD;
            return ((a & 0x0F) << 12) | ((b & 0x3F) << 6) | (c & 0x3F);
            
        } else if(a <= 0xF7) {
            
            if(size < 4) return 0xFFFFFFFF;
            char32_t b = static_cast<unsigned char>(*p++); if((b & 0xC0) != 0x80) return 0xFFFD;
            char32_t c = static_cast<unsigned char>(*p++); if((c & 0xC0) != 0x80) return 0xFFFD;
            char32_t d = static_cast<unsigned char>(*p++); if((d & 0xC0) != 0x80) return 0xFFFD;
            return ((a & 0x07) << 18) | ((b & 0x3F) << 12) | ((c & 0x3F) << 6) | (d & 0x3F);
            
        } else return 0xFFFD;
        
    }
    static bool encode(T*& p, T* q, char32_t codepoint) {
        
        std::ptrdiff_t size = q - p;
        if(codepoint <= 0x007F) {
            
            // 0xxxxxxx
            if(!size) return false;
            *p++ = codepoint;
            return true;
            
        } else if(codepoint <= 0x07FF) {
            
            // 110xxxxx 10xxxxxx
            if(size < 2) return false;
            *p++ = 0xC0 | (codepoint >> 6);
            *p++ = 0x80 | (codepoint & 0x3F);
            return true;
            
        } else if(codepoint <= 0xFFFF) {
            
            // 1110xxxx 10xxxxxx 10xxxxxx
            if(size < 3) return false;
            if(codepoint - 0xD800 < 0x0800) { *p++ = 0xEF; *p++ = 0xBF; *p++ = 0xBD; }
            else {
                
                *p++ = 0xE0 | (codepoint >> 12);
                *p++ = 0x80 | ((codepoint >> 6) & 0x3F);
                *p++ = 0x80 | (codepoint & 0x3F);
                
            }
            return true;
            
        } else if(codepoint <= 0x10FFFF) {
            
            // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            if(size < 4) return false;
            *p++ = 0xF0 | (codepoint >> 18);
            *p++ = 0x80 | ((codepoint >> 12) & 0x3F);
            *p++ = 0x80 | ((codepoint >> 6) & 0x3F);
            *p++ = 0x80 | (codepoint & 0x3F);
            return true;
            
        } else {
            
            // Invalid code point
            *p++ = 0xEF; *p++ = 0xBF; *p++ = 0xBD;
            return true;
            
        }
        
    }
    
};

}
}
}


#endif
