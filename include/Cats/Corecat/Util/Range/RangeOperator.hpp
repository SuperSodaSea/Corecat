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

#ifndef CATS_CORECAT_UTIL_RANGE_RANGEOPERATOR_HPP
#define CATS_CORECAT_UTIL_RANGE_RANGEOPERATOR_HPP


#include <tuple>
#include <utility>

#include "../Function.hpp"


namespace Cats {
namespace Corecat {
inline namespace Util {

template <typename F, typename... Arg>
class RangeOperator {
    
public:
    
    const F& f;
    std::tuple<Arg...> arg;
    
public:
    
    template <typename... Arg1>
    RangeOperator(const F& f_, Arg1&&... arg_) : f(f_), arg(std::forward<Arg1>(arg_)...) {}
    
};

template <typename R, typename F, typename... Arg>
inline decltype(auto) operator |(R&& a, RangeOperator<F, Arg...>&& b) {
    
    return apply(b.f, std::tuple_cat(std::tuple<R&&>(std::forward<R>(a)), b.arg));
    
}

}
}
}


#endif
