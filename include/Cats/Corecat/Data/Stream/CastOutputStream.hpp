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

#ifndef CATS_CORECAT_DATA_STREAM_CASTOUTPUTSTREAM_HPP
#define CATS_CORECAT_DATA_STREAM_CASTOUTPUTSTREAM_HPP


#include <algorithm>

#include "OutputStream.hpp"


namespace Cats {
namespace Corecat {
inline namespace Data {

template <typename T, typename U>
class CastOutputStream : public OutputStream<T> {
    
private:
    
    OutputStream<U>* os;
    std::uint8_t data[sizeof(U)];
    std::size_t size = 0;
    
public:
    
    CastOutputStream(OutputStream<U>& os_) : os(&os_) {}
    ~CastOutputStream() override = default;
    
    std::size_t write(const T* buffer, std::size_t count) override {
        
        if(size + count * sizeof(T) < sizeof(U)) {
            
            std::memcpy(data + size, buffer, count * sizeof(T));
            size += count * sizeof(T);
            
        } else if(size) {
            
            std::memcpy(data + size, buffer, sizeof(U) - size);
            os->write(reinterpret_cast<U*>(data), 1);
            std::size_t x = size + count * sizeof(T) - sizeof(U);
            if(x >= sizeof(U)) os->write(reinterpret_cast<const U*>(reinterpret_cast<const std::uint8_t*>(buffer) + sizeof(U)), x / sizeof(U));
            size = x % sizeof(U);
            std::memcpy(data, reinterpret_cast<const std::uint8_t*>(buffer) + sizeof(U) + x - size, size);
            
        } else {
            
            std::size_t x = count * sizeof(T);
            if(x >= sizeof(U)) os->write(reinterpret_cast<const U*>(buffer), x / sizeof(U));
            size = x % sizeof(U);
            std::memcpy(data, reinterpret_cast<const std::uint8_t*>(buffer) + x - size, size);
            
        }
        return count;
        
    }
    void flush() override { os->flush(); }
    
};

template <typename T, typename U>
inline CastOutputStream<T, U> createCastOutputStream(OutputStream<U>& os) { return CastOutputStream<T, U>(os); }

}
}
}


#endif
