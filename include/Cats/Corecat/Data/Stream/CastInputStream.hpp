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

#ifndef CATS_CORECAT_DATA_STREAM_CASTINPUTSTREAM_HPP
#define CATS_CORECAT_DATA_STREAM_CASTINPUTSTREAM_HPP


#include <cstring>

#include <stdexcept>

#include "InputStream.hpp"


namespace Cats {
namespace Corecat {
inline namespace Data {

template <typename T, typename U>
class CastInputStream : public InputStream<T> {
    
private:
    
    InputStream<U>* is;
    std::uint8_t data[sizeof(T)];
    std::size_t size = 0;
    
public:
    
    CastInputStream(InputStream<U>& is_) : is(&is_) {}
    ~CastInputStream() override = default;
    
    std::size_t readSome(T* buffer, std::size_t count) override {
        
        std::memcpy(buffer, data, size);
        std::size_t x = size;
        while(x < sizeof(T)) {
            
            std::size_t y = is->read(reinterpret_cast<U*>(reinterpret_cast<std::uint8_t*>(buffer) + x), count * sizeof(T) - x) * sizeof(U);
            if(!y) return 0;
            x += y;
            
        }
        size = x % sizeof(T);
        std::memcpy(data, reinterpret_cast<std::uint8_t*>(buffer) + x - size, size);
        return x / sizeof(T);
        
    }
    void skip(std::size_t count) override { /* TODO */ }
    
};

template <typename T, typename U>
inline CastInputStream<T, U> createCastInputStream(InputStream<U>& is) { return CastInputStream<T, U>(is); }

}
}
}


#endif
