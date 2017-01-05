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

#ifndef CATS_CORECAT_CHARSET_UTF8_HPP
#define CATS_CORECAT_CHARSET_UTF8_HPP


#include <cstdlib>

#include "../Sequence.hpp"

#include "Charset.hpp"


namespace Cats {
namespace Corecat {
namespace Charset {
    
template <typename T = char>
struct UTF8 : public Charset<T> {
    
    static const char* getName() noexcept { return "UTF-8"; }
    
    static char32_t decode(Stream::Stream<T>& stream) {
        
        using namespace Sequence;
        
        struct DecodeTable {
            
            static constexpr unsigned char get(unsigned char c) {
                
                return c <= 0x7F ? 0
                    : c <= 0xBF ? 1
                    : c <= 0xDF ? 2
                    : c <= 0xEF ? 3
                    : c <= 0xF7 ? 4
                    : 5;
                
            }
            
        };
        
        T c = stream.read();
        switch(Table<Mapper<DecodeTable, Index<unsigned char, 0, 255>>>::get(static_cast<unsigned char>(c))) {
        
        case 0: return c;
        case 1: {
            
            while((stream.peek() & 0xC0) == 0x80) stream.read();
            break;
            
        }
        case 2: {
            
            char32_t codepoint = static_cast<char32_t>(c & 0x1F) << 6;
            if(((c = stream.peek()) & 0xC0) != 0x80) break;
            stream.read();
            codepoint |= c & 0x3F;
            return codepoint;
            
        }
        case 3: {
            
            char32_t codepoint = static_cast<char32_t>(c & 0x0F) << 12;
            if(((c = stream.peek()) & 0xC0) != 0x80) break;
            stream.read();
            codepoint |= static_cast<char32_t>(c & 0x3F) << 6;
            if(((c = stream.peek()) & 0xC0) != 0x80) break;
            stream.read();
            codepoint |= c & 0x3F;
            return codepoint;
            
        }
        case 4: {
            
            char32_t codepoint = static_cast<char32_t>(c & 0x07) << 18;
            if(((c = stream.peek()) & 0xC0) != 0x80) break;
            stream.read();
            codepoint |= static_cast<char32_t>(c & 0x3F) << 12;
            if(((c = stream.peek()) & 0xC0) != 0x80) break;
            stream.read();
            codepoint |= static_cast<char32_t>(c & 0x3F) << 6;
            if(((c = stream.peek()) & 0xC0) != 0x80) break;
            stream.read();
            codepoint |= c & 0x3F;
            return codepoint;
            
        }
        case 5: break;
        
        }
        return 0xFFFD;
        
    }
    static void encode(Stream::Stream<T>& stream, char32_t codepoint) {
        
        if(codepoint <= 0x007F) {
            
            // 0xxxxxxx
            T data = codepoint;
            stream.write(&data, 1);
            
        } else if(codepoint <= 0x07FF) {
            
            // 110xxxxx 10xxxxxx
            T data[] = {
                static_cast<T>(0xC0 | (codepoint >> 6)),
                static_cast<T>(0x80 | (codepoint & 0x3F)),
            };
            stream.write(data, 2);
            
        } else if(codepoint <= 0xFFFF) {
            
            // 1110xxxx 10xxxxxx 10xxxxxx
            T data[] = {
                static_cast<T>(0xE0 | (codepoint >> 12)),
                static_cast<T>(0x80 | ((codepoint >> 6) & 0x3F)),
                static_cast<T>(0x80 | (codepoint & 0x3F)),
            };
            stream.write(data, 3);
            
        } else if(codepoint <= 0x10FFFF) {
            
            // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            T data[] = {
                static_cast<T>(0xF0 | (codepoint >> 18)),
                static_cast<T>(0x80 | ((codepoint >> 12) & 0x3F)),
                static_cast<T>(0x80 | ((codepoint >> 6) & 0x3F)),
                static_cast<T>(0x80 | (codepoint & 0x3F)),
            };
            stream.write(data, 4);
            
        } else {
            
            // Invalid code point
            T data[] = { 0xEF, 0xBF, 0xBD };
            stream.write(data, 3);
            
        }
        
    }
    
};

}
}
}


#endif
