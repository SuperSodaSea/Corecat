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

#ifndef CATS_CORECAT_STREAM_BUFFEREDINPUTSTREAM_HPP
#define CATS_CORECAT_STREAM_BUFFEREDINPUTSTREAM_HPP


#include <algorithm>
#include <stdexcept>

#include "InputStream.hpp"


namespace Cats {
namespace Corecat {
namespace Stream {

template <typename T>
class BufferedInputStream : public InputStream<T> {
    
private:
    
    InputStream<T>* is;
    std::vector<T> data;
    std::size_t offset = 0;
    std::size_t size = 0;
    
public:
    
    BufferedInputStream(InputStream<T>& is_) : is(&is_), data(4096) {}
    BufferedInputStream(const BufferedInputStream& src) = delete;
    BufferedInputStream(BufferedInputStream&& src) : is(src.is), data(std::move(src.data)) { src.is = nullptr; }
    ~BufferedInputStream() override = default;
    
    BufferedInputStream& operator =(const BufferedInputStream& src) = delete;
    BufferedInputStream& operator =(BufferedInputStream&& src) { /* TODO */ return *this; }
    
    T read() override { T t; if(read(&t, 1)) return t; else throw std::runtime_error("End of stream"); }
    std::size_t read(T* buffer, std::size_t count) override {
        
        if(count <= size) {
            
            std::copy(data.data() + offset, data.data() + offset + count, buffer);
            offset += count, size -= count;
            return count;
            
        } else {
            
            if(size) std::copy(data.data() + offset, data.data() + offset + size, buffer);
            if(count < data.size() / 2) {
                
                std::size_t x = is->read(data.data(), count - size);
                if(x <= count - size) {
                    
                    std::copy(data.data(), data.data() + x, buffer + size);
                    x += size;
                    size = 0;
                    return x; 
                    
                } else {
                    
                    std::copy(data.data(), data.data() + count - size, buffer + size);
                    offset = count - size;
                    size = x - offset;
                    return count;
                    
                }
                
            } else {
                
                std::size_t x = size + is->read(buffer + size, count - size);
                size = 0;
                return x;
                
            }
            
        }
        
    }
    void skip() override { skip(1); }
    void skip(std::size_t count) override {
        
        if(count <= size) offset += count, size -= count;
        else is->skip(count - size), size = 0;
    }
    
};

template <typename T>
inline BufferedInputStream<T> createBufferedInputStream(InputStream<T>& is) { return BufferedInputStream<T>(is); }

}
}
}


#endif
