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

#ifndef CATS_CORECAT_STREAM_TRANSFORMSTREAM_HPP
#define CATS_CORECAT_STREAM_TRANSFORMSTREAM_HPP


#include <cstdlib>

#include "Stream.hpp"


namespace Cats {
namespace Corecat {
namespace Stream {

template <typename T = char>
class TransformStream : public Stream<T> {
    
private:
    
    Stream<>* stream;
    
public:
    
    TransformStream(Stream<>& stream_) : stream(&stream_) {}
    
    bool isReadable() const override { return stream->isReadable(); }
    using Stream<T>::read;
    std::size_t read(T* buffer, std::size_t count) override {
        
        std::size_t res = stream->peek(reinterpret_cast<char*>(buffer), count * sizeof(T));
        stream->skip(res - res % sizeof(T));
        return res / sizeof(T);
        
    }
    using Stream<T>::peek;
    std::size_t peek(T* buffer, std::size_t count) override {
        
        std::size_t res = stream->peek(reinterpret_cast<char*>(buffer), count * sizeof(T));
        return res / sizeof(T);
        
    }
    
    bool isWriteable() const override { return stream->isWriteable(); }
    using Stream<T>::write;
    void write(const T* buffer, std::size_t count) override {
        
        stream->write(reinterpret_cast<const char*>(buffer), count * sizeof(T));
        
    }
    void flush() override {
        
        stream->flush();
        
    }
    
    bool isSeekable() const override { return stream->isSeekable(); }

    Stream<>& getStream() { return *stream; }
    void setStream(Stream<>& stream_) { stream = &stream_; }
    
};

template <typename T>
inline TransformStream<T> createTransformyStream(Stream<>& stream) { return TransformStream<T>(stream); }

}
}
}


#endif
