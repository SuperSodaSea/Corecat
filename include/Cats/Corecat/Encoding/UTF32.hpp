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

#ifndef CATS_CORECAT_ENCODING_UTF32_HPP
#define CATS_CORECAT_ENCODING_UTF32_HPP


#include <cstdlib>

#include "Base.hpp"


namespace Cats {
namespace Corecat {
namespace Encoding {
    
template <typename T = char32_t>
struct UTF32 : public Base<T> {
    
    static const char* getName() noexcept { return "UTF-32"; }
    
    static char32_t decode(StreamBase<T>& stream) {
        
        char32_t codepoint = stream.read();
        return codepoint <= 0x10FFFF ? codepoint : 0xFFFD;
        
    }
    static void encode(StreamBase<T>& stream, char32_t codepoint) {
        
        T data = codepoint <= 0x10FFFF ? codepoint : 0xFFFD;
        stream.write(&data, 1);
        
    }
    
};

}
}
}


#endif
