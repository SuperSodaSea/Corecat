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
    virtual std::size_t read(T* /*data*/, std::size_t /*count*/) { throw std::runtime_error("not implemented"); }
    virtual T peek() { T data; return peek(&data, 1) ? data : 0; }
    virtual std::size_t peek(T* /*data*/, std::size_t /*count*/) { throw std::runtime_error("not implemented"); }
    
    virtual bool isWriteable() const { return false; };
    virtual void write(T data) { write(&data, 1); }
    virtual void write(const T* /*data*/, std::size_t /*count*/) { throw std::runtime_error("not implemented"); }
    virtual void flush() { throw std::runtime_error("not implemented"); }
    
    virtual bool isSeekable() const { return false; };
    virtual std::intmax_t seek(std::intmax_t /*offset*/, SeekOrigin /*method*/) { throw std::runtime_error("not implemented"); }
    virtual std::intmax_t getPosition() { throw std::runtime_error("not implemented"); }
    
};


template <typename T>
class BufferedStream : public Stream<T> {
    
private:
    
    Stream<T>* stream;
    
    std::size_t bufferSize;
    
    T* readBufferData;
    T* readBufferStart;
    T* readBufferEnd;
    
    T* writeBufferData;
    T* writeBufferEnd;
    
private:
    
    std::size_t readToBuffer() {
        
        std::size_t res = stream->read(readBufferEnd, readBufferData + bufferSize - readBufferEnd);
        return res;
        
    }
    std::size_t readFromBuffer(T* data, std::size_t count) noexcept {
        
        std::copy(readBufferStart, readBufferStart + count, data);
        readBufferStart += count;
        if(readBufferStart == readBufferEnd) readBufferStart = readBufferEnd = readBufferData;
        return count;
        
    }
    
public:
    
    BufferedStream(Stream<T>& stream_, std::size_t bufferSize_ = 4096) :
        stream(&stream_), bufferSize(bufferSize_),
        readBufferData(), readBufferStart(), readBufferEnd(),
        writeBufferData(), writeBufferEnd() {}
    ~BufferedStream() {
        
        if(readBufferData) { delete[] readBufferData; readBufferData = nullptr; }
        if(writeBufferData) { delete[] writeBufferData; writeBufferData = nullptr; }
        
    }
    
    bool isReadable() const override { return stream->isReadable(); };
    std::size_t read(char* data, std::size_t count) override {
        
        if(!readBufferData) {
            
            readBufferData = new T[bufferSize];
            readBufferStart = readBufferEnd = readBufferData;
            
        }
        
    }
    
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
    std::size_t read(char* data, std::size_t count) override {
        
        std::size_t ret = std::fread(data, 1, count, file);
        return ret;
        
    }
    
    bool isWriteable() const override { return true; };
    void write(const char* data, std::size_t count) override {
        
        std::fwrite(data, 1, count, file);
        
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
    std::size_t read(char* data, std::size_t count) override { is->read(data, count); return is->gcount(); }
    
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
    void write(const char* data, std::size_t count) override { os->write(data, count); }
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
    std::size_t read(char* data, std::size_t count) override { ios->read(data, count); return ios->gcount(); }
    
    bool isWriteable() const override { return true; };
    void write(const char* data, std::size_t count) override { ios->write(data, count); }
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
