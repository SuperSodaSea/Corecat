/*
 *
 * MIT License
 *
 * Copyright (c) 2016 The Cats Project
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

#ifndef CATS_CORECAT_STRING_HPP
#define CATS_CORECAT_STRING_HPP


#include <cstdlib>
#include <cstring>

#include <iostream>


namespace Cats {
namespace Corecat {

class StringView {
    
private:
    
    const char* data;
    std::size_t length;
    
public:
    
    StringView() = default;
    StringView(const char* data_) : data(data_), length(std::strlen(data_)) {}
    StringView(const char* data_, std::size_t length_) : data(data_), length(length_) {}
    StringView(const StringView& src) = default;
    ~StringView() = default;
    
    StringView& operator =(const StringView& src) = default;
    bool operator ==(StringView sv) const noexcept {
        
        if(length != sv.length) return false;
        const char* p = data;
        const char* q = sv.data;
        for(const char* end = data + length; p != end; ++p, ++q) if(*p != *q) return false;
        return true;
        
    }
    bool operator !=(StringView sv) const noexcept { return !(*this == sv); }
    
    const char* getData() const noexcept { return data; }
    std::size_t getLength() const noexcept { return length; }
    
};

inline std::ostream& operator <<(std::ostream& stream, StringView sv) {
    
    stream.write(sv.getData(), sv.getLength());
    return stream;
    
}

}
}


#endif
