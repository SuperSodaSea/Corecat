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

#ifndef CATS_CORECAT_DATA_STREAM_WRAPPEROUTPUTSTREAM_HPP
#define CATS_CORECAT_DATA_STREAM_WRAPPEROUTPUTSTREAM_HPP


#include <cstdio>

#include <iostream>
#include <stdexcept>
#include <type_traits>

#include "InputStream.hpp"


namespace Cats {
namespace Corecat {
inline namespace Data {

template <typename T, typename = void>
class WrapperOutputStream;
template <>
class WrapperOutputStream<std::FILE*> : public OutputStream<char> {
    
private:
    
    std::FILE* file;
    
public:
    
    WrapperOutputStream(std::FILE* file_) : file(file_) {}
    WrapperOutputStream(WrapperOutputStream&& src) : file(src.file) { src.file = nullptr; }
    ~WrapperOutputStream() override = default;
    
    WrapperOutputStream& operator =(WrapperOutputStream&& src) { file = src.file, src.file = nullptr; return *this; }
    
    std::size_t writeSome(const char* buffer, std::size_t count) override {
        
        if(std::fwrite(buffer, 1, count, file) != count)
            throw std::runtime_error("std::fwrite failed");
        return count;
        
    }
    void flush() override {
        
        if(std::fflush(file))
            throw std::runtime_error("std::fflush failed");
        
    };
    
};
template <typename T>
class WrapperOutputStream<T, typename std::enable_if<std::is_base_of<std::ostream, T>::value>::type> : public OutputStream<char> {
    
private:
    
    std::ostream* os;
    
public:
    
    WrapperOutputStream(std::ostream& os_) : os(&os_) {}
    WrapperOutputStream(WrapperOutputStream&& src) : os(src.os) { src.os = nullptr; }
    ~WrapperOutputStream() override = default;
    
    WrapperOutputStream& operator =(WrapperOutputStream&& src) { os = src.os, src.os = nullptr; return *this; }
    
    std::size_t writeSome(const char* buffer, std::size_t count) override { os->write(buffer, count); return count; }
    void flush() override { os->flush(); };
    
};

template <typename T>
inline WrapperOutputStream<T> createWrapperOutputStream(T& t) { return WrapperOutputStream<T>(t); }

}
}
}


#endif
