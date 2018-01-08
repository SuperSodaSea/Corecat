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

#ifndef CATS_CORECAT_TEXT_FORMATTER_HPP
#define CATS_CORECAT_TEXT_FORMATTER_HPP


#include <cstddef>

#include <array>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "String.hpp"
#include "../Util/Sequence.hpp"


namespace Cats {
namespace Corecat {
inline namespace Text {

namespace Impl {

template <typename T>
struct DecimalDigit {
    
    static constexpr T get(int x) { return '0' + (x % 2 ? (x / 2) % 10 : (x / 2) / 10); }
    
};
template <typename T>
using DecimalDigitTable = SequenceTable<MapperSequence<DecimalDigit<T>, IndexSequence<int, 0, 199>>>;

template <typename T>
inline void toStringMiddle2(std::uint8_t x, T*& p) {
    
    *reinterpret_cast<std::array<T, 2>*>(p) = reinterpret_cast<const std::array<T, 2>*>(DecimalDigitTable<T>::TABLE)[x], p += 2;
    
}
template <typename T>
inline void toStringBegin2(std::uint8_t x, T*& p) {
    
    if(x < 10) *p++ = '0' + x;
    else toStringMiddle2(x, p);
    
}
template <typename T>
inline void toStringMiddle4(std::uint16_t x, T*& p) {
    
    std::uint8_t a = x / 100, b = x % 100;
    toStringMiddle2(a, p); toStringMiddle2(b, p);
    
}
template <typename T>
inline void toStringBegin4(std::uint16_t x, T*& p) {
    
    if(x < 100) toStringBegin2(std::uint8_t(x), p);
    else { std::uint8_t a = x / 100, b = x % 100; toStringBegin2(a, p), toStringMiddle2(b, p); }
    
}
template <typename T>
inline void toStringMiddle8(std::uint32_t x, T*& p) {
    
    std::uint16_t a = x / 10000, b = x % 10000;
    toStringMiddle4(a, p), toStringMiddle4(b, p);
    
}
template <typename T>
inline void toStringBegin8(std::uint32_t x, T*& p) {
    
    if(x < 10000) toStringBegin4(x, p);
    else { std::uint16_t a = x / 10000, b = x % 10000; toStringBegin4(a, p), toStringMiddle4(b, p); }
    
}
template <typename T>
inline void toStringMiddle16(std::uint64_t x, T*& p) {
    
    std::uint32_t a = x / 100000000, b = x % 100000000;
    toStringMiddle8(a, p), toStringMiddle8(b, p);
    
}

}

template <typename T>
inline T* u32ToString(std::uint32_t x, T* p) {
    
    if(x < 100000000) Impl::toStringBegin8(x, p);
    else {
        std::uint32_t a = x / 100000000, b = x % 100000000;
        Impl::toStringBegin2(a, p), Impl::toStringMiddle8(b, p);
    }
    return p;
    
}
template <typename T>
inline T* i32ToString(std::int32_t x, T* p) {
    
    std::uint32_t t;
    if(x >= 0) t = x;
    else *p++ = '-', t = std::uint32_t(0) - std::uint32_t(x);
    return u32ToString(t, p);
    
}
template <typename T>
inline T* u64ToString(std::uint64_t x, T* p) {
    
    if(x < 4294967296) return u32ToString(x, p);
    else if(x < 10000000000000000) {
        std::uint32_t a = x / 100000000, b = x % 100000000;
        Impl::toStringBegin8(a, p), Impl::toStringMiddle8(b, p);
    } else {
        std::uint64_t a = x / 10000000000000000, b = x % 10000000000000000;
        Impl::toStringBegin4(a, p), Impl::toStringMiddle16(b, p);
    }
    return p;
    
}
template <typename T>
inline T* i64ToString(std::int64_t x, T* p) {
    
    std::uint64_t t;
    if(x >= 0) t = x;
    else *p++ = '-', t = -std::uint64_t(x);
    return u64ToString(t, p);
    
}


template <typename C, typename T>
String<C> toString(T t, typename std::enable_if<std::is_unsigned<T>::value && sizeof(T) <= 4>::type* = 0) {
    
    String<C> s;
    s.setLength(u32ToString(t, s.getData()) - s.getData());
    return s;
    
}
template <typename C, typename T>
String<C> toString(T t, typename std::enable_if<std::is_signed<T>::value && sizeof(T) <= 4>::type* = 0) {
    
    String<C> s;
    s.setLength(i32ToString(t, s.getData()) - s.getData());
    return s;
    
}
template <typename C, typename T>
String<C> toString(T t, typename std::enable_if<(std::is_unsigned<T>::value && sizeof(T) > 4 && sizeof(T) <= 8)>::type* = 0) {
    
    String<C> s;
    s.setLength(u64ToString(t, s.getData()) - s.getData());
    return s;
    
}
template <typename C, typename T>
String<C> toString(T t, typename std::enable_if<(std::is_signed<T>::value && sizeof(T) > 4 && sizeof(T) <= 8)>::type* = 0) {
    
    String<C> s;
    s.setLength(i64ToString(t, s.getData()) - s.getData());
    return s;
    
}

template <typename C>
String<C> toString(String<C> s) { return s; }

template <typename T>
inline String8 toString8(T t) { return toString<UTF8Charset<>>(t); }
template <typename T>
inline String16 toString16(T t) { return toString<UTF16Charset<>>(t); }
template <typename T>
inline String32 toString32(T t) { return toString<UTF32Charset<>>(t); }


namespace Impl {

template <typename T, bool CAP>
struct Digit {
    
    static constexpr T get(int x) { return x < 10 ? '0' + x : CAP ? ('A' + (x - 10)) : ('a' + (x - 10)); }
    
};
template <typename T, bool CAP>
using DigitTable = SequenceTable<MapperSequence<Digit<T, CAP>, IndexSequence<int, 0, 35>>>;

}

template <typename C, typename T>
inline typename std::enable_if<std::is_unsigned<T>::value, String<C>>::type toStringBase(T t, std::size_t base, bool capital = false) {
    
    using CharType = typename C::CharType;
    
    if(base < 2 || base > 36) throw std::invalid_argument("Base must be in [2, 36]");
    
    auto table = capital ? Impl::DigitTable<CharType, true>::TABLE : Impl::DigitTable<CharType, false>::TABLE;
    
    CharType buffer[sizeof(T) * 8];
    auto p = std::end(buffer), q = p;
    do {
        
        *--p = table[t % base];
        t /= T(base);
        
    } while(t);
    
    return {p, std::size_t(q - p)};
    
}


namespace Impl {

template <typename T>
inline std::pair<T, T> parseFormatAlignType(const T*& p, const T* q) {
    
    T fillChar = ' ', alignType = '<';
    if(q - p >= 1){
     
        if(*p == T('<') || *p == T('>') || *p == T('^'))
            alignType = *p, ++p;
        else if(q - p >= 2 && (p[1] == T('<') || p[1] == T('>') || p[1] == T('^')))
            fillChar = p[0], alignType = p[1], p += 2;
        
    }
    return {fillChar, alignType};
    
}

template <typename T>
inline T parseFormatSignType(const T*& p, const T* q) {
    
    T signType = '-';
    if(p != q && (*p == T('+') || *p == T('-') || *p == T(' ')))
        signType = *p, ++p;
    return signType;
    
}

template <typename T>
inline bool parseFormatAlter(const T*& p, const T* q) {
    
    bool alter = false;
    if(p != q && *p == T('#'))
        alter = true, ++p;
    return alter;
    
}

template <typename T>
inline std::size_t parseFormatWidth(const T*& p, const T* q) {
    
    std::size_t width = 0;
    for(; p != q && *p >= '0' && *p <= '9'; ++p)
        width = width * 10 + (*p - '0');
    return width;
    
}

template <typename T>
inline T parseFormatType(const T*& p, const T* q, T t) {
    
    if(p != q) return *p++;
    else return t;
    
}

}


template <typename C>
void formatString(String<C>& writer, StringView<C> str, StringView<C> arg) {
    
    using CharType = typename C::CharType;
    
    if(arg.isEmpty()) { writer += str; return; }
    
    auto p = arg.begin(), q = arg.end();
    
    CharType fillChar, alignType; std::tie(fillChar, alignType) = Impl::parseFormatAlignType(p, q);
    std::size_t width = Impl::parseFormatWidth(p, q);
    
    if(p != q) throw std::invalid_argument("Invalid format specifier");
    
    std::size_t length = str.getLength();
    if(width <= length) { writer += str; return; }
    std::size_t fill = width - length;
    switch(alignType) {
    case '<': writer += str, writer.append(fillChar, fill); break;
    case '>': writer.append(fillChar, fill), writer += str; break;
    case '^': {
        
        std::size_t left = fill / 2, right = fill - left;
        writer.append(fillChar, left), writer += str, writer.append(fillChar, right);
        break;
        
    }
    }
    
}
template <typename C>
void formatString(String<C>& writer, const typename C::CharType* str, StringView<C> arg) { formatString(writer, StringView<C>(str), arg); }
template <typename C>
void formatString(String<C>& writer, const String<C>& str, StringView<C> arg) { formatString(writer, StringView<C>(str), arg); }

template <typename C, typename T>
typename std::enable_if<std::is_integral<T>::value>::type formatString(String<C>& writer, T t, StringView<C> arg) {
    
    using CharType = typename C::CharType;
    using UnsignedType = typename std::make_unsigned<T>::type;
    
    if(arg.isEmpty()) { writer += toString<C>(t); return; }
    
    auto p = arg.begin(), q = arg.end();
    CharType fillChar, alignType; std::tie(fillChar, alignType) = Impl::parseFormatAlignType(p, q);
    CharType signType = Impl::parseFormatSignType(p, q);
    bool alter = Impl::parseFormatAlter(p, q);
    std::size_t width = Impl::parseFormatWidth(p, q);
    CharType type = Impl::parseFormatType(p, q, CharType('d'));
    if(type != CharType('b') && type != CharType('c') && type != CharType('d') && type != CharType('o') && type != CharType('x') && type != CharType('X'))
        throw std::invalid_argument("Invalid format type");
    if(p != q) throw std::invalid_argument("Invalid format specifier");
    
    if(type == CharType('c')) { writer += CharType(t); return; }
    
    UnsignedType x; CharType sign;
    if(t >= 0) x = t, sign = signType == '+' ? '+' : signType == '-' ? 0 : ' ';
    else x = UnsignedType(0) - UnsignedType(t), sign = '-';
    String<C> str;
    switch(type) {
    case 'b': str = alter ? String<C>("0b"_sv) + toStringBase<C>(x, 2) : toStringBase<C>(x, 2); break;
    case 'o': str = alter ? String<C>("0o"_sv) + toStringBase<C>(x, 8) : toStringBase<C>(x, 8); break;
    case 'd': str = toString<C>(x); break;
    case 'x': str = alter ? String<C>("0x"_sv) + toStringBase<C>(x, 16) : toStringBase<C>(x, 16); break;
    case 'X': str = alter ? String<C>("0x"_sv) + toStringBase<C>(x, 16, true) : toStringBase<C>(x, 16, true); break;
    }
    
    std::size_t length = str.getLength() + !!sign;
    if(width <= length) { writer += str; return; }
    std::size_t fill = width - length;
    switch(alignType) {
    case '<': if(sign) writer += sign; writer += str, writer.append(fillChar, fill); break;
    case '>': writer.append(fillChar, fill); if(sign) writer += sign; writer += str; break;
    case '^': {
        
        std::size_t left = fill / 2, right = fill - left;
        writer.append(fillChar, left); if(sign) writer += sign; writer += str, writer.append(fillChar, right);
        break;
        
    }
    }
    
}


template <typename C>
class Formatter {
    
public:
    
    using CharsetType = C;
    using CharType = typename C::CharType;
    
    using StringType = String<C>;
    using StringViewType = StringView<C>;
    
private:
    
    class HolderBase {
        
    public:
        
        virtual ~HolderBase() {}
        
        virtual void format(StringType& writer, StringViewType arg) const = 0;
        
    };
    
    template <typename T>
    class Holder final : public HolderBase {
        
    private:
        
        T t;
        
    public:
        
        Holder(T t_) : t(std::move(t_)) {}
        ~Holder() final = default;
        
        void format(StringType& writer, StringViewType arg) const final { formatString(writer, t, arg); }
        
    };
    
    struct Segment {
        
        std::size_t index;
        StringViewType arg;
        
        Segment(std::size_t index_, StringViewType arg_) : index(index_), arg(arg_) {}
        
    };
    
private:
    
    StringType data;
    std::vector<Segment> segments;
    
public:
    
    Formatter(StringViewType data_) : data(data_) {
        
        std::size_t index = 0; 
        for(auto p = data.begin(), q = data.end(); p != q; ) {
            
            auto begin = p;
            while(*p != CharType('{') && *p != CharType('}') && p != q) ++p;
            bool isEscape = false;
            if(p != q) {
                
                if(*p == '{') {
                    
                    if(*(p + 1) == '{') ++p, isEscape = true;
                    
                } else if(*p == '}') {
                    
                    if(p + 1 == q || *(p + 1) != '}') throw std::invalid_argument("Unexpected end of format string");
                    ++p, isEscape = true;
                    
                }
                
            }
            if(p != begin) {
                
                segments.emplace_back(-1, StringViewType(begin, p - begin));
                if(isEscape) ++p;
                
            } else {
                
                ++p;
                begin = p;
                while(p != q && *p >= '0' && *p <= '9') ++p;
                if(p != begin) {
                    
                    index = *begin - '0';
                    for(++begin; begin != p; ++begin) index = index * 10 + (*begin - '0');
                    
                }
                if(p == q) throw std::invalid_argument("Unexpected end of format string");
                else if(*p == '}') segments.emplace_back(index, StringViewType(begin, p - begin));
                else if(*p == ':') {
                    
                    begin = ++p;
                    while(p != q && *p != CharType('}')) ++p;
                    if(p == q) throw std::invalid_argument("Unexpected end of format string");
                    segments.emplace_back(index, StringViewType(begin, p - begin));
                    
                } else throw std::invalid_argument("Unexpected character");
                ++p;
                ++index;
                
            }
            
        }
        
    }
    
    template<typename... Arg>
    StringType operator ()(Arg&&... arg) const { return format(std::forward<Arg>(arg)...); }
    
    template<typename... Arg>
    StringType format(Arg&&... arg) const {
        
        std::array<std::unique_ptr<HolderBase>, sizeof...(arg)> arr = {{std::unique_ptr<HolderBase>(new Holder<typename std::decay<Arg>::type>(arg))...}};
        StringType str;
        for(auto&& segment : segments) {
            
            auto index = segment.index;
            auto arg = segment.arg;
            if(index == std::size_t(-1)) str += arg;
            else {
                
                if(index >= arr.size()) throw std::invalid_argument("Too few argument for formatter");
                arr[index]->format(str, arg);
                ++index;
                
            }
            
        }
        return str;
        
    }
    
};

using Formatter8 = Formatter<UTF8Charset<>>;
using Formatter16 = Formatter<UTF16Charset<>>;
using Formatter32 = Formatter<UTF32Charset<>>;

inline namespace FormatterLiteral {

inline Formatter8 operator "" _format(const char* str, std::size_t len) { return {{str, len}}; }
inline Formatter16 operator "" _format(const char16_t* str, std::size_t len) { return {{str, len}}; }
inline Formatter32 operator "" _format(const char32_t* str, std::size_t len) { return {{str, len}}; }

}

}
}
}


#endif
