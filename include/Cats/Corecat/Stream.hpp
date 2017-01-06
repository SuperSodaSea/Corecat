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

#ifndef CATS_CORECAT_STREAM_HPP
#define CATS_CORECAT_STREAM_HPP


#include <cstdint>
#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <memory>
#include <system_error>
#include <type_traits>


namespace Cats {
namespace Corecat {
namespace Stream {
    
template <typename T = char>
struct Stream {
    
    using CharType = T;
    
    enum class SeekOrigin { Begin, Current, End };
    
    virtual bool isReadable() const { return false; };
    virtual T read() { T data; return read(&data, 1) ? data : 0; }
    virtual std::size_t read(T* /*data*/, std::size_t /*size*/) { throw std::runtime_error("not implemented"); }
    virtual T peek() { T data; return peek(&data, 1) ? data : 0; }
    virtual std::size_t peek(T* /*data*/, std::size_t /*size*/) { throw std::runtime_error("not implemented"); }
    
    virtual bool isWriteable() const { return false; };
    virtual void write(T data) { write(&data, 1); }
    virtual void write(const T* /*data*/, std::size_t /*size*/) { throw std::runtime_error("not implemented"); }
    virtual void flush() { throw std::runtime_error("not implemented"); }
    
    virtual bool isSeekable() const { return false; };
    virtual std::intmax_t seek(std::intmax_t /*offset*/, SeekOrigin /*method*/) { throw std::runtime_error("not implemented"); }
    virtual std::intmax_t getPosition() { throw std::runtime_error("not implemented"); }
    
};


template <typename T>
class StreamBuffer {
    
private:
    
    T* data;
    T* start;
    T* end;
    std::size_t size;
    std::size_t avail;
    
public:
    
    StreamBuffer(std::size_t size_ = 4096) : data(new T[size_]), start(data), end(data), size(size_), avail() {}
    
    void read(Stream<T>& stream) {
        
        if(avail == size) return;
        std::size_t count;
        if(end < start) count = stream.read(end, start - end);
        else count = stream.read(end, data + size - end);
        end += count;
        if(end == data + size) end = data;
        avail += count;
        
    }
    void read(T* data_, std::size_t count) noexcept {
        
        if(start - data + count < size) {
            
            std::copy(start, start + count, data_);
            start += count;
            
        } else {
            
            auto p = std::copy(start, data + size, data_);
            start = data + count - (p - data_);
            std::copy(data, start, p);
            
        }
        avail -= size;
        
    }
    
    void reset() noexcept {
        
        start = data;
        end = data;
        avail = 0;
        
    }
    
    std::size_t getAvail() noexcept { return avail; }
    
};

template <typename T>
class BufferedStream : public Stream<T> {
    
private:
    
    Stream<T>* stream;
    StreamBuffer<T> readBuffer;
    StreamBuffer<T> writeBuffer;
    
public:
    
    BufferedStream(Stream<T>& stream_) : stream(&stream), readBuffer(), writeBuffer() {}
    
    bool isReadable() const override { return stream->isReadable(); };
    
    bool isWriteable() const override { return stream->isWriteable(); };
    
    bool isSeekable() const override { return stream->isSeekable(); };
    
    Stream<T>& getStream() { return *stream; }
    void setStream(Stream<T>& stream_) { stream = &stream_; }
    
};


template <typename T, typename = void>
class WrapperStream;
template <>
class WrapperStream<std::FILE*> : public Stream<> {
    
private:
    
    std::FILE* file;
    
public:
    
    WrapperStream(std::FILE* file_) : file(file_) {}
    
    bool isReadable() const override { return true; };
    std::size_t read(char* data, std::size_t size) override {
        
        std::size_t ret = std::fread(data, 1, size, file);
        return ret;
        
    }
    
    bool isWriteable() const override { return true; };
    void write(const char* data, std::size_t size) override {
        
        std::fwrite(data, 1, size, file);
        
    }
    void flush() override { std::fflush(file); };
    
