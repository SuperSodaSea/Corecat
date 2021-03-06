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

#ifndef CATS_CORECAT_UTIL_RANGE_FILTERRANGE_HPP
#define CATS_CORECAT_UTIL_RANGE_FILTERRANGE_HPP


#include "RangeOperator.hpp"
#include "RangeTraits.hpp"


namespace Cats {
namespace Corecat {
inline namespace Util {

template <typename R, typename F>
class FilterRange {
    
private:
    
    using Iter = typename RangeTraits<R>::IteratorType;
    
public:
    
    class Iterator {
        
    private:
        
        friend FilterRange;
        
    public:
        
        using value_type = typename std::iterator_traits<Iter>::value_type;
        using difference_type = typename std::iterator_traits<Iter>::difference_type;
        using pointer = typename std::iterator_traits<Iter>::pointer;
        using reference = typename std::iterator_traits<Iter>::reference;
        using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
        
    private:
        
        template <typename X>
        using EnableIfForwardIterator = std::enable_if_t<IsForwardIterator<Iter>, X>;
        template <typename X>
        using EnableIfBidirectionalIterator = std::enable_if_t<IsBidirectionalIterator<Iter>, X>;
        
        const FilterRange* r;
        Iter i;
        
    private:
        
        Iterator(const FilterRange* r_, Iter i_) : r(r_), i(std::move(i_)) {}
        
    public:
        
        value_type operator *() const { return *i; }
        pointer operator ->() const { return i.operator ->(); }
        Iterator& operator ++() {
            
            while(i != std::end(r->r) && !r->f(*++i));
            return *this;
            
        }
        friend bool operator ==(const Iterator& a, const Iterator& b) { return a.i == b.i; }
        friend bool operator !=(const Iterator& a, const Iterator& b) { return a.i != b.i; }
        
        template <typename X = Iterator>
        EnableIfForwardIterator<X> operator ++(int) { auto t = *this; ++*this; return t; }
        
        template <typename X = Iterator&>
        EnableIfBidirectionalIterator<X> operator --() {
            
            while(i != r->b && !r->f(*--i));
            return *this;
            
        }
        template <typename X = Iterator>
        EnableIfBidirectionalIterator<X> operator --(int) { auto t = *this; --*this; return t; }
        
    };
    
private:
    
    R r;
    F f;
    Iter b;
    
public:
    
    FilterRange(R r_, F f_) : r(std::move(r_)), f(std::move(f_)), b(std::begin(r)) {
        
        for(auto e = std::end(r); b != e && !f(*b); ++b);
        
    }
    
    Iterator begin() const { return {this, b}; }
    Iterator end() const { return {this, std::end(r)}; }
    
};


namespace Impl {

struct FilterRangeFunc {
    
    template <typename F>
    RangeOperator<FilterRangeFunc, F> operator ()(F&& f) const { return {*this, std::forward<F>(f)}; }
    template <typename R, typename F>
    FilterRange<typename std::decay<R>::type, typename std::decay<F>::type> operator ()(R&& r, F&& f) const { return {std::forward<R>(r), std::forward<F>(f)}; }
    
};

}
namespace { constexpr Impl::FilterRangeFunc filter; }

}
}
}


#endif
