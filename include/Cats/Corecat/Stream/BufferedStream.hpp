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

#ifndef CATS_CORECAT_STREAM_BUFFEREDSTREAM_HPP
#define CATS_CORECAT_STREAM_BUFFEREDSTREAM_HPP


#include <cstdlib>

#include <algorithm>

#include "Stream.hpp"


namespace Cats {
namespace Corecat {
namespace Stream {

template <typename T = char>
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

}
}
}


#endif
