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
#include <iterator>
#include <string>
#include <type_traits>

#include "Charset.hpp"


namespace Cats {
namespace Corecat {

template <typename C>
class StringViewBase;


template <typename C>
class StringBase {
    
public:
    
    static constexpr std::size_t BUFFER_SIZE = 32;
    
    using CharType = typename C::CharType;
    using CharsetType = C;
    
    using Iterator = CharType*;
    using ConstIterator = const CharType*;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;
    
    using View = StringViewBase<C>;
    
private:
    
    union {
        
        struct {
            
            CharType* data;
            std::size_t length;
            std::size_t capacity;
            
        };
        struct {
            
            CharType data[BUFFER_SIZE - 1];
            CharType length;
            
        } buffer;
        
    } storage;
    
private:
    
    bool isSmall() const noexcept { return static_cast<std::size_t>(storage.buffer.length) < BUFFER_SIZE; }
    
public:

    StringBase(const StringBase& src) : StringBase() { append(src); }
    StringBase(StringBase&& src) noexcept : StringBase() { swap(src); }
    ~StringBase() noexcept { if(!isSmall()) delete[] storage.data; }
    
    StringBase() noexcept { storage.buffer.data[0] = 0; storage.buffer.length = BUFFER_SIZE - 1; }
    StringBase(CharType ch, std::size_t count = 1) : StringBase() { append(ch, count); }
    StringBase(const CharType* data_) : StringBase() { append(data_); }
    StringBase(const CharType* data_, std::size_t length_) : StringBase() { append(data_, length_); }
    StringBase(const View& sv) : StringBase() { append(sv); }
    
    StringBase& operator =(const StringBase& src) { clear(); append(src); return *this; }
    StringBase& operator =(StringBase&& src) noexcept { swap(src); return *this; }
    
    StringBase& operator =(const CharType* data_) { clear(); append(data_); return *this; }
    StringBase& operator =(const View& sv) { clear(); append(sv); return *this; }
    
    StringBase& operator +=(CharType ch) { append(ch); return *this; }
    StringBase& operator +=(const CharType* data_) { append(data_); return *this; }
    StringBase& operator +=(const StringBase& str) { append(str); return *this; }
    StringBase& operator +=(const View& sv) { append(sv); return *this; }
    
    friend StringBase operator +(const StringBase& a, CharType b) { StringBase t(a); t += b; return t; }
    friend StringBase operator +(const StringBase& a, const CharType* b) { StringBase t(a); t += b; return t; }
    friend StringBase operator +(const StringBase& a, const StringBase& b) { StringBase t(a); t += b; return t; }
    friend StringBase operator +(const StringBase& a, const View& b) { StringBase t(a); t += b; return t; }
    friend StringBase operator +(CharType a, const StringBase& b) { StringBase t(a); t += b; return t; }
    friend StringBase operator +(const CharType* a, const StringBase& b) { StringBase t(a); t += b; return t; }
    friend StringBase operator +(const View& a, const StringBase& b) { StringBase t(a); t += b; return t; }
    
    CharType& operator [](std::size_t index) noexcept { return getData()[index]; };
    const CharType& operator [](std::size_t index) const noexcept { return getData()[index]; };
    
    operator View() const noexcept { return { getData(), getLength() }; }
    
    friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& stream, const StringBase<C>& str) {
        
        stream.write(str.getData(), str.getLength());
        return stream;
        
    }
    
    CharType* getData() noexcept { return isSmall() ? storage.buffer.data : storage.data; }
    const CharType* getData() const noexcept { return const_cast<StringBase*>(this)->getData(); }
    std::size_t getLength() const noexcept { return isSmall() ? BUFFER_SIZE - storage.buffer.length - 1 : storage.length; }
    std::size_t getCapacity() const noexcept { return isSmall() ? BUFFER_SIZE - 1 : storage.capacity; }
    
    bool isEmpty() const noexcept { return getLength() == 0; }
    
    void clear() noexcept {
        
        if(isSmall()) { storage.buffer.data[0] = 0; storage.buffer.length = BUFFER_SIZE - 1; }
        else { storage.data[0] = 0; storage.length = 0; }
        
    }
    
