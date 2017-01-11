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

#ifndef CATS_CORECAT_STREAM_WRAPPERSTREAM_HPP
#define CATS_CORECAT_STREAM_WRAPPERSTREAM_HPP


#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <system_error>
#include <type_traits>

#include "Stream.hpp"


namespace Cats {
namespace Corecat {
namespace Stream {

template <typename T, typename = void>
class WrapperStream;
template <>
class WrapperStream<std::FILE*> : public Stream<> {
    
private:
    
    std::FILE* file;
    
public:
    
    WrapperStream(std::FILE* file_) : file(file_) {}
    
    bool isReadable() const override { return true; };
    using Stream<>::read;
    std::size_t read(char* buffer, std::size_t count) override {
        
        std::size_t ret = std::fread(buffer, 1, count, file);
        return ret;
        
    }
    
    bool isWriteable() const override { return true; };
    using Stream<>::write;
    void write(const char* buffer, std::size_t count) override {
        
        std::fwrite(buffer, 1, count, file);
        
    }
    void flush() override { std::fflush(file); };
    
    bool isSeekable() const override { return true; };
    std::intmax_t seek(std::intmax_t offset, SeekOrigin origin) override {
        
        int ori;
        switch(origin) {
        
        case SeekOrigin::Begin: ori = SEEK_SET; break;
        case SeekOrigin::Current: ori = SEEK_CUR; break;
        case SeekOrigin::End: ori = SEEK_END; break;
        default: throw std::runtime_error("invalid origin");
        
        }
        std::fseek(file, offset, ori);
        return getPosition();
        
    }
    std::intmax_t getPosition() override {
        
        return std::ftell(file);
        
    }
    
};
template <typename T>
class WrapperStream<T, typename std::enable_if<std::is_base_of<std::istream, T>::value && !std::is_base_of<std::ostream, T>::value>::type> :
    public Stream<> {
    
private:
    
    std::istream* is;
    
public:
    
    WrapperStream(std::istream& is_) : is(&is_) {}
    
    bool isReadable() const override { return true; };
    using Stream<>::read;
    std::size_t read(char* buffer, std::size_t count) override { is->read(buffer, count); return is->gcount(); }
    using Stream<>::skip;
    void skip(std::size_t count) override { is->ignore(count); }
    
    bool isSeekable() const override { return true; };
    std::intmax_t seek(std::intmax_t offset, SeekOrigin origin) override {
        
        std::ios_base::seekdir dir;
        switch(origin) {
        
        case SeekOrigin::Begin: dir = std::ios_base::beg; break;
        case SeekOrigin::Current: dir = std::ios_base::cur; break;
        case SeekOrigin::End: dir = std::ios_base::end; break;
        default: throw std::runtime_error("invalid origin");
        
        }
        is->seekg(offset, dir);
        return getPosition();
        
    }
    std::intmax_t getPosition() override { return is->tellg(); }
    
};
template <typename T>
class WrapperStream<T, typename std::enable_if<std::is_base_of<std::ostream, T>::value && !std::is_base_of<std::istream, T>::value>::type> :
    public Stream<> {
    
private:
    
    std::ostream* os;
    
public:
    
    WrapperStream(std::ostream& os_) : os(&os_) {}
    
    bool isWriteable() const override { return true; };
    using Stream<>::write;
    void write(const char* buffer, std::size_t count) override { os->write(buffer, count); }
    void flush() override { os->flush(); };
    
    bool isSeekable() const override { return true; };
    std::intmax_t seek(std::intmax_t offset, SeekOrigin origin) override {
        
        std::ios_base::seekdir dir;
        switch(origin) {
        
        case SeekOrigin::Begin: dir = std::ios_base::beg; break;
        case SeekOrigin::Current: dir = std::ios_base::cur; break;
        case SeekOrigin::End: dir = std::ios_base::end; break;
        default: throw std::runtime_error("invalid origin");
        
        }
        os->seekp(offset, dir);
        return getPosition();
        
    }
    std::intmax_t getPosition() override { return os->tellp(); }
    
};
template <typename T>
class WrapperStream<T, typename std::enable_if<std::is_base_of<std::iostream, T>::value>::type> :
    public Stream<> {
    
private:
    
    std::iostream* ios;
    
public:
    
    WrapperStream(std::iostream& ios_) : ios(&ios_) {}
    
    bool isReadable() const override { return true; };
    using Stream<>::read;
    std::size_t read(char* buffer, std::size_t count) override { ios->read(buffer, count); return ios->gcount(); }
    using Stream<>::skip;
    void skip(std::size_t count) override { ios->ignore(count); }
    
    bool isWriteable() const override { return true; };
    using Stream<>::write;
    void write(const char* buffer, std::size_t count) override { ios->write(buffer, count); }
    void flush() override { ios->flush(); };
    
    bool isSeekable() const override { return true; };
    std::intmax_t seek(std::intmax_t offset, SeekOrigin origin) override {
        
        std::ios_base::seekdir dir;
        switch(origin) {
        
        case SeekOrigin::Begin: dir = std::ios_base::beg; break;
        case SeekOrigin::Current: dir = std::ios_base::cur; break;
        case SeekOrigin::End: dir = std::ios_base::end; break;
        default: throw std::runtime_error("invalid origin");
        
        }
        return ios->rdbuf()->pubseekoff(static_cast<std::iostream::pos_type>(offset), dir);
        
    }
    std::intmax_t getPosition() override { return ios->tellg(); }
    
};

template <typename T>
inline WrapperStream<T> createWrapperStream(T& t) { return WrapperStream<T>(t); }

}
}
}


#endif
