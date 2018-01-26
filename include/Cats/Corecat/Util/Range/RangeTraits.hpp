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

#ifndef CATS_CORECAT_UTIL_RANGE_RANGETRAITS_HPP
#define CATS_CORECAT_UTIL_RANGE_RANGETRAITS_HPP


#include <iterator>
#include <type_traits>
#include <utility>

#include "../Detector.hpp"
#include "../Iterator.hpp"


namespace Cats {
namespace Corecat {
inline namespace Util {

template <typename R>
using RangeConcept = VoidType<
    std::enable_if_t<std::is_same<decltype(std::end(std::declval<R>())), decltype(std::begin(std::declval<R>()))>::value>,
    InputIteratorConcept<decltype(std::begin(std::declval<R>()))>>;
template <typename R>
using SizedRangeConcept = VoidType<
    RangeConcept<R>,
    RandomAccessIteratorConcept<decltype(std::begin(std::declval<R>()))>>;

template <typename R>
constexpr bool IsRange = IsDetected<RangeConcept, R>;
template <typename R>
constexpr bool IsSizedRange = IsDetected<SizedRangeConcept, R>;


template <typename R>
struct RangeTraits {
    
    using IteratorType = decltype(std::begin(std::declval<R>()));
    using ValueType = typename std::iterator_traits<IteratorType>::value_type;
    using ReferenceType = typename std::iterator_traits<IteratorType>::reference;
    using DifferenceType = typename std::iterator_traits<IteratorType>::difference_type;
    
    static constexpr bool IS_SIZED_RANGE = IsSizedRange<R>;
    
};

}
}
}


#endif
