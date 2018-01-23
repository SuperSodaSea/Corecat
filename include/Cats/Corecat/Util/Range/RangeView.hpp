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

#ifndef CATS_CORECAT_UTIL_RANGE_RANGEVIEW_HPP
#define CATS_CORECAT_UTIL_RANGE_RANGEVIEW_HPP


#include <initializer_list>
#include <iterator>
#include <utility>


namespace Cats {
namespace Corecat {
inline namespace Util {

template <typename I>
class RangeView {
    
public:
    
    using Iterator = I;
    
private:
    
    I b, e;
    
public:
    
    RangeView(I b_, I e_) : b(std::move(b_)), e(std::move(e_)) {}
    
    I begin() const noexcept { return b; }
    I end() const noexcept { return e; }
    
};

template <typename I>
inline RangeView<I> rangeView(I b, I e) { return RangeView<I>(std::move(b), std::move(e)); }
template <typename T>
inline auto rangeView(T& t) -> decltype(rangeView(std::begin(t), std::end(t))) { return rangeView(std::begin(t), std::end(t)); }
template <typename T>
inline auto rangeView(std::initializer_list<T> t) -> decltype(rangeView(std::begin(t), std::end(t))) { return rangeView(std::begin(t), std::end(t)); }

}
}
}


#endif
