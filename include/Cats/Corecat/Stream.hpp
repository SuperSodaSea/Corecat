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

#include <iostream>
#include <system_error>


namespace Cats {
namespace Corecat {
namespace Stream {

struct Base {
    
    enum class SeekOrigin { Begin, Current, End };
    
    virtual ~Base() {}
    
    virtual bool isReadable() const { return false; };
    virtual std::size_t read(char* /*data*/, std::size_t /*size*/) { throw std::runtime_error("not implemented"); }
    
    virtual bool isWriteable() const { return false; };
    virtual void write(const char* /*data*/, std::size_t /*size*/) { throw std::runtime_error("not implemented"); }
    virtual void flush() { throw std::runtime_error("not implemented"); }
    
    virtual bool isSeekable() const { return false; };
    virtual std::intmax_t seek(std::intmax_t /*offset*/, SeekOrigin /*method*/) { throw std::runtime_error("not implemented"); }
    virtual std::intmax_t getPosition() { throw std::runtime_error("not implemented"); }
    
};

template <typename T>
class Warpper;
template <>
class Warpper<std::FILE*> : public Base {
    
private:
    
    std::FILE* file;
    
public:
    
    Warpper(std::FILE* file_) : file(file_) {}
    
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
template <>
class Warpper<std::istream> : public Base {
    
private:
    
    std::istream* is;
    
public:
    
    Warpper(std::istream& is_) : is(&is_) {}
    
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
template <>
class Warpper<std::ostream> : public Base {
    
private:
    
    std::ostream* os;
    
public:
    
    Warpper(std::ostream& os_) : os(&os_) {}
    
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
template <>
class Warpper<std::iostream> : public Base {
    
private:
    
    std::iostream* ios;
    
public:
    
    Warpper(std::iostream& ios_) : ios(&ios_) {}
    
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
inline Warpper<T> createWrapper(T& t) { return Warpper<T>(t); }

}
}
}


#endif
