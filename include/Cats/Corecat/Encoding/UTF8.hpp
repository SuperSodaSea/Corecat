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

#ifndef CATS_CORECAT_ENCODING_UTF8_HPP
#define CATS_CORECAT_ENCODING_UTF8_HPP


#include <cstdlib>

#include "Base.hpp"


namespace Cats {
namespace Corecat {
namespace Encoding {
    
template <typename T>
struct UTF8 : public Base<T> {
    
    static const char* getName() noexcept { return "UTF-8"; }
    
    static void encode(Stream& stream, char32_t codepoint) {
        
        if(codepoint <= 0x007F) {
            
            // 0xxxxxxx
            T data[] = {
                static_cast<T>(codepoint),
            };
            stream.write(data, 1);
            
        } else if(codepoint <= 0x07FF) {
            
            // 110xxxxx 10xxxxxx
            T data[] = {
                0xC0 | static_cast<T>(codepoint >> 6),
                0x80 | static_cast<T>(codepoint & 0x3F),
            };
            stream.write(data, 2);
            
        } else if(codepoint <= 0xFFFF) {
            
            // 1110xxxx 10xxxxxx 10xxxxxx
            T data[] = {
                0xE0 | static_cast<T>(codepoint >> 12),
                0x80 | static_cast<T>((codepoint >> 6) & 0x3F),
                0x80 | static_cast<T>(codepoint & 0x3F),
            };
            stream.write(data, 3);
            
        } else if(codepoint <= 0x10FFFF) {
            
            // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            T data[] = {
                0xF0 | static_cast<T>(codepoint >> 18),
                0x80 | static_cast<T>((codepoint >> 12) & 0x3F),
                0x80 | static_cast<T>((codepoint >> 6) & 0x3F),
                0x80 | static_cast<T>(codepoint & 0x3F),
            };
            stream.write(data, 4);
            
        }
        else {
            
            // TODO: Invalid code point
            
        }
        
    }
    
};

}
}
}


#endif