    bool isSeekable() const override { return true; };
    std::intmax_t seek(std::intmax_t offset, SeekOrigin origin) override {
        
        int ori;
        switch(origin) {
        
        case SeekOrigin::Begin: ori = SEEK_SET; break;
        case SeekOrigin::Current: ori = SEEK_CUR; break;
        case SeekOrigin::End: ori = SEEK_END; break;
        default: throw std::runtime_error("invalid origin");
        
        }
        std::fseek(file, offset, ori);
        return getPosition();
        
    }
    std::intmax_t getPosition() override {
        
        return std::ftell(file);
        
    }
    
};
template <typename T>
class WrapperStream<T, typename std::enable_if<std::is_base_of<std::istream, T>::value && !std::is_base_of<std::ostream, T>::value>::type> :
    public Stream<> {
    
private:
    
    std::istream* is;
    
public:
    
    WrapperStream(std::istream& is_) : is(&is_) {}
    
    bool isReadable() const override { return true; };
    std::size_t read(char* data, std::size_t size) override { is->read(data, size); return is->gcount(); }
    
    bool isSeekable() const override { return true; };
    std::intmax_t seek(std::intmax_t offset, SeekOrigin origin) override {
        
        std::ios_base::seekdir dir;
        switch(origin) {
        
        case SeekOrigin::Begin: dir = std::ios_base::beg; break;
        case SeekOrigin::Current: dir = std::ios_base::cur; break;
        case SeekOrigin::End: dir = std::ios_base::end; break;
        default: throw std::runtime_error("invalid origin");
        
        }
        is->seekg(offset, dir);
        return getPosition();
        
    }
    std::intmax_t getPosition() override { return is->tellg(); }
    
};
template <typename T>
class WrapperStream<T, typename std::enable_if<std::is_base_of<std::ostream, T>::value && !std::is_base_of<std::istream, T>::value>::type> :
    public Stream<> {
    
private:
    
    std::ostream* os;
    
public:
    
    WrapperStream(std::ostream& os_) : os(&os_) {}
    
    bool isWriteable() const override { return true; };
    void write(const char* data, std::size_t size) override { os->write(data, size); }
    void flush() override { os->flush(); };
    
    bool isSeekable() const override { return true; };
    std::intmax_t seek(std::intmax_t offset, SeekOrigin origin) override {
        
        std::ios_base::seekdir dir;
        switch(origin) {
        
        case SeekOrigin::Begin: dir = std::ios_base::beg; break;
        case SeekOrigin::Current: dir = std::ios_base::cur; break;
        case SeekOrigin::End: dir = std::ios_base::end; break;
        default: throw std::runtime_error("invalid origin");
        
        }
        os->seekp(offset, dir);
        return getPosition();
        
    }
    std::intmax_t getPosition() override { return os->tellp(); }
    
};
template <typename T>
class WrapperStream<T, typename std::enable_if<std::is_base_of<std::iostream, T>::value>::type> :
    public Stream<> {
    
private:
    
    std::iostream* ios;
    
public:
    
    WrapperStream(std::iostream& ios_) : ios(&ios_) {}
    
    bool isReadable() const override { return true; };
    std::size_t read(char* data, std::size_t size) override { ios->read(data, size); return ios->gcount(); }
    
    bool isWriteable() const override { return true; };
    void write(const char* data, std::size_t size) override { ios->write(data, size); }
    void flush() override { ios->flush(); };
    
    bool isSeekable() const override { return true; };
    std::intmax_t seek(std::intmax_t offset, SeekOrigin origin) override {
        
        std::ios_base::seekdir dir;
        switch(origin) {
        
        case SeekOrigin::Begin: dir = std::ios_base::beg; break;
        case SeekOrigin::Current: dir = std::ios_base::cur; break;
        case SeekOrigin::End: dir = std::ios_base::end; break;
        default: throw std::runtime_error("invalid origin");
        
        }
        return ios->rdbuf()->pubseekoff(static_cast<std::iostream::pos_type>(offset), dir);
        
    }
    std::intmax_t getPosition() override { return ios->tellg(); }
    
};

template <typename T>
inline WrapperStream<T> createWrapperStream(T& t) { return WrapperStream<T>(t); }

}
}
}


#endif
