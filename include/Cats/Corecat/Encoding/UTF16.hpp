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

#ifndef CATS_CORECAT_ENCODING_UTF16_HPP
#define CATS_CORECAT_ENCODING_UTF16_HPP


#include <cstdlib>

#include "Base.hpp"


namespace Cats {
namespace Corecat {
namespace Encoding {
    
template <typename T = char16_t>
struct UTF16 : public Base<T> {
    
    static const char* getName() noexcept { return "UTF-16"; }
    
    static char32_t decode(Stream::Stream<T>& stream) {
        
        T c = stream.read();
        if((c & 0xDC00) == 0xD800) {
            
            char32_t codepoint = static_cast<char32_t>(c & 0x03FF) << 10;
            if(((c = stream.peek()) & 0xDC00) != 0xDC00) return 0xFFFD;
            stream.read();
            codepoint |= c & 0x03FF;
            return codepoint;
            
        } else {
            
            return c;
            
        }
        
    }
    static void encode(Stream::Stream<T>& stream, char32_t codepoint) {
        
        if(codepoint <= 0xFFFF) {
            
            T data;
            if((codepoint & 0xD800) == 0xD800) data = 0xFFFD;
            else data = codepoint;
            stream.write(&data, 1);
            
        } else if(codepoint <= 0x10FFFF) {
            
            T data[] = {
                
                static_cast<T>(0xD800 | (codepoint >> 10)),
                static_cast<T>(0xDC00 | (codepoint & 0x3FF)),
                
            };
            stream.write(data, 2);
            
        } else {
            
            T data = 0xFFFD;
            stream.write(&data, 1);
            
        }
            
        
    }
    
};

}
}
}


#endif
