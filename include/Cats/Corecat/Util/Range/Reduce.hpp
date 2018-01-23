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

#ifndef CATS_CORECAT_UTIL_RANGE_REDUCE_HPP
#define CATS_CORECAT_UTIL_RANGE_REDUCE_HPP


#include <type_traits>
#include <utility>

#include "RangeOperator.hpp"
#include "RangeTraits.hpp"


namespace Cats {
namespace Corecat {
inline namespace Util {

namespace Impl {

struct ReduceFunc {
    
private:
    
    struct Plus {
        
        template <typename T, typename U>
        auto operator ()(T&& t, U&& u) -> decltype(std::forward<T>(t) + std::forward<U>(u)) { return std::forward<T>(t) + std::forward<U>(u); }
        
    };
    
public:
    
    RangeOperator<ReduceFunc> operator ()() const { return {*this}; }
    template <typename T, typename = typename std::enable_if<!IsRange<typename std::remove_reference<T>::type>::value>::type>
    auto operator ()(T&& t) const -> RangeOperator<ReduceFunc, T> {
        
        return {*this, std::forward<T>(t)};
        
    }
    template <typename F, typename T, typename = typename std::enable_if<!IsRange<typename std::remove_reference<F>::type>::value>::type>
    auto operator ()(F&& f, T&& t) const -> RangeOperator<ReduceFunc, F, T> {
        
        return {*this, std::forward<F>(f), std::forward<T>(t)};
        
    }
    
    template <typename R, typename = typename std::enable_if<IsRange<typename std::remove_reference<R>::type>::value>::type>
    auto operator ()(R&& r) const -> decltype((*this)(std::forward<R>(r), typename RangeTraits<typename std::remove_reference<R>::type>::ValueType())) {
        
        return (*this)(std::forward<R>(r), typename RangeTraits<typename std::remove_reference<R>::type>::ValueType());
        
    }
    template <typename R, typename T, typename = typename std::enable_if<IsRange<typename std::remove_reference<R>::type>::value>::type>
    auto operator ()(R&& r, T&& t) const -> decltype((*this)(std::forward<R>(r), Plus(), std::forward<T>(t))) {
        
        return (*this)(std::forward<R>(r), Plus(), std::forward<T>(t));
        
    }
    template <typename R, typename F, typename T>
    T operator ()(R&& r, F&& f, T t) const { for(auto&& x : r) t = f(t, x); return t; }
    
};

}
namespace { constexpr Impl::ReduceFunc reduce; }

}
}
}


#endif
