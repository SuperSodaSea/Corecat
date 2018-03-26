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

#ifndef CATS_CORECAT_DATA_ALLOCATOR_DEFAULTALLOCATOR_HPP
#define CATS_CORECAT_DATA_ALLOCATOR_DEFAULTALLOCATOR_HPP


#include <cstdlib>


namespace Cats {
namespace Corecat {
inline namespace Data {
inline namespace Allocator {

class DefaultAllocator {
    
public:
    
    DefaultAllocator() = default;
    DefaultAllocator(const DefaultAllocator& src) = delete;
    DefaultAllocator(DefaultAllocator&& src) = default;
    
    DefaultAllocator& operator =(const DefaultAllocator& src) = delete;
    DefaultAllocator& operator =(DefaultAllocator&& src) = default;
    
    void* allocate(std::size_t size) {
        
        return std::malloc(size);
        
    }
    
    void deallocate(void* data, std::size_t /*size*/) noexcept {
        
        std::free(data);
        
    }
    
};

}
}
}
}


#endif
