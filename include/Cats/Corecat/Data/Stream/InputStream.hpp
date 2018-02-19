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

#ifndef CATS_CORECAT_DATA_STREAM_INPUTSTREAM_HPP
#define CATS_CORECAT_DATA_STREAM_INPUTSTREAM_HPP


#include <cstddef>

#include "../../Util/Exception.hpp"


namespace Cats {
namespace Corecat {
inline namespace Data {

template <typename T>
struct InputStream {
    
    using Type = T;
    
    InputStream() = default;
    InputStream(const InputStream& src) = delete;
    virtual ~InputStream() = default;
    
    InputStream& operator =(const InputStream& src) = delete;
    
    virtual std::size_t read(T* buffer, std::size_t count) = 0;
    virtual T read() { T t; if(read(&t, 1)) return t; else throw IOException("End of stream"); }
    virtual void readAll(T* buffer, std::size_t count) {
        
        std::size_t size = 0;
        while(size < count) {
            
            std::size_t x = read(buffer + size, count - size);
            if(!x) throw IOException("End of stream");
            size += x;
            
        }
        
    }
    virtual void skip(std::size_t count) = 0;
    virtual void skip() { skip(1); }
    
    
};

}
}
}


#endif
