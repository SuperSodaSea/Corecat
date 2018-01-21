/*
 *
 * MIT License
 *
 * Copyright (c) 2016-2018 The Cats Project
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

#ifndef CATS_CORECAT_TEXT_STRING_HPP
#define CATS_CORECAT_TEXT_STRING_HPP


#include <cstddef>

#include <algorithm>
#include <iostream>
#include <string>

#include "Charset/DefaultCharset.hpp"
#include "../Util/Iterator.hpp"


namespace Cats {
namespace Corecat {
inline namespace Text {

template <typename C>
class StringView;
template <typename C>
class Formatter;


template <typename C>
class String {
    
public:
    
    static constexpr std::size_t BUFFER_SIZE = 32;
    
    using CharsetType = C;
    using CharType = typename C::CharType;
    
    using Iterator = CharType*;
    using ConstIterator = const CharType*;
    using ReverseIterator = Corecat::ReverseIterator<Iterator>;
    using ConstReverseIterator = Corecat::ReverseIterator<ConstIterator>;
    
    using StringViewType = StringView<C>;
    
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
    
    String() noexcept { storage.buffer.data[0] = 0; storage.buffer.length = BUFFER_SIZE - 1; }
    String(const CharType* data_) : String() { append(data_); }
    String(const CharType* data_, std::size_t length_) : String() { append(data_, length_); }
    explicit String(CharType ch, std::size_t count = 1) : String() { append(ch, count); }
    explicit String(const StringViewType& sv) : String() { append(sv); }
    template <typename D>
    explicit String(const StringView<D>& sv) : String() {
        
        CharType buf[8];
        const typename D::CharType* p = sv.getData();
        const typename D::CharType* pEnd = p + sv.getLength();
        while(true) {
            
            char32_t c = D::decode(p, pEnd);
            if(c == 0xFFFFFFFF) break;
            CharType* q = buf;
            C::encode(q, buf + 8, c);
            append(buf, q - buf);
            
        }
        
    }
    template <typename T>
    explicit String(const T* data_) : String(StringView<DefaultCharset<T>>(data_)) {}
    template <typename D>
    explicit String(const String<D>& s) : String(s.getView()) {}
    String(const String& src) : String() { append(src); }
    String(String&& src) noexcept : String() { swap(src); }
    ~String() { if(!isSmall()) delete[] storage.data; }
    
    String& operator =(const CharType* data_) { clear(); append(data_); return *this; }
    String& operator =(const StringViewType& sv) { clear(); append(sv); return *this; }
    String& operator =(const String& src) { clear(); append(src); return *this; }
    String& operator =(String&& src) noexcept { swap(src); return *this; }
    
    String& operator +=(CharType ch) { append(ch); return *this; }
    String& operator +=(const CharType* data_) { append(data_); return *this; }
    String& operator +=(const String& str) { append(str); return *this; }
    String& operator +=(const StringViewType& sv) { append(sv); return *this; }
    
    friend String operator +(const String& a, CharType b) { String t(a); t += b; return t; }
    friend String operator +(const String& a, const CharType* b) { String t(a); t += b; return t; }
    friend String operator +(const String& a, const String& b) { String t(a); t += b; return t; }
    friend String operator +(const String& a, const StringViewType& b) { String t(a); t += b; return t; }
    friend String operator +(CharType a, const String& b) { String t(a); t += b; return t; }
    friend String operator +(const CharType* a, const String& b) { String t(a); t += b; return t; }
    
    String& operator *=(std::size_t count) { return *this = repeat(count); }
    
    friend String operator *(const String& a, std::size_t b) { return a.repeat(b); }
    
    CharType& operator [](std::size_t index) noexcept { return getData()[index]; };
    const CharType& operator [](std::size_t index) const noexcept { return getData()[index]; };
    
    friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& stream, const String<C>& str) {
        
        stream.write(str.getData(), str.getLength());
        return stream;
        
    }
    
    bool operator ==(const String& b) const noexcept { return getView() == b.getView(); }
    bool operator ==(const CharType* b) const noexcept { return getView() == StringViewType(b); }
    bool operator ==(const StringViewType& b) const noexcept { return getView() == b; }
    friend bool operator ==(const CharType* a, const String& b) noexcept { return StringViewType(a) == b.getView(); }
    friend bool operator ==(const StringViewType& a, const String& b) noexcept { return a == b.getView(); }
    bool operator !=(const String& b) const noexcept { return getView() != b.getView(); }
    bool operator !=(const CharType* b) const noexcept { return getView() != StringViewType(b); }
    bool operator !=(const StringViewType& b) const noexcept { return getView() != b; }
    friend bool operator !=(const CharType* a, const String& b) noexcept { return StringViewType(a) != b.getView(); }
    friend bool operator !=(const StringViewType& a, const String& b) noexcept { return a != b.getView(); }
    bool operator <(const String& b) const noexcept { return getView() < b.getView(); }
    bool operator <(const CharType* b) const noexcept { return getView() < StringViewType(b); }
    bool operator <(const StringViewType& b) const noexcept { return getView() < b; }
    friend bool operator <(const CharType* a, const String& b) noexcept { return StringViewType(a) < b.getView(); }
    friend bool operator <(const StringViewType& a, const String& b) noexcept { return a < b.getView(); }
    bool operator >(const String& b) const noexcept { return getView() > b.getView(); }
    bool operator >(const CharType* b) const noexcept { return getView() > StringViewType(b); }
    bool operator >(const StringViewType& b) const noexcept { return getView() > b; }
    friend bool operator >(const CharType* a, const String& b) noexcept { return StringViewType(a) > b.getView(); }
    friend bool operator >(const StringViewType& a, const String& b) noexcept { return a > b.getView(); }
    bool operator <=(const String& b) const noexcept { return getView() <= b.getView(); }
    bool operator <=(const CharType* b) const noexcept { return getView() <= StringViewType(b); }
    bool operator <=(const StringViewType& b) const noexcept { return getView() <= b; }
    friend bool operator <=(const CharType* a, const String& b) noexcept { return StringViewType(a) <= b.getView(); }
    friend bool operator <=(const StringViewType& a, const String& b) noexcept { return a <= b.getView(); }
    bool operator >=(const String& b) const noexcept { return getView() >= b.getView(); }
    bool operator >=(const CharType* b) const noexcept { return getView() >= StringViewType(b); }
    bool operator >=(const StringViewType& b) const noexcept { return getView() >= b; }
    friend bool operator >=(const CharType* a, const String& b) noexcept { return StringViewType(a) >= b.getView(); }
    friend bool operator >=(const StringViewType& a, const String& b) noexcept { return a >= b.getView(); }
    
    operator StringViewType() const noexcept { return getView(); }
    
    CharType* getData() noexcept { return isSmall() ? storage.buffer.data : storage.data; }
    const CharType* getData() const noexcept { return const_cast<String*>(this)->getData(); }
    std::size_t getLength() const noexcept { return isSmall() ? BUFFER_SIZE - storage.buffer.length - 1 : storage.length; }
    std::size_t getCapacity() const noexcept { return isSmall() ? BUFFER_SIZE - 1 : storage.capacity; }
    
    StringViewType getView() const noexcept { return StringViewType(getData(), getLength()); }
    
    bool isEmpty() const noexcept { return !getLength(); }
    
    bool startsWith(const StringViewType& sv) const noexcept { return getView().startsWith(sv); }
    bool endsWith(const StringViewType& sv) const noexcept { return getView().endsWith(sv); }
    
    std::ptrdiff_t find(CharType ch, std::ptrdiff_t beginPos = 0) const noexcept { return getView().find(ch, beginPos); }
    std::ptrdiff_t find(const CharType* data_, std::size_t length_, std::ptrdiff_t beginPos) const noexcept { return getView().find(data_, length_, beginPos); }
    std::ptrdiff_t find(const CharType* data_, std::ptrdiff_t beginPos = 0) const noexcept { return getView().find(data_, beginPos); }
    std::ptrdiff_t find(const StringViewType& sv, std::ptrdiff_t beginPos = 0) const noexcept { return getView().find(sv, beginPos); }
    
    String repeat(std::size_t count) const { return getView().repeat(count); }
    
    StringViewType slice(std::ptrdiff_t beginPos) const noexcept { return getView().slice(beginPos); }
    StringViewType slice(std::ptrdiff_t beginPos, std::ptrdiff_t endPos) const noexcept { return getView().slice(beginPos, endPos); }
    
    StringViewType substr(std::ptrdiff_t beginPos) const noexcept { return getView().substr(beginPos); }
    StringViewType substr(std::ptrdiff_t beginPos, std::size_t count) const noexcept { return getView().substr(beginPos, count); }
    
    template<typename... Arg>
    String format(Arg&&... arg) const { return getView().format(std::forward<Arg>(arg)...); }
    
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
            storage.length = length;
            storage.capacity = cap;
            if(isSmall()) storage.buffer.length = BUFFER_SIZE;
            else delete[] oldData;
            
        }
        
    }
    
    void setLength(std::size_t length) {
        
        reserve(length);
        getData()[length] = 0;
        if(isSmall()) storage.buffer.length = CharType(BUFFER_SIZE - length - 1);
        else storage.length = length;
        
    }
    
    String& append(CharType ch, std::size_t count = 1) {
        
        auto length = getLength();
        setLength(length + count);
        auto data = getData();
        std::fill(data + length, data + length + count, ch);
        return *this;
        
    }
    String& append(const CharType* data_, std::size_t length_) {
        
        auto length = getLength();
        setLength(length + length_);
        auto data = getData();
        std::copy(data_, data_ + length_, data + length);
        return *this;
        
    }
    String& append(const CharType* data_) { return append(data_, C::getLength(data_)); }
    String& append(const String& str) { return append(str.getData(), str.getLength()); }
    String& append(const StringViewType& sv) { return append(sv.getData(), sv.getLength()); }
    
    void swap(String& src) noexcept { std::swap(storage, src.storage); }
    
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

using String8 = String<UTF8Charset<>>;
using String16 = String<UTF16Charset<>>;
using String32 = String<UTF32Charset<>>;

inline namespace StringLiteral {

inline String8 operator "" _s(const char* str, std::size_t len) { return {str, len}; }
inline String16 operator "" _s(const char16_t* str, std::size_t len) { return {str, len}; }
inline String32 operator "" _s(const char32_t* str, std::size_t len) { return {str, len}; }

}


template <typename C>
class StringView {
    
public:
    
    using CharsetType = C;
    using CharType = typename C::CharType;
    
    using Iterator = const CharType*;
    using ConstIterator = Iterator;
    using ReverseIterator = Corecat::ReverseIterator<Iterator>;
    using ConstReverseIterator = ReverseIterator;
    
    using StringType = String<C>;
    
private:
    
    const CharType* data;
    std::size_t length;
    
public:
    
    constexpr StringView() noexcept : data(nullptr), length(0) {}
    StringView(const CharType* data_) noexcept : data(data_), length(C::getLength(data_)) {}
    constexpr StringView(const CharType* data_, std::size_t length_) noexcept : data(data_), length(length_) {}
    constexpr StringView(const StringView& src) noexcept = default;
    ~StringView() = default;
    
    StringView& operator =(const StringView& src) = default;
    
    friend StringType operator +(const StringView& a, CharType b) { StringType t(a); t += b; return t; }
    friend StringType operator +(const StringView& a, const CharType* b) { StringType t(a); t += b; return t; }
    friend StringType operator +(const StringView& a, const StringView& b) { StringType t(a); t += b; return t; }
    friend StringType operator +(const StringView& a, const StringType& b) { StringType t(a); t += b; return t; }
    friend StringType operator +(CharType a, const StringView& b) { StringType t(a); t += b; return t; }
    friend StringType operator +(const CharType* a, const StringView& b) { StringType t(a); t += b; return t; }
    
    const CharType& operator [](std::size_t index) const noexcept { return data[index]; }
    
    friend bool operator ==(const StringView& a, const StringView& b) noexcept {
        
        if(a.length != b.length) return false;
        for(auto p = a.data, q = b.data, end = p + a.length; p != end; ++p, ++q) if(*p != *q) return false;
        return true;
        
    }
    friend bool operator !=(const StringView& a, const StringView& b) noexcept { return !(a == b); }
    friend bool operator <(const StringView& a, const StringView& b) noexcept { return C::compare(a.data, a.data + a.length, b.data, b.data + b.length) < 0; }
    friend bool operator >(const StringView& a, const StringView& b) noexcept { return C::compare(a.data, a.data + a.length, b.data, b.data + b.length) > 0; }
    friend bool operator <=(const StringView& a, const StringView& b) noexcept { return C::compare(a.data, a.data + a.length, b.data, b.data + b.length) <= 0; }
    friend bool operator >=(const StringView& a, const StringView& b) noexcept { return C::compare(a.data, a.data + a.length, b.data, b.data + b.length) >= 0; }
    
    friend StringType operator *(const StringView& a, std::size_t b) { return a.repeat(b); }
    
    friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& stream, const StringView& sv) {
        
        stream.write(sv.getData(), sv.getLength());
        return stream;
        
    }
    
    const CharType* getData() const noexcept { return data; }
    void setData(const CharType* data_) noexcept { setData(data_, C::length(data_)); }
    void setData(const CharType* data_, std::size_t length_) noexcept { data = data_, length = length_; }
    std::size_t getLength() const noexcept { return length; }
    void setLength(std::size_t length_) noexcept { length = length_; }
    
    bool isEmpty() const noexcept { return !length; }
    
    bool startsWith(const StringView& sv) const noexcept {
        
        std::size_t len1 = getLength(), len2 = sv.getLength();
        if(len1 < len2) return false;
        return StringView(getData(), len2) == sv;
        
    }
    bool endsWith(const StringView& sv) const noexcept {
        
        std::size_t len1 = getLength(), len2 = sv.getLength();
        if(len1 < len2) return false;
        return StringView(getData() + len1 - len2, len2) == sv;
        
    }
    
    std::ptrdiff_t find(CharType ch, std::ptrdiff_t beginPos = 0) const noexcept {
        
        if(beginPos >= static_cast<std::ptrdiff_t>(length)) return -1;
        if(beginPos < 0) beginPos += length;
        beginPos = std::max<std::ptrdiff_t>(beginPos, 0);
        for(auto p = data + beginPos, end = data + length; p < end; ++p) if(*p == ch) return p - data;
        return -1;
        
    }
    std::ptrdiff_t find(const CharType* data_, std::size_t length_, std::ptrdiff_t beginPos) const noexcept {
        
        if(length < length_ || beginPos + length_ > length) return -1;
        if(beginPos < 0) beginPos += length;
        beginPos = std::max<std::ptrdiff_t>(beginPos, 0);
        for(auto p = data + beginPos, end1 = data + length - length_ + 1; p < end1; ++p) {
            
            auto q = p, end2 = p + length_;
            for(auto r = data_; q < end2 && *q == *r; ++q, ++r);
            if(q == end2) return p - data;
            
        }
        return -1;
        
    }
    std::ptrdiff_t find(const CharType* data_, std::ptrdiff_t beginPos = 0) const noexcept { return find(data_, C::getLength(data_), beginPos); }
    std::ptrdiff_t find(const StringView& sv, std::ptrdiff_t beginPos = 0) const noexcept { return find(sv.getData(), sv.getLength(), beginPos); }
    
    StringType repeat(std::size_t count) const {
        
        StringType t;
        t.reserve(getLength() * count);
        for(std::size_t i = 0; i < count; ++i) { t += *this; }
        return t;
        
    }
    
    StringView slice(std::ptrdiff_t beginPos) const noexcept {
        
        if(beginPos < 0) beginPos += length;
        beginPos = std::max<std::ptrdiff_t>(beginPos, 0);
        if(beginPos < static_cast<std::ptrdiff_t>(length)) return {data + beginPos, length - beginPos};
        else return {};
        
    }
    StringView slice(std::ptrdiff_t beginPos, std::ptrdiff_t endPos) const noexcept {
        
        if(beginPos < 0) beginPos += length;
        beginPos = std::max<std::ptrdiff_t>(beginPos, 0);
        if(endPos < 0) endPos += length;
        endPos = std::min<std::ptrdiff_t>(std::max<std::ptrdiff_t>(endPos, 0), length);
        if(beginPos < static_cast<std::ptrdiff_t>(length) && beginPos < endPos)
            return {data + beginPos, static_cast<std::size_t>(endPos - beginPos)};
        else return {};
        
    }
    
    StringView substr(std::ptrdiff_t beginPos) const noexcept { return slice(beginPos); }
    StringView substr(std::ptrdiff_t beginPos, std::size_t count) const noexcept {
        
        if(beginPos < 0) beginPos += length;
        beginPos = std::max<std::ptrdiff_t>(beginPos, 0);
        if(beginPos < static_cast<std::ptrdiff_t>(length)) return {data + beginPos, std::min(count, length - beginPos)};
        else return {};
        
    }
    
    template<typename... Arg>
    StringType format(Arg&&... arg) const { return Formatter<C>(*this).format(std::forward<Arg>(arg)...); }
    
    void swap(StringView& src) noexcept { std::swap(data, src.data), std::swap(length, src.length); }
    
    Iterator begin() const noexcept { return getData(); }
    Iterator end() const noexcept { return getData() + getLength(); }
    ReverseIterator rbegin() const noexcept { return ReverseIterator(end()); }
    ReverseIterator rend() const noexcept { return ReverseIterator(begin()); }
    
};

using StringView8 = StringView<UTF8Charset<>>;
using StringView16 = StringView<UTF16Charset<>>;
using StringView32 = StringView<UTF32Charset<>>;

inline namespace StringViewLiteral {

constexpr StringView8 operator "" _sv(const char* str, std::size_t len) noexcept { return {str, len}; }
constexpr StringView16 operator "" _sv(const char16_t* str, std::size_t len) noexcept { return {str, len}; }
constexpr StringView32 operator "" _sv(const char32_t* str, std::size_t len) noexcept { return {str, len}; }

}

}
}
}


#endif
