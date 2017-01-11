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

#ifndef CATS_CORECAT_STREAM_STREAM_HPP
#define CATS_CORECAT_STREAM_STREAM_HPP


#include <cstdlib>

#include <system_error>


namespace Cats {
namespace Corecat {
namespace Stream {

template <typename T = char>
struct Stream {
    
    using CharType = T;
    
    enum class SeekOrigin { Begin, Current, End };
    
    virtual bool isReadable() const { return false; };
    virtual T read() { T value; return read(&value, 1) ? value : 0; }
    virtual std::size_t read(T* /*buffer*/, std::size_t /*count*/) { throw std::runtime_error("stream is not readable"); }
    virtual T peek() { T value; return peek(&value, 1) ? value : 0; }
    virtual std::size_t peek(T* /*buffer*/, std::size_t /*count*/) { throw std::runtime_error("stream is not readable"); }
    virtual void skip() { skip(1); }
    virtual void skip(std::size_t /*count*/) { throw std::runtime_error("stream is not readable"); }
    
    virtual bool isWriteable() const { return false; };
    virtual void write(T value) { write(&value, 1); }
    virtual void write(const T* /*buffer*/, std::size_t /*count*/) { throw std::runtime_error("stream is not writeable"); }
    virtual void flush() { throw std::runtime_error("stream is not writeable"); }
    
    virtual bool isSeekable() const { return false; };
    virtual std::intmax_t seek(std::intmax_t /*offset*/, SeekOrigin /*method*/) { throw std::runtime_error("stream is not seekable"); }
    virtual std::intmax_t getPosition() { throw std::runtime_error("stream is not seekable"); }
    
};

}
}
}


#endif
