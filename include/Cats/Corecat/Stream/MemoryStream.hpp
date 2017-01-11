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

#ifndef CATS_CORECAT_STREAM_MEMORYSTREAM_HPP
#define CATS_CORECAT_STREAM_MEMORYSTREAM_HPP


#include <cstdlib>

#include <algorithm>

#include "Stream.hpp"


namespace Cats {
namespace Corecat {
namespace Stream {

template <typename T = char>
class MemoryStream : public Stream<T> {
    
private:
    
    T* data;
    T* end;
    T* cur;
    
public:
    
    MemoryStream(T* data_, std::size_t size) : data(data_), end(data + size), cur(data) {}
    
    bool isReadable() const override { return true; }
    T read() override {
        
        if(cur == end) return 0;
        else return *(cur++);
        
    }
    std::size_t read(T* buffer, std::size_t count) override {
        
        std::size_t res = std::min<std::size_t>(count, end - cur);
        std::copy(cur, cur + res, buffer);
        cur += res;
        return res;
        
    }
    T peek() override {
        
        if(cur == end) return 0;
        else return *cur;
        
    }
    std::size_t peek(T* buffer, std::size_t count) override {
        
        std::size_t res = std::min<std::size_t>(count, end - cur);
        std::copy(cur, cur + res, buffer);
        return res;
        
    }
    void skip() override {
        
        if(cur <=  end) cur++;
        
    }
    void skip(std::size_t count) override {
        
        if(cur - data + count <=  end - data) cur += count;
        else cur = end;
        
    }
    
    bool isWriteable() const override { return true; }
    void write(T value) override {
        
        if(cur == end) return;
        else *(cur++) = value;
        
    }
    void write(const T* buffer, std::size_t count) override {
        
        std::size_t res = std::min<std::size_t>(count, end - cur);
        std::copy(buffer, buffer + res, cur);
        cur += res;
        
    }
    void flush() override {}
    
    bool isSeekable() const override { return true; }
    using typename Stream<T>::SeekOrigin;
    std::intmax_t seek(std::intmax_t offset, SeekOrigin origin) override {
        
        std::intmax_t pos;
        switch(origin) {
        
        case SeekOrigin::Begin: pos = offset; break;
        case SeekOrigin::Current: pos = cur - data + offset; break;
        case SeekOrigin::End: pos = end - data + offset; break;
        default: throw std::runtime_error("invalid origin");
        
        }
        pos = std::min<std::intmax_t>(std::max<std::intmax_t>(pos, 0), end - data);
        cur = data + pos;
        return pos;
        
    }
    std::intmax_t getPosition() override { return cur - data; }
    
    T* getData() { return data; }
    T* getCurrent() { return cur; }
    T* getEnd() { return end; }
    
};

template <typename T>
inline MemoryStream<T> createMemoryStream(T* buffer, std::size_t size) { return MemoryStream<T>(buffer, size); }

}
}
}


#endif
