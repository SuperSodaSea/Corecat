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

#ifndef CATS_CORECAT_DATA_ARRAY_HPP
#define CATS_CORECAT_DATA_ARRAY_HPP


#include <cstddef>

#include <iterator>
#include <memory>
#include <utility>

#include "Allocator/DefaultAllocator.hpp"
#include "../Util/Iterator.hpp"


namespace Cats {
namespace Corecat {
inline namespace Data {

template <typename T>
class ArrayView;

template <typename T, typename A = DefaultAllocator>
class Array {
    
public:
    
    using Type = T;
    using AllocatorType = A;
    
    using Iterator = Type*;
    using ConstIterator = const Type*;
    using ReverseIterator = Corecat::ReverseIterator<Iterator>;
    using ConstReverseIterator = Corecat::ReverseIterator<ConstIterator>;
    
    using ArrayViewType = ArrayView<T>;
    using ConstArrayViewType = ArrayView<const T>;
    
private:
    
    A allocator;
    Type* data = nullptr;
    std::size_t size = 0;
    std::size_t capacity = 0;
    
public:
    
    Array() = default;
    Array(std::size_t size_) { resize(size_); }
    Array(const Array& src) {
        
        data = static_cast<T*>(allocator.allocate(src.size * sizeof(T)));
        size = src.size;
        capacity = size;
        std::size_t i = 0;
        try {
            
            for(; i < size; ++i) new(data + i) T(src.data[i]);
                
        } catch(...) {
            
            for(--i; i != std::size_t(-1); --i) data[i].~T();
            allocator.deallocate(data, capacity * sizeof(T));
            std::rethrow_exception(std::current_exception());
            
        }
        
    }
    Array(Array&& src) noexcept { swap(src); }
    ~Array() {
        
        clear();
        if(data) allocator.deallocate(data, capacity * sizeof(T));
        
    }
    
    Array& operator =(const Array& src) = delete;
    Array& operator =(Array&& src) noexcept { swap(*this, src); return *this; }
    
    operator ConstArrayViewType() const noexcept { return getView(); }
    operator ArrayViewType() noexcept { return getView(); }
    
    const Type& operator [](std::size_t index) const noexcept { return data[index]; }
    Type& operator [](std::size_t index) noexcept { return data[index]; }
    
    const Type* getData() const noexcept { return data; }
    Type* getData() noexcept { return data; }
    std::size_t getSize() const noexcept { return size; }
    std::size_t getCapacity() const noexcept { return capacity; }
    
    ConstArrayViewType getView() const noexcept { return {data, size}; }
    ArrayViewType getView() noexcept { return {data, size}; }
    
    bool isEmpty() const noexcept { return !size; }
    
    void clear() noexcept { resize(0); }
    
    void reserve(std::size_t capacity_) {
        
        if(capacity >= capacity_) return;
        T* data_ = static_cast<T*>(allocator.allocate(capacity_ * sizeof(T)));
        std::size_t i = 0;
        try {
            
            for(; i < size; ++i) new(data_ + i) T(data[i]);
                
        } catch(...) {
            
            for(--i; i != std::size_t(-1); --i) data_[i].~T();
            allocator.deallocate(data_, capacity_ * sizeof(T));
            std::rethrow_exception(std::current_exception());
            
        }
        if(data) {
            
            for(std::size_t i = size - 1; i != std::size_t(-1); --i) data[i].~T();
            allocator.deallocate(data, capacity * sizeof(T));
            
        }
        data = data_;
        capacity = capacity_;
        
    }
    
    void resize(std::size_t size_) {
        
        if(size_ < size) {
            
            for(std::size_t i = size_ - 1; i != size_ - 1; --i)
                data[i].~T();
            
        } else if(size_ > size) {
            
            if(size_ <= capacity) {
                
                std::size_t i = size;
                try {
                    
                    for(; i < size_; ++i) new(data + i) T();
                    
                } catch(...) {
                    
                    for(--i; i != size - 1; --i) data[i].~T();
                    std::rethrow_exception(std::current_exception());
                    
                }
                
            } else {
                
                T* data_ = static_cast<T*>(allocator.allocate(size_ * sizeof(T)));
                std::size_t i = 0;
                try {
                    
                    for(; i < size; ++i) new(data_ + i) T(data[i]);
                    for(; i < size_; ++i) new(data_ + i) T();
                        
                } catch(...) {
                    
                    for(--i; i != std::size_t(-1); --i) data_[i].~T();
                    allocator.deallocate(data_, size_ * sizeof(T));
                    std::rethrow_exception(std::current_exception());
                    
                }
                if(data) {
                    
                    for(std::size_t i = size - 1; i != std::size_t(-1); --i) data[i].~T();
                    allocator.deallocate(data, capacity * sizeof(T));
                    
                }
                data = data_;
                capacity = size_;
                
            }
            
        }
        size = size_;
        
    }
    
    template <typename... Arg>
    void append(Arg&&... arg) {
        
        if(size < capacity) {
            
            new(data + size) T(std::forward<Arg>(arg)...);
            
        } else {
            
            std::size_t newSize = size + 1;
            T* newData = static_cast<T*>(allocator.allocate(newSize * sizeof(T)));
            std::size_t i = 0;
            try {
                
                for(; i < size; ++i) new(newData + i) T(data[i]);
                new(newData + size) T(std::forward<Arg>(arg)...);
                    
            } catch(...) {
                
                for(--i; i != std::size_t(-1); --i) newData[i].~T();
                allocator.deallocate(newData, newSize * sizeof(T));
                std::rethrow_exception(std::current_exception());
                
            }
            if(data) {
                
                for(std::size_t i = size - 1; i != std::size_t(-1); --i) data[i].~T();
                allocator.deallocate(data, capacity * sizeof(T));
                
            }
            data = newData;
            capacity = newSize;
            
        }
        ++size;
        
    }
    
    void swap(Array& src) noexcept {
        
        std::swap(allocator, src.allocator);
        std::swap(data, src.data);
        std::swap(size, src.size);
        std::swap(capacity, src.capacity);
        
    }
    
    Iterator begin() const noexcept { return data; }
    Iterator end() const noexcept { return data + size; }
    
    ConstIterator cbegin() const noexcept { return begin(); }
    ConstIterator cend() const noexcept { return end(); }
    
    ReverseIterator rbegin() const noexcept { return ReverseIterator(end()); }
    ReverseIterator rend() const noexcept { return ReverseIterator(begin()); }
    
    ConstReverseIterator crbegin() const noexcept { return ConstReverseIterator(cend()); }
    ConstReverseIterator crend() const noexcept { return ConstReverseIterator(cbegin()); }
    
};

template <typename T>
class ArrayView {
    
public:
    
    using Type = T;
    
    using Iterator = Type*;
    using ConstIterator = const Type*;
    using ReverseIterator = Corecat::ReverseIterator<Iterator>;
    using ConstReverseIterator = Corecat::ReverseIterator<ConstIterator>;
    
private:
    
    Type* data = nullptr;
    std::size_t size = 0;
    
public:
    
    ArrayView() = default;
    ArrayView(std::nullptr_t) noexcept {}
    ArrayView(Type* data_, std::size_t size_) noexcept : data(data_), size(size_) {}
    template <std::size_t S>
    ArrayView(Type (&array)[S]) noexcept : data(array), size(S) {}
    ArrayView(const ArrayView& src) = default;
    
    ArrayView& operator =(const ArrayView& src) = default;
    
    operator ArrayView<const Type>() noexcept { return {data, size}; }
    
    Type& operator [](std::size_t index) const noexcept { return data[index]; }
    
    Type* getData() const noexcept { return data; }
    std::size_t getSize() const noexcept { return size; }
    
    bool isEmpty() const noexcept { return !size; }
    
    Iterator begin() const noexcept { return data; }
    Iterator end() const noexcept { return data + size; }
    
    ConstIterator cbegin() const noexcept { return begin(); }
    ConstIterator cend() const noexcept { return end(); }
    
    ReverseIterator rbegin() const noexcept { return ReverseIterator(end()); }
    ReverseIterator rend() const noexcept { return ReverseIterator(begin()); }
    
    ConstReverseIterator crbegin() const noexcept { return ConstReverseIterator(cend()); }
    ConstReverseIterator crend() const noexcept { return ConstReverseIterator(cbegin()); }
    
};

}
}
}


#endif
