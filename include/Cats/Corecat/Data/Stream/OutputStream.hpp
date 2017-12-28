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

#ifndef CATS_CORECAT_DATA_STREAM_OUTPUTSTREAM_HPP
#define CATS_CORECAT_DATA_STREAM_OUTPUTSTREAM_HPP


#include <cstddef>


namespace Cats {
namespace Corecat {
namespace Data {
namespace Stream {

template <typename T>
struct OutputStream {
    
    using Type = T;
    
    OutputStream() = default;
    OutputStream(const OutputStream& src) = delete;
    virtual ~OutputStream() = default;
    
    OutputStream& operator =(const OutputStream& src) = delete;
    
    virtual std::size_t writeSome(const T* buffer, std::size_t count) = 0;
    virtual void write(T t) { writeSome(&t, 1); }
    virtual void writeAll(const T* buffer, std::size_t count) {
        
        std::size_t size = 0;
        while(size < count) {
            
            std::size_t x = writeSome(buffer + size, count - size);
            size += x;
            
        }
        
    }
    virtual void flush() = 0;
    
};

}
}
}
}


#endif
