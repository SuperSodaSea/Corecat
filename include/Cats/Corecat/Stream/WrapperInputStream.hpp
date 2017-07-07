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

#ifndef CATS_CORECAT_STREAM_WRAPPERINPUTSTREAM_HPP
#define CATS_CORECAT_STREAM_WRAPPERINPUTSTREAM_HPP


#include <cstdio>

#include <iostream>
#include <stdexcept>
#include <type_traits>

#include "InputStream.hpp"


namespace Cats {
namespace Corecat {
namespace Stream {

template <typename T, typename = void>
class WrapperInputStream;
template <>
class WrapperInputStream<std::FILE*> : public InputStream<char> {
    
private:
    
    std::FILE* file;
    
public:
    
    WrapperInputStream(std::FILE* file_) : file(file_) {}
    WrapperInputStream(const WrapperInputStream& src) = delete;
    WrapperInputStream(WrapperInputStream&& src) : file(src.file) { src.file = nullptr; }
    ~WrapperInputStream() override = default;
    
    WrapperInputStream& operator =(const WrapperInputStream& src) = delete;
    WrapperInputStream& operator =(WrapperInputStream&& src) { file = src.file, src.file = nullptr; return *this; }
    
    std::size_t readSome(char* buffer, std::size_t count) override {
        
        std::size_t ret = std::fread(buffer, 1, count, file);
        return ret;
        
    }
    void skip(std::size_t count) override {
        
        if(std::fseek(file, count, SEEK_CUR))
            throw std::runtime_error("std::fseek failed");
        
    }
    
};
template <typename T>
class WrapperInputStream<T, typename std::enable_if<std::is_base_of<std::istream, T>::value>::type> : public InputStream<char> {
    
private:
    
    std::istream* is;
    
public:
    
    WrapperInputStream(std::istream& is_) : is(&is_) {}
    WrapperInputStream(const WrapperInputStream& src) = delete;
    WrapperInputStream(WrapperInputStream&& src) : is(src.is) { src.is = nullptr; }
    ~WrapperInputStream() override = default;
    
    WrapperInputStream& operator =(const WrapperInputStream& src) = delete;
    WrapperInputStream& operator =(WrapperInputStream&& src) { is = src.is, src.is = nullptr; return *this; }
    
    std::size_t readSome(char* buffer, std::size_t count) override { is->read(buffer, count); return is->gcount(); }
    void skip(std::size_t count) override { is->ignore(count); }
    
};

template <typename T>
inline WrapperInputStream<T> createWrapperInputStream(T& t) { return WrapperInputStream<T>(t); }

}
}
}


#endif
