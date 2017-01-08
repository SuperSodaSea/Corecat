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
    virtual T read() { T value; return read(&value, 1) ? value : 0; }
    virtual std::size_t read(T* /*buffer*/, std::size_t /*count*/) { throw std::runtime_error("stream is not readable"); }
    virtual T peek() { T value; return peek(&value, 1) ? value : 0; }
    virtual std::size_t peek(T* /*buffer*/, std::size_t /*count*/) { throw std::runtime_error("stream is not readable"); }
    
    virtual bool isWriteable() const { return false; };
    virtual void write(T value) { write(&value, 1); }
    virtual void write(const T* /*buffer*/, std::size_t /*count*/) { throw std::runtime_error("stream is not writeable"); }
    virtual void flush() { throw std::runtime_error("stream is not writeable"); }
    
    virtual bool isSeekable() const { return false; };
    virtual std::intmax_t seek(std::intmax_t /*offset*/, SeekOrigin /*method*/) { throw std::runtime_error("stream is not seekable"); }
    virtual std::intmax_t getPosition() { throw std::runtime_error("stream is not seekable"); }
    
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
    
    std::intmax_t position;
    
public:
    
    BufferedStream(Stream<T>& stream_, std::size_t bufferSize_ = 4096) :
        stream(&stream_), bufferSize(bufferSize_),
        readBufferData(), readBufferStart(), readBufferEnd(),
        writeBufferData(), writeBufferEnd(),
        position() {
        
        if(isSeekable()) position = stream->getPosition();
        
    }
    ~BufferedStream() {
        
        if(readBufferData) { delete[] readBufferData; readBufferData = nullptr; }
        if(writeBufferData) {
            
            if(isWriteable()) flush();
            delete[] writeBufferData; writeBufferData = nullptr;
            
        }
        
    }
    
    bool isReadable() const override { return stream->isReadable(); }
    using Stream<T>::read;
    std::size_t read(T* buffer, std::size_t count) override {
        
        if(!readBufferData) {
            
            readBufferData = new T[bufferSize];
            readBufferStart = readBufferEnd = readBufferData;
            
        }
        if(readBufferStart == readBufferData) readBufferEnd += stream->read(readBufferEnd, readBufferData + bufferSize - readBufferEnd);
        std::size_t res = std::min(count, static_cast<std::size_t>(readBufferEnd - readBufferStart));
        std::copy(readBufferStart, readBufferStart + res, buffer);
        readBufferStart += res;
        if(readBufferStart == readBufferEnd) readBufferStart = readBufferEnd = readBufferData;
        return res;
        
    }
    using Stream<T>::peek;
    std::size_t peek(T* buffer, std::size_t count) override {
        
        if(!readBufferData) {
            
            readBufferData = new T[bufferSize];
            readBufferStart = readBufferEnd = readBufferData;
            
        }
        if(static_cast<std::size_t>(readBufferEnd - readBufferStart) >= count) {
            
            std::copy(readBufferStart, readBufferStart + count, buffer);
            return count;
            
        } else {
            
            readBufferEnd += stream->read(readBufferEnd, readBufferData + bufferSize - readBufferEnd);
            std::size_t res = std::min(count, static_cast<std::size_t>(readBufferEnd - readBufferStart));
            std::copy(readBufferStart, readBufferStart + res, buffer);
            return res;
            
        }
        
    }
    
    bool isWriteable() const override { return stream->isWriteable(); }
    using Stream<T>::write;
    void write(const T* buffer, std::size_t count) override {
        
        if(!writeBufferData) {
            
            writeBufferData = new T[bufferSize];
            writeBufferEnd = writeBufferData;
            
        }
        if(writeBufferEnd - writeBufferData + count > bufferSize) {
            
            flush();
            if(count >= bufferSize) { stream->write(buffer, count); return; }
            
        }
        writeBufferEnd = std::copy(buffer, buffer + count, writeBufferEnd);
        
    }
    void flush() override {
        
        stream->write(writeBufferData, writeBufferEnd - writeBufferData);
        writeBufferEnd = writeBufferData;
        
    }
    
    bool isSeekable() const override { return stream->isSeekable(); }
    using typename Stream<T>::SeekOrigin;
    std::intmax_t seek(std::intmax_t offset, SeekOrigin origin) override {
        
        return position;
        
    }
    std::intmax_t getPosition() override {
        
        return position;
        
    }
    
    Stream<T>& getStream() { return *stream; }
    void setStream(Stream<T>& stream_) {
        
        if(isWriteable()) flush();
        stream = &stream_;
        if(isSeekable()) position = stream->getPosition();
        
    }
    
};

