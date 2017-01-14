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

#include <algorithm>
#include <iostream>
#include <string>

#include "Charset.hpp"


namespace Cats {
namespace Corecat {

template <typename C>
class StringViewBase;

template <typename C>
class StringBase {
    
public:
    
    using CharType = typename C::CharType;
    using CharsetType = C;
    
    using View = StringViewBase<C>;
    
    static constexpr std::size_t BUFFER_SIZE = 32;
    
private:
    
    CharType* data;
    std::size_t length;
    std::size_t capacity;
    struct { CharType data[BUFFER_SIZE]; } buffer;
    
public:
    
    StringBase() noexcept : data(buffer.data), length(), capacity(BUFFER_SIZE) { buffer.data[0] = 0; }
    StringBase(CharType ch, std::size_t count = 1) : StringBase() { append(ch, count); }
    StringBase(const CharType* data_) : StringBase() { append(data_); }
    StringBase(const CharType* data_, std::size_t length_) : StringBase() { append(data_, length_); }
    StringBase(const View& sv) : StringBase() { append(sv); }
    
    StringBase(const StringBase& src) : StringBase() { append(src); }
    StringBase(StringBase&& src) noexcept : StringBase() { swap(src); }
    ~StringBase() noexcept { if(data != buffer.data) delete[] data; }
    
    StringBase& operator =(const StringBase& src) { length = 0; append(src); return *this; }
    StringBase& operator =(StringBase&& src) noexcept { swap(src); return *this; }
    
    StringBase& operator =(const CharType* data_) { length = 0; append(data_); return *this; }
    StringBase& operator =(const View& sv) { length = 0; append(sv); return *this; }
    
    StringBase& operator +=(CharType ch) { append(ch); return *this; }
    StringBase& operator +=(const CharType* data_) { append(data_); return *this; }
    StringBase& operator +=(const StringBase& str) { append(str); return *this; }
    StringBase& operator +=(const View& sv) { append(sv); return *this; }
    
    friend StringBase operator+(const StringBase& a, CharType b) { StringBase t(a); t += b; return t; }
    friend StringBase operator+(const StringBase& a, const CharType* b) { StringBase t(a); t += b; return t; }
    friend StringBase operator+(const StringBase& a, const StringBase& b) { StringBase t(a); t += b; return t; }
    friend StringBase operator+(const StringBase& a, const View& b) { StringBase t(a); t += b; return t; }
    friend StringBase operator+(CharType a, const StringBase& b) { StringBase t(a); t += b; return t; }
    friend StringBase operator+(const CharType* a, const StringBase& b) { StringBase t(a); t += b; return t; }
    friend StringBase operator+(const View& a, const StringBase& b) { StringBase t(a); t += b; return t; }
    
    CharType* getData() noexcept { return data; }
    const CharType* getData() const noexcept { return data; }
    std::size_t getLength() const noexcept { return length; }
    std::size_t getCapacity() const noexcept { return capacity; }
    
    bool isEmpty() const noexcept { return length == 0; }
    
    void clear() noexcept { length = 0; data[0] = 0; }
    
    void reserve(std::size_t cap) {
        
        if(cap > capacity) {
            
            auto old = data;
            data = new CharType[cap + 1];
            capacity = cap;
            *std::copy(old, old + length, data) = 0;
            if(old != buffer.data) delete[] old;
            
        }
        
    }
    
    StringBase& append(CharType ch, std::size_t count = 1) {
        
        reserve(length + count);
        std::fill(data + length, data + length + count, ch);
        length += count;
        data[length] = 0;
        
    }
    StringBase& append(const CharType* data_, std::size_t length_) {
        
        reserve(length + length_);
        *std::copy(data_, data_ + length_, data + length) = 0;
        length += length_;
        
    }
    StringBase& append(const CharType* data_) { return append(data_, std::char_traits<CharType>::length(data_)); }
    StringBase& append(const StringBase& str) { return append(str.data, str.length); }
    StringBase& append(const View& sv) { return append(sv.getData(), sv.getLength()); }
    
    void swap(StringBase& src) noexcept {
        
        if(data == buffer.data)
            if(src.data == src.buffer.data) { data = buffer.data; src.data = src.buffer.data; }
            else { data = src.data; src.data = src.buffer.data; }
        else
            if(src.data == src.buffer.data) { src.data = data; data = buffer.data; }
            else std::swap(data, src.data);
        std::swap(length, src.length);
        std::swap(capacity, src.capacity);
        std::swap(buffer, src.buffer);
        
    }
    
};

template <typename C, typename T = typename C::CharType>
inline std::basic_ostream<T>& operator <<(std::basic_ostream<T>& stream, const StringBase<C>& str) {
    
    stream.write(str.getData(), str.getLength());
    return stream;
    
}

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
