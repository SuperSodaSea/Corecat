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

#ifndef CATS_CORECAT_UTIL_RANGE_SKIPRANGE_HPP
#define CATS_CORECAT_UTIL_RANGE_SKIPRANGE_HPP


#include <cstddef>

#include <algorithm>

#include "RangeOperator.hpp"
#include "RangeTraits.hpp"


namespace Cats {
namespace Corecat {
inline namespace Util {

template <typename R>
class SkipRange {
    
public:
    
    using Iterator = typename RangeTraits<R>::IteratorType;
    using DifferenceType = typename RangeTraits<R>::DifferenceType;
    
private:
    
    R r;
    DifferenceType count;
    
public:
    
    SkipRange(R r_, DifferenceType count_) : r(std::move(r_)), count(count_) {}
    
    Iterator begin() const { return advanceUntil(std::begin(r), std::end(r), count); }
    Iterator end() const { return std::end(r); }
    
};


namespace Impl {

struct SkipRangeFunc {
    
    template <typename T>
    RangeOperator<SkipRangeFunc, T> operator ()(T count) const { return {*this, count}; }
    template <typename R>
    SkipRange<std::decay_t<R>> operator ()(R&& r, typename RangeTraits<R>::DifferenceType count) const { return {std::forward<R>(r), count}; }
    
};

}
namespace { constexpr Impl::SkipRangeFunc skip; }

}
}
}


#endif
