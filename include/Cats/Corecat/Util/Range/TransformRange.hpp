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

#ifndef CATS_CORECAT_UTIL_RANGE_TRANSFORMRANGE_HPP
#define CATS_CORECAT_UTIL_RANGE_TRANSFORMRANGE_HPP


#include "RangeOperator.hpp"
#include "RangeTraits.hpp"


namespace Cats {
namespace Corecat {
inline namespace Util {

template <typename R, typename F>
class TransformRange {
    
private:
    
    using Iter = typename RangeTraits<R>::IteratorType;
    
public:
    
    class Iterator {
        
    private:
        
        friend TransformRange;
        
    public:
        
        using value_type = decltype(std::declval<F>()(*std::declval<Iter>()));
        using difference_type = typename std::iterator_traits<Iter>::difference_type;
        using reference = value_type&;
        using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
        
        class pointer {
            
        private:
            
            value_type data;
            
        public:
            
            pointer(value_type data_) : data(std::move(data_)) {}
            
            const value_type& operator *() const noexcept { return data; }
            value_type& operator *() noexcept { return data; }
            const value_type* operator ->() const noexcept { return &data; }
            value_type* operator ->() noexcept { return &data; }
            
        };
        
    private: 
        
        template <typename X>
        using EnableIfForwardIterator = std::enable_if_t<IsForwardIterator<Iter>, X>;
        template <typename X>
        using EnableIfBidirectionalIterator = std::enable_if_t<IsBidirectionalIterator<Iter>, X>;
        template <typename X>
        using EnableIfRandomAccessIterator = std::enable_if_t<IsRandomAccessIterator<Iter>, X>;
        
        const TransformRange* r;
        Iter i;
        
    private:
        
        Iterator(const TransformRange* r_, Iter i_) : r(r_), i(std::move(i_)) {}
        
    public:
        
        value_type operator *() const { return r->f(*i); }
        pointer operator ->() const { return **this; }
        Iterator& operator ++() { ++i; return *this; }
        friend bool operator ==(const Iterator& a, const Iterator& b) { return a.i == b.i; }
        friend bool operator !=(const Iterator& a, const Iterator& b) { return a.i != b.i; }
        
        template <typename X = Iterator&>
        EnableIfForwardIterator<X> operator ++(int) { auto t = *this; ++i; return t; }
        
        template <typename X = Iterator&>
        EnableIfBidirectionalIterator<X> operator --() { --i; return *this; }
        template <typename X = Iterator&>
        EnableIfBidirectionalIterator<X> operator --(int) { auto t = *this; --i; return t; }
        
        template <typename X = Iterator&>
        EnableIfRandomAccessIterator<X> operator +=(difference_type n) { i += n; return *this; }
        template <typename X = Iterator&>
        EnableIfRandomAccessIterator<X> operator -=(difference_type n) { i -= n; return *this; }
        template <typename X = Iterator>
        friend EnableIfRandomAccessIterator<X> operator +(Iterator a, difference_type b) { return a += b; }
        template <typename X = Iterator>
        friend EnableIfRandomAccessIterator<X> operator +(difference_type a, Iterator b) { return b += a; }
        template <typename X = Iterator>
        friend EnableIfRandomAccessIterator<X> operator -(Iterator a, difference_type b) { return a -= b; }
        template <typename X = difference_type>
        friend EnableIfRandomAccessIterator<X> operator -(const Iterator& a, const Iterator& b) { return a.i - b.i; }
        template <typename X = value_type>
        EnableIfRandomAccessIterator<X> operator [](difference_type n) { return *(*this + n); }
        template <typename X = bool>
        friend EnableIfRandomAccessIterator<X> operator <(const Iterator& a, const Iterator& b) { return a.i < b.i; }
        template <typename X = bool>
        friend EnableIfRandomAccessIterator<X> operator >(const Iterator& a, const Iterator& b) { return b < a; }
        template <typename X = bool>
        friend EnableIfRandomAccessIterator<X> operator <=(const Iterator& a, const Iterator& b) { return !(b < a); }
        template <typename X = bool>
        friend EnableIfRandomAccessIterator<X> operator >=(const Iterator& a, const Iterator& b) { return !(a < b); }
        
    };
    
private:
    
    R r;
    F f;
    
public:
    
    TransformRange(R r_, F f_) : r(std::move(r_)), f(std::move(f_)) {}
    
    Iterator begin() const { return {this, std::begin(r)}; }
    Iterator end() const { return {this, std::end(r)}; }
    
};


namespace Impl {

struct TransformRangeFunc {
    
    template <typename F>
    RangeOperator<TransformRangeFunc, F> operator ()(F&& f) const { return {*this, std::forward<F>(f)}; }
    template <typename R, typename F>
    TransformRange<std::remove_cv_t<std::remove_reference_t<R>>, std::remove_cv_t<std::remove_reference_t<F>>> operator ()(R&& r, F&& f) const { return {std::forward<R>(r), std::forward<F>(f)}; }
    
};

}
namespace { constexpr Impl::TransformRangeFunc transform; }

}
}
}


#endif