    void reserve(std::size_t cap) {
        
        if(cap > getCapacity()) {
            
            auto length = getLength();
            auto oldData = getData();
            auto newData = new CharType[cap + 1];
            std::copy(oldData, oldData + length + 1, newData);
            storage.data = newData;
            storage.capacity = cap;
            if(isSmall()) storage.buffer.length = BUFFER_SIZE;
            else delete[] oldData;
            
        }
        
    }
    
    StringBase& append(CharType ch, std::size_t count = 1) {
        
        auto length = getLength();
        reserve(length + count);
        auto data = getData();
        std::fill(data + length, data + length + count, ch);
        length += count;
        data[length] = 0;
        if(isSmall()) storage.buffer.length = BUFFER_SIZE - length - 1;
        else storage.length = length;
        return *this;
        
    }
    StringBase& append(const CharType* data_, std::size_t length_) {
        
        auto length = getLength();
        reserve(length + length_);
        auto data = getData();
        *std::copy(data_, data_ + length_, data + length) = 0;
        length += length_;
        if(isSmall()) storage.buffer.length = BUFFER_SIZE - length - 1;
        else storage.length = length;
        return *this;
        
    }
    StringBase& append(const CharType* data_) { return append(data_, std::char_traits<CharType>::length(data_)); }
    StringBase& append(const StringBase& str) { return append(str.getData(), str.getLength()); }
    StringBase& append(const View& sv) { return append(sv.getData(), sv.getLength()); }
    
    void swap(StringBase& src) noexcept { std::swap(storage, src.storage); }
    
    Iterator begin() noexcept { return getData(); }
    ConstIterator begin() const noexcept { return getData(); }
    ConstIterator cbegin() const noexcept { return getData(); }
    
    Iterator end() noexcept { return getData() + getLength(); }
    ConstIterator end() const noexcept { return getData() + getLength(); }
    ConstIterator cend() const noexcept { return getData() + getLength(); }
    
    ReverseIterator rbegin() noexcept { return ReverseIterator(end()); }
    ConstReverseIterator rbegin() const noexcept { return ConstReverseIterator(end()); }
    ConstReverseIterator crbegin() const noexcept { return ConstReverseIterator(cend()); }
    
    ReverseIterator rend() noexcept { return ReverseIterator(begin()); }
    ConstReverseIterator rend() const noexcept { return ConstReverseIterator(begin()); }
    ConstReverseIterator crend() const noexcept { return ConstReverseIterator(cbegin()); }
    
};

using String = StringBase<Charset::UTF8<>>;
using String16 = StringBase<Charset::UTF16<>>;
using String32 = StringBase<Charset::UTF32<>>;


template <typename C>
class StringViewBase {
    
public:
    
    using CharType = typename C::CharType;
    using CharsetType = C;
    
    using Iterator = const CharType*;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    
private:
    
    const CharType* data;
    std::size_t length;
    
public:
    
    StringViewBase(const StringViewBase& src) = default;
    ~StringViewBase() = default;
    
    StringViewBase() : data(""), length() {}
    StringViewBase(const CharType* data_) : data(data_), length(std::char_traits<CharType>::length(data_)) {}
    StringViewBase(const CharType* data_, std::size_t length_) : data(data_), length(length_) {}
    
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
    
    friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& stream, const StringViewBase& sv) {
        
        stream.write(sv.getData(), sv.getLength());
        return stream;
        
    }
    
    const CharType* getData() const noexcept { return data; }
    void setData(const CharType* data_) noexcept { data = data_; }
    std::size_t getLength() const noexcept { return length; }
    void setLength(std::size_t length_) noexcept { length = length_; }
    
    Iterator begin() const noexcept { return getData(); }
    
    Iterator end() const noexcept { return getData() + getLength(); }
    
    ReverseIterator rbegin() const noexcept { return ReverseIterator(end()); }
    
    ReverseIterator rend() const noexcept { return ReverseIterator(begin()); }
    
};

using StringView = StringViewBase<Charset::UTF8<>>;
using StringView16 = StringViewBase<Charset::UTF16<>>;
using StringView32 = StringViewBase<Charset::UTF32<>>;

}
}


#endif
