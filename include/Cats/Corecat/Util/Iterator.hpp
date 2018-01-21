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


#include <type_traits>
#include <utility>

#include "Detector.hpp"


namespace Cats {
namespace Corecat {
inline namespace Util {

template <typename I>
struct IteratorTraits {
    
public:
    
    using ValueType = typename std::decay<decltype(*std::declval<I>())>::type;
    using ReferenceType = ValueType&;
    
private:
    
    template <typename X>
    using DifferenceTypeImpl = decltype(std::declval<X>() - std::declval<X>());
    
public:
    
    using DifferenceType = DetectedType<DifferenceTypeImpl, I>;
    
private:
    
    template <typename T>
    using IteratorConcept = VoidType<
        decltype(*std::declval<T>()),
        decltype(std::declval<T>() == std::declval<T>()),
        decltype(std::declval<T>() != std::declval<T>())>;
    template <typename T>
    using InputIteratorConcept = VoidType<
        IteratorConcept<T>,
        decltype(++std::declval<T&>())>;
    template <typename T>
    using ForwardIteratorConcept = VoidType<
        InputIteratorConcept<T>,
        decltype(std::declval<T&>()++)>;
    template <typename T>
    using BidirectionalIteratorConcept = VoidType<
        ForwardIteratorConcept<T>,
        decltype(--std::declval<T&>()),
        decltype(std::declval<T&>()--)>;
    template <typename T>
    using RandomAccessIteratorConcept = VoidType<
        BidirectionalIteratorConcept<T>,
        decltype(std::declval<T&>() += std::declval<DifferenceType>()),
        decltype(std::declval<T&>() -= std::declval<DifferenceType>()),
        decltype(std::declval<T>() + std::declval<DifferenceType>()),
        decltype(std::declval<DifferenceType>() + std::declval<T>()),
        decltype(std::declval<T>() - std::declval<DifferenceType>()),
        decltype(std::declval<T>() - std::declval<T>()),
        decltype(std::declval<T>()[std::declval<DifferenceType>()]),
        decltype(std::declval<T>() < std::declval<T>()),
        decltype(std::declval<T>() > std::declval<T>()),
        decltype(std::declval<T>() <= std::declval<T>()),
        decltype(std::declval<T>() >= std::declval<T>())>;
    
public:
    
    static constexpr bool IS_ITERATOR = IsDetected<IteratorConcept, I>::value;
    static constexpr bool IS_INPUT_ITERATOR = IsDetected<InputIteratorConcept, I>::value;
    static constexpr bool IS_FORWARD_ITERATOR = IsDetected<ForwardIteratorConcept, I>::value;
    static constexpr bool IS_BIDIRECTIONAL_ITERATOR = IsDetected<BidirectionalIteratorConcept, I>::value;
    static constexpr bool IS_RANDOM_ACCESS_ITERATOR = IsDetected<RandomAccessIteratorConcept, I>::value;
    
};


template <typename I>
class ReverseIterator {
    
private:
    
    using Traits = IteratorTraits<I>;
    using ValueType = typename Traits::ValueType;
    using DiffType = typename Traits::DifferenceType;
    
    static_assert(Traits::IS_BIDIRECTIONAL_ITERATOR, "Iterator must be bidirectional");
    
    template <typename X>
    using EnableIfRandomAccessIterator = typename std::enable_if<Traits::IS_RANDOM_ACCESS_ITERATOR, X>::type;
    
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

}
}
}


#endif
