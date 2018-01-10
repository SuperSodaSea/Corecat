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

#ifndef CATS_CORECAT_DATA_STREAM_BUFFEREDINPUTSTREAM_HPP
#define CATS_CORECAT_DATA_STREAM_BUFFEREDINPUTSTREAM_HPP


#include <algorithm>
#include <vector>

#include "InputStream.hpp"


namespace Cats {
namespace Corecat {
inline namespace Data {

template <typename T>
class BufferedInputStream : public InputStream<T> {
    
private:
    
    InputStream<T>* is;
    std::vector<T> data;
    std::size_t offset = 0;
    std::size_t size = 0;
    
public:
    
    BufferedInputStream(InputStream<T>& is_) : is(&is_), data(4096) {}
    BufferedInputStream(BufferedInputStream&& src) : is(src.is), data(std::move(src.data)), offset(src.offset), size(src.size) { src.is = nullptr; }
    ~BufferedInputStream() override = default;
    
    BufferedInputStream& operator =(BufferedInputStream&& src) {
        
        is = src.is, src.is = nullptr;
        data = std::move(src.data), offset = src.offset, size = src.size;
        return *this;
        
    }
    
    std::size_t readSome(T* buffer, std::size_t count) override {
        
        if(size) {
            
            std::size_t x = std::min(count, size);
            std::copy(data.data() + offset, data.data() + offset + x, buffer);
            offset += x, size -= x;
            return x;
            
        } else {
            
            if(count < data.size() / 2) {
                
                size = is->readSome(data.data(), data.size());
                offset = std::min(count, size);
                std::copy(data.data(), data.data() + offset, buffer);
                size -= offset;
                return offset;
                
            } else return is->readSome(buffer, count);
            
        }
        
    }
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
