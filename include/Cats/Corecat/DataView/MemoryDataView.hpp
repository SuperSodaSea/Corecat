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

#ifndef CATS_CORECAT_DATAVIEW_MEMORYDATAVIEW_HPP
#define CATS_CORECAT_DATAVIEW_MEMORYDATAVIEW_HPP


#include <stdexcept>

#include "DataView.hpp"


namespace Cats {
namespace Corecat {
namespace DataView {


template <typename T>
class MemoryDataView : public DataView<T> {
    
private:
    
    T* data;
    std::size_t size;
    
public:
    
    MemoryDataView(T* data_, std::size_t size_) : data(data_), size(size_) {}
    MemoryDataView(const MemoryDataView& src) = delete;
    MemoryDataView(MemoryDataView&& src) = default;
    ~MemoryDataView() override = default;
    
    MemoryDataView& operator =(const MemoryDataView& src) = delete;
    MemoryDataView& operator =(MemoryDataView&& src) = default;
    
    bool isReadable() override { return true; }
    bool isWritable() override { return true; }
    bool isResizable() override { return false; }
    void read(T* buffer, std::size_t count, std::uint64_t offset) override {
        
        if(offset + count > getSize())
            throw std::invalid_argument("End of data");
        std::copy(data + offset, data + offset + count, buffer);
        
    }
    void write(const T* buffer, std::size_t count, std::uint64_t offset) override {
        
        if(offset + count > getSize())
            throw std::invalid_argument("End of data");
        std::copy(buffer, buffer + count, data + offset);
        
    }
    void flush() override {}
    std::uint64_t getSize() override { return size; }
    void setSize(std::uint64_t size) override { throw std::runtime_error("Not resizable"); }
    
};


}
}
}


#endif
