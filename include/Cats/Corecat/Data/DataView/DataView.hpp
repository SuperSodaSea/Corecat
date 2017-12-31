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

#ifndef CATS_CORECAT_DATA_DATAVIEW_DATAVIEW_HPP
#define CATS_CORECAT_DATA_DATAVIEW_DATAVIEW_HPP


#include <cstddef>
#include <cstdint>


namespace Cats {
namespace Corecat {
inline namespace Data {

template <typename T>
struct DataView {
    
    using Type = T;
    
    virtual ~DataView() = default;
    virtual bool isReadable() = 0;
    virtual bool isWritable() = 0;
    virtual bool isResizable() = 0;
    virtual void read(T* buffer, std::size_t count, std::uint64_t offset) = 0;
    virtual void write(const T* buffer, std::size_t count, std::uint64_t offset) = 0;
    virtual void flush() = 0;
    virtual std::uint64_t getSize() = 0;
    virtual void setSize(std::uint64_t size) = 0;
    
};

}
}
}


#endif
