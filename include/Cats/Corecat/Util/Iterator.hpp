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
#include <type_traits>
#include <utility>

#include "Detector.hpp"


namespace Cats {
namespace Corecat {
inline namespace Util {

template <typename I>
using IteratorConcept = VoidType<
    decltype(*std::declval<I>()),
    decltype(std::declval<I>() == std::declval<I>()),
    decltype(std::declval<I>() != std::declval<I>())>;
template <typename I>
using InputIteratorConcept = VoidType<
    IteratorConcept<I>,
    decltype(++std::declval<I&>())>;
template <typename I>
using ForwardIteratorConcept = VoidType<
    InputIteratorConcept<I>,
    decltype(std::declval<I&>()++)>;
template <typename I>
using BidirectionalIteratorConcept = VoidType<
    ForwardIteratorConcept<I>,
    decltype(--std::declval<I&>()),
    decltype(std::declval<I&>()--)>;
template <typename I>
using RandomAccessIteratorConcept = VoidType<
    BidirectionalIteratorConcept<I>,
    decltype(std::declval<I&>() += std::declval<decltype(std::declval<I>() - std::declval<I>())>()),
    decltype(std::declval<I&>() -= std::declval<decltype(std::declval<I>() - std::declval<I>())>()),
    decltype(std::declval<I>() + std::declval<decltype(std::declval<I>() - std::declval<I>())>()),
    decltype(std::declval<decltype(std::declval<I>() - std::declval<I>())>() + std::declval<I>()),
    decltype(std::declval<I>() - std::declval<decltype(std::declval<I>() - std::declval<I>())>()),
    decltype(std::declval<I>() - std::declval<I>()),
    decltype(std::declval<I>()[std::declval<decltype(std::declval<I>() - std::declval<I>())>()]),
    decltype(std::declval<I>() < std::declval<I>()),
    decltype(std::declval<I>() > std::declval<I>()),
    decltype(std::declval<I>() <= std::declval<I>()),
    decltype(std::declval<I>() >= std::declval<I>())>;

template <typename I>
constexpr bool IsIterator = IsDetected<IteratorConcept, I>;
template <typename I>
constexpr bool IsInputIterator = IsDetected<InputIteratorConcept, I>;
template <typename I>
constexpr bool IsForwardIterator = IsDetected<ForwardIteratorConcept, I>;
template <typename I>
constexpr bool IsBidirectionalIterator = IsDetected<BidirectionalIteratorConcept, I>;
template <typename I>
constexpr bool IsRandomAccessIterator = IsDetected<RandomAccessIteratorConcept, I>;

template <typename I>
struct IteratorTraits {
    
public:
    
    using ValueType = std::decay_t<decltype(*std::declval<I>())>;
    using ReferenceType = ValueType&;
    
private:
    
    template <typename X>
    using DifferenceTypeImpl = decltype(std::declval<X>() - std::declval<X>());
    
public:
    
    using DifferenceType = DetectedType<DifferenceTypeImpl, I>;
    
public:
    
    static constexpr bool IS_INPUT_ITERATOR = IsInputIterator<I>;
    static constexpr bool IS_FORWARD_ITERATOR = IsForwardIterator<I>;
    static constexpr bool IS_BIDIRECTIONAL_ITERATOR = IsBidirectionalIterator<I>;
    static constexpr bool IS_RANDOM_ACCESS_ITERATOR = IsRandomAccessIterator<I>;
    
};


template <typename I>
class ReverseIterator {
    
private:
    
    using Traits = IteratorTraits<I>;
    using ValueType = typename Traits::ValueType;
    using DiffType = typename Traits::DifferenceType;
    
    static_assert(Traits::IS_BIDIRECTIONAL_ITERATOR, "ReverseIterator must be bidirectional");
    
    template <typename X>
    using EnableIfRandomAccessIterator = std::enable_if_t<Traits::IS_RANDOM_ACCESS_ITERATOR, X>;
    
    I i;
    
public:
    
    ReverseIterator(I i_) : i(std::move(i_)) {}
    
    ValueType operator *() const { return *--I(i); }
    ReverseIterator& operator ++() { --i; return *this; }
    friend bool operator ==(const ReverseIterator& a, const ReverseIterator& b) { return a.i == b.i; }
    friend bool operator !=(const ReverseIterator& a, const ReverseIterator& b) { return a.i != b.i; }
    
    ReverseIterator& operator ++(int) { auto t = *this; --i; return t; }
    
    ReverseIterator& operator --() { ++i; return *this; }
    ReverseIterator& operator --(int) { auto t = *this; ++i; return t; }
    
    template <typename X = ReverseIterator&>
    EnableIfRandomAccessIterator<X> operator +=(DiffType n) { i -= n; return *this; }
    template <typename X = ReverseIterator&>
    EnableIfRandomAccessIterator<X> operator -=(DiffType n) { i += n; return *this; }
    template <typename X = ReverseIterator>
    friend EnableIfRandomAccessIterator<X> operator +(ReverseIterator a, DiffType b) { return a += b; }
    template <typename X = ReverseIterator>
    friend EnableIfRandomAccessIterator<X> operator +(DiffType a, ReverseIterator b) { return b += a; }
    template <typename X = ReverseIterator>
    friend EnableIfRandomAccessIterator<X> operator -(ReverseIterator a, DiffType b) { return a -= b; }
    template <typename X = DiffType>
    friend EnableIfRandomAccessIterator<X> operator -(const ReverseIterator& a, const ReverseIterator& b) { return b.i - a.i; }
    template <typename X = ValueType>
    EnableIfRandomAccessIterator<X> operator [](DiffType n) { return *(*this + n); }
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
std::enable_if_t<!IteratorTraits<I>::IS_RANDOM_ACCESS_ITERATOR, I> advanceUntil(I b, I e, T count) {
    
    for(T i = T(); b != e && i < count; ++b, ++i);
    return b;
    
}
template <typename I, typename T>
std::enable_if_t<IteratorTraits<I>::IS_RANDOM_ACCESS_ITERATOR, I> advanceUntil(I b, I e, T count) {
    
    return b + std::min(count, e - b);
    
}

}
}
}


#endif
