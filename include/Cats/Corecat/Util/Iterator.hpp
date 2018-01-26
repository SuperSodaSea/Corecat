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

#ifndef CATS_CORECAT_UTIL_ITERATOR_HPP
#define CATS_CORECAT_UTIL_ITERATOR_HPP


#include <algorithm>
#include <iterator>
#include <type_traits>
#include <utility>

#include "Detector.hpp"


namespace Cats {
namespace Corecat {
inline namespace Util {

template <typename I>
using InputIteratorConcept = std::enable_if_t<std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<I>::iterator_category>::value>;
template <typename I>
using ForwardIteratorConcept = std::enable_if_t<std::is_base_of<std::forward_iterator_tag, typename std::iterator_traits<I>::iterator_category>::value>;
template <typename I>
using BidirectionalIteratorConcept = std::enable_if_t<std::is_base_of<std::bidirectional_iterator_tag, typename std::iterator_traits<I>::iterator_category>::value>;
template <typename I>
using RandomAccessIteratorConcept = std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, typename std::iterator_traits<I>::iterator_category>::value>;

template <typename I>
constexpr bool IsInputIterator = IsDetected<InputIteratorConcept, I>;
template <typename I>
constexpr bool IsForwardIterator = IsDetected<ForwardIteratorConcept, I>;
template <typename I>
constexpr bool IsBidirectionalIterator = IsDetected<BidirectionalIteratorConcept, I>;
template <typename I>
constexpr bool IsRandomAccessIterator = IsDetected<RandomAccessIteratorConcept, I>;


template <typename I>
class ReverseIterator {
    
public:
    
    static_assert(IsBidirectionalIterator<I>, "ReverseIterator must be bidirectional");
    
    using value_type = typename std::iterator_traits<I>::value_type;
    using difference_type = typename std::iterator_traits<I>::difference_type;
    using pointer = typename std::iterator_traits<I>::pointer;
    using reference = typename std::iterator_traits<I>::reference;
    using iterator_category = typename std::iterator_traits<I>::iterator_category;
    
private:
    
    template <typename X>
    using EnableIfRandomAccessIterator = std::enable_if_t<IsRandomAccessIterator<I>, X>;
    
    I i;
    
public:
    
    ReverseIterator(I i_) : i(std::move(i_)) {}
    
    value_type operator *() const { return *--I(i); }
    pointer operator ->() const { return (--I(i)).operator ->(); }
    ReverseIterator& operator ++() { --i; return *this; }
    friend bool operator ==(const ReverseIterator& a, const ReverseIterator& b) { return a.i == b.i; }
    friend bool operator !=(const ReverseIterator& a, const ReverseIterator& b) { return a.i != b.i; }
    
    ReverseIterator& operator ++(int) { auto t = *this; --i; return t; }
    
    ReverseIterator& operator --() { ++i; return *this; }
    ReverseIterator& operator --(int) { auto t = *this; ++i; return t; }
    
    template <typename X = ReverseIterator&>
    EnableIfRandomAccessIterator<X> operator +=(difference_type n) { i -= n; return *this; }
    template <typename X = ReverseIterator&>
    EnableIfRandomAccessIterator<X> operator -=(difference_type n) { i += n; return *this; }
    template <typename X = ReverseIterator>
    friend EnableIfRandomAccessIterator<X> operator +(ReverseIterator a, difference_type b) { return a += b; }
    template <typename X = ReverseIterator>
    friend EnableIfRandomAccessIterator<X> operator +(difference_type a, ReverseIterator b) { return b += a; }
    template <typename X = ReverseIterator>
    friend EnableIfRandomAccessIterator<X> operator -(ReverseIterator a, difference_type b) { return a -= b; }
    template <typename X = difference_type>
    friend EnableIfRandomAccessIterator<X> operator -(const ReverseIterator& a, const ReverseIterator& b) { return b.i - a.i; }
    template <typename X = value_type>
    EnableIfRandomAccessIterator<X> operator [](difference_type n) { return *(*this + n); }
    template <typename X = bool>
    friend EnableIfRandomAccessIterator<X> operator <(const ReverseIterator& a, const ReverseIterator& b) { return b.i < a.i; }
    template <typename X = bool>
    friend EnableIfRandomAccessIterator<X> operator >(const ReverseIterator& a, const ReverseIterator& b) { return b < a; }
    template <typename X = bool>
    friend EnableIfRandomAccessIterator<X> operator <=(const ReverseIterator& a, const ReverseIterator& b) { return !(b < a); }
    template <typename X = bool>
    friend EnableIfRandomAccessIterator<X> operator >=(const ReverseIterator& a, const ReverseIterator& b) { return !(a < b); }
    
};


template <typename I, typename T>
std::enable_if_t<!IsRandomAccessIterator<I>, I> advanceUntil(I b, I e, T count) {
    
    for(T i = T(); b != e && i < count; ++b, ++i);
    return b;
    
}
template <typename I, typename T>
std::enable_if_t<IsRandomAccessIterator<I>, I> advanceUntil(I b, I e, T count) {
    
    return b + std::min(count, e - b);
    
}

}
}
}


#endif
