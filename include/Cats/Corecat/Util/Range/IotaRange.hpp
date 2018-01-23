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

#ifndef CATS_CORECAT_UTIL_RANGE_IOTARANGE_HPP
#define CATS_CORECAT_UTIL_RANGE_IOTARANGE_HPP


#include <type_traits>
#include <utility>


namespace Cats {
namespace Corecat {
inline namespace Util {

template <typename T>
class IotaRange {
    
public:
    
    class Iterator {
        
    private:
        
        friend IotaRange;
        
        using DifferenceType = typename std::make_signed<T>::type;
        
        T data, step;
        bool end;
        
    private:
        
        Iterator() : end(true) {}
        Iterator(T data_, T step_) : data(std::move(data_)), step(std::move(step_)), end(false) {}
        
    public:
        
        T operator *() const { return data; }
        Iterator& operator ++() { data += step; return *this; }
        friend bool operator ==(const Iterator& a, const Iterator& b) { return !a.end && !b.end && a.data == b.data; }
        friend bool operator !=(const Iterator& a, const Iterator& b) { return a.end || b.end || a.data != b.data; }
        
        Iterator& operator ++(int) { auto t = *this; data += step; return t; }
        
        Iterator& operator --() { data -= step; return *this; }
        Iterator& operator --(int) { auto t = *this; data -= step; return t; }
        
        Iterator& operator +=(DifferenceType n) { data += step * n; return *this; }
        Iterator& operator -=(DifferenceType n) { data -= step * n; return *this; }
        friend Iterator operator +(Iterator a, DifferenceType b) { return a += b; }
        friend Iterator operator +(DifferenceType a, Iterator b) { return b += a; }
        friend Iterator operator -(Iterator a, DifferenceType b) { return a -= b; }
        friend DifferenceType operator -(const Iterator& a, const Iterator& b) {
            
            return (a.end && b.end) ? DifferenceType()
                : a.end ? std::numeric_limits<DifferenceType>::max()
                : b.end ? std::numeric_limits<DifferenceType>::min()
                : (DifferenceType(a.data - b.data) / a.step);
            
        }
        T operator [](DifferenceType n) { return *(*this + n); }
        friend bool operator <(const Iterator& a, const Iterator& b) { return !a.end && (b.end || ((a.step < 0) ^ (a.data < b.data))); }
        friend bool operator >(const Iterator& a, const Iterator& b) { return b < a; }
        friend bool operator <=(const Iterator& a, const Iterator& b) { return !(b < a); }
        friend bool operator >=(const Iterator& a, const Iterator& b) { return !(a < b); }
        
    };
    
private:
    
    T first, step;
    
public:
    
    IotaRange(T first_, T step_ = 1) : first(first_), step(step_) {}
    
    Iterator begin() const noexcept { return {first, step}; }
    Iterator end() const noexcept { return {}; }
    
};

template <typename T>
inline IotaRange<T> iota(T first, T step = 1) { return {first, step}; }

}
}
}


#endif