template <typename T>
inline BufferedStream<T> createBufferedStream(Stream<T>& t) { return BufferedStream<T>(t); }


template <typename T>
class MemoryStream : public Stream<T> {
    
private:
    
    T* data;
    T* end;
    T* cur;
    
public:
    
    MemoryStream(T* data_, std::size_t size) : data(data_), end(data + size), cur(data) {}
    
    bool isReadable() const override { return true; }
    T read() override {
        
        if(cur == end) return 0;
        else return *(cur++);
        
    }
    std::size_t read(T* buffer, std::size_t count) override {
        
        std::size_t res = std::min<std::size_t>(count, end - cur);
        std::copy(cur, cur + res, buffer);
        cur += res;
        return res;
        
    }
    T peek() override {
        
        if(cur == end) return 0;
        else return *cur;
        
    }
    std::size_t peek(T* buffer, std::size_t count) override {
        
        std::size_t res = std::min<std::size_t>(count, end - cur);
        std::copy(cur, cur + res, buffer);
        return res;
        
    }
    
    bool isWriteable() const override { return true; }
    void write(T value) override {
        
        if(cur == end) return;
        else *(cur++) = value;
        
    }
    void write(const T* buffer, std::size_t count) override {
        
        std::size_t res = std::min<std::size_t>(count, end - cur);
        std::copy(buffer, buffer + res, cur);
        cur += res;
        
    }
    void flush() override {}
    
    bool isSeekable() const override { return true; }
    using typename Stream<T>::SeekOrigin;
    std::intmax_t seek(std::intmax_t offset, SeekOrigin origin) override {
        
        std::intmax_t pos;
        switch(origin) {
        
        case SeekOrigin::Begin: pos = offset; break;
        case SeekOrigin::Current: pos = cur - data + offset; break;
        case SeekOrigin::End: pos = end - data + offset; break;
        default: throw std::runtime_error("invalid origin");
        
        }
        pos = std::min<std::intmax_t>(std::max<std::intmax_t>(pos, 0), end - data);
        cur = data + pos;
        return pos;
        
    }
    std::intmax_t getPosition() override { return cur - data; }
    
};

template <typename T>
inline MemoryStream<T> createMemoryStream(T* buffer, std::size_t size) { return MemoryStream<T>(buffer, size); }


template <typename T, typename = void>
class WrapperStream;
template <>
class WrapperStream<std::FILE*> : public Stream<> {
    
private:
    
    std::FILE* file;
    
public:
    
    WrapperStream(std::FILE* file_) : file(file_) {}
    
    bool isReadable() const override { return true; };
    using Stream<>::read;
    std::size_t read(char* buffer, std::size_t count) override {
        
        std::size_t ret = std::fread(buffer, 1, count, file);
        return ret;
        
    }
    
    bool isWriteable() const override { return true; };
    using Stream<>::write;
    void write(const char* buffer, std::size_t count) override {
        
        std::fwrite(buffer, 1, count, file);
        
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
    using Stream<>::read;
    std::size_t read(char* buffer, std::size_t count) override { is->read(buffer, count); return is->gcount(); }
    
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
    using Stream<>::write;
    void write(const char* buffer, std::size_t count) override { os->write(buffer, count); }
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
    using Stream<>::read;
    std::size_t read(char* buffer, std::size_t count) override { ios->read(buffer, count); return ios->gcount(); }
    
    bool isWriteable() const override { return true; };
    using Stream<>::write;
    void write(const char* buffer, std::size_t count) override { ios->write(buffer, count); }
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
