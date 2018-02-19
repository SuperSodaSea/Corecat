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

#ifndef CATS_CORECAT_DATA_STREAM_DATAVIEWOUTPUTSTREAM_HPP
#define CATS_CORECAT_DATA_STREAM_DATAVIEWOUTPUTSTREAM_HPP


#include "OutputStream.hpp"
#include "../DataView/DataView.hpp"
#include "../../Util/Exception.hpp"


namespace Cats {
namespace Corecat {
inline namespace Data {

template <typename T>
class DataViewOutputStream : public OutputStream<T> {
    
private:
    
    DataView<T>* dv;
    std::uint64_t offset;
    
public:
    
    DataViewOutputStream(DataView<T>& dv_, std::uint64_t offset_ = 0) : dv(&dv_), offset(offset_) {
        
        if(!dv->isWritable()) throw InvalidArgumentException("DataView is not writable");
        
    }
    DataViewOutputStream(DataViewOutputStream&& src) : dv(src.dv), offset(src.offset) { src.dv = nullptr; }
    ~DataViewOutputStream() override = default;
    
    DataViewOutputStream& operator =(DataViewOutputStream&& src) { dv = src.dv, src.dv = nullptr, offset = src.offset; return *this; }
    
    std::size_t write(const T* buffer, std::size_t count) override {
        
        dv->write(buffer, count, offset);
        offset += count;
        return count;
        
    }
    void flush() override { dv->flush(); }
    
};

template <typename T>
inline DataViewOutputStream<T> createDataViewOutputStream(DataView<T>& dv, std::uint64_t offset = 0) { return DataViewOutputStream<T>(dv, offset); }

}
}
}


#endif
