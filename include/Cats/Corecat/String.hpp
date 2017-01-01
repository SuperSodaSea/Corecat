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

#ifndef CATS_CORECAT_STRING_HPP
#define CATS_CORECAT_STRING_HPP


#include <cstdlib>

#include <iostream>
#include <string>

#include "Encoding.hpp"


namespace Cats {
namespace Corecat {

template <typename T, template<typename> class E>
class StringViewBase {
    
public:
    
    using CharType = T;
    using EncodingType = E<T>;
    
private:
    
    const T* data;
    std::size_t length;
    
public:
    
    StringViewBase() = default;
    StringViewBase(const T* data_) : data(data_), length(std::char_traits<T>::length(data_)) {}
    StringViewBase(const T* data_, std::size_t length_) : data(data_), length(length_) {}
    StringViewBase(const StringViewBase& src) = default;
    ~StringViewBase() = default;
    
    StringViewBase& operator =(const StringViewBase& src) = default;
    const T& operator [](std::size_t index) const noexcept { return data[index]; }
    bool operator ==(StringViewBase sv) const noexcept {
        
        if(length != sv.length) return false;
        const T* p = data;
        const T* q = sv.data;
        for(const T* end = data + length; p != end; ++p, ++q) if(*p != *q) return false;
        return true;
        
    }
    bool operator !=(StringViewBase sv) const noexcept { return !(*this == sv); }
    
    const T* getData() const noexcept { return data; }
    void setData(const T* data_) noexcept { data = data_; }
    std::size_t getLength() const noexcept { return length; }
    void setLength(std::size_t length_) noexcept { length = length_; }
    
};

template <typename T, template<typename> class E>
inline std::basic_ostream<T>& operator <<(std::basic_ostream<T>& stream, StringViewBase<T, E> sv) {
    
    stream.write(sv.getData(), sv.getLength());
    return stream;
    
}

using StringView = StringViewBase<char, Encoding::UTF8>;

}
}


#endif
