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

#ifndef CATS_CORECAT_STREAM_BUFFEREDOUTPUTSTREAM_HPP
#define CATS_CORECAT_STREAM_BUFFEREDOUTPUTSTREAM_HPP


#include <algorithm>
#include <vector>

#include "OutputStream.hpp"


namespace Cats {
namespace Corecat {
namespace Stream {

template <typename T>
class BufferedOutputStream : public OutputStream<T> {
    
private:
    
    OutputStream<T>* os;
    std::vector<T> data;
    std::size_t size = 0;
    
public:
    
    BufferedOutputStream(OutputStream<T>& os_) : os(&os_), data(4096) {}
    BufferedOutputStream(const BufferedOutputStream& src) = delete;
    BufferedOutputStream(BufferedOutputStream&& src) : os(src.os), data(std::move(src.data)) { src.os = nullptr; }
    ~BufferedOutputStream() override { flush(); }
    
    BufferedOutputStream& operator =(const BufferedOutputStream& src) = delete;
    BufferedOutputStream& operator =(BufferedOutputStream&& src) { /* TODO */ return *this; }
    
    void write(T t) override { write(&t, 1); }
    void write(const T* buffer, std::size_t count) override {
        
        if(size + count <= data.size()) std::copy(buffer, buffer + count, data.data() + size), size += count;
        else {
            
            if(size) os->write(data.data(), size);
            if(data.size() <= count) std::copy(buffer, buffer + count, data.data()), size = count;
            else os->write(buffer, count), size = 0;
            
        }
        
    }
    void flush() override {
        
        if(size) os->write(data.data(), size), size = 0;
        os->flush();
        
    }
    
};

template <typename T>
inline BufferedOutputStream<T> createBufferedOutputStream(OutputStream<T>& os) { return BufferedOutputStream<T>(os); }

}
}
}


#endif
