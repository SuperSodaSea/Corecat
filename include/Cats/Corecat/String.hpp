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

#include "Charset.hpp"


namespace Cats {
namespace Corecat {

template <typename C>
class StringBase {
    
public:
    
    using CharType = typename C::CharType;
    using CharsetType = C;
    
    static constexpr std::size_t SIZE = 16;
    
private:
    
    CharType* data;
    std::size_t length;
    std::size_t capacity;
    CharType buffer[];
    
public:
    
    StringBase() : data(), length() {}
    StringBase(const CharType* data_) {}
    StringBase(const CharType* data_, std::size_t length_) {}
    StringBase(const StringBase& src) {}
    StringBase(StringBase&& src) {}
    ~StringBase() {}
    
    StringBase& operator =(const StringBase& src) {}
    
    CharType* getData() noexcept { return data; }
    const CharType* getData() const noexcept { return data; }
    std::size_t getLength() const noexcept { return length; }
    std::size_t getCapacity() const noexcept { return capacity; }
    
};

using String = StringBase<Charset::UTF8<>>;
using String16 = StringBase<Charset::UTF16<>>;
using String32 = StringBase<Charset::UTF32<>>;


template <typename C>
class StringViewBase {
    
public:
    
    using CharType = typename C::CharType;
    using CharsetType = C;
    
private:
    
    const CharType* data;
    std::size_t length;
    
public:
    
    StringViewBase() : data(""), length() {}
    StringViewBase(const CharType* data_) : data(data_), length(std::char_traits<CharType>::length(data_)) {}
    StringViewBase(const CharType* data_, std::size_t length_) : data(data_), length(length_) {}
    StringViewBase(const StringViewBase& src) = default;
    ~StringViewBase() = default;
    
    StringViewBase& operator =(const StringViewBase& src) = default;
    const CharType& operator [](std::size_t index) const noexcept { return data[index]; }
    bool operator ==(StringViewBase sv) const noexcept {
        
        if(length != sv.length) return false;
        const CharType* p = data;
        const CharType* q = sv.data;
        for(const CharType* end = data + length; p != end; ++p, ++q) if(*p != *q) return false;
        return true;
        
    }
    bool operator !=(StringViewBase sv) const noexcept { return !(*this == sv); }
    
    const CharType* getData() const noexcept { return data; }
    void setData(const CharType* data_) noexcept { data = data_; }
    std::size_t getLength() const noexcept { return length; }
    void setLength(std::size_t length_) noexcept { length = length_; }
    
};

template <typename C, typename T = typename C::CharType>
inline std::basic_ostream<T>& operator <<(std::basic_ostream<T>& stream, StringViewBase<C> sv) {
    
    stream.write(sv.getData(), sv.getLength());
    return stream;
    
}

using StringView = StringViewBase<Charset::UTF8<>>;
using StringView16 = StringViewBase<Charset::UTF16<>>;
using StringView32 = StringViewBase<Charset::UTF32<>>;

}
}


#endif
