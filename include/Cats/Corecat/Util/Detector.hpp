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

#ifndef CATS_CORECAT_UTIL_DETECTOR_HPP
#define CATS_CORECAT_UTIL_DETECTOR_HPP


#include <type_traits>

#include "VoidType.hpp"


namespace Cats {
namespace Corecat {
inline namespace Util {

struct None {
    
    None() = delete;
    None(const None& src) = delete;
    ~None() = delete;
    None& operator=(const None& src) = delete;
    
};

namespace Impl {

template <typename T, typename, template <typename...> class Op, typename... Arg>
struct DetectorImpl {
    static constexpr bool VALUE = false;
    using Type = T;
};
template <typename T, template <typename...> class Op, typename... Arg>
struct DetectorImpl<T, VoidType<Op<Arg...>>, Op, Arg...> {
    static constexpr bool VALUE = true;
    using Type = Op<Arg...>;
};

}
template <typename T, template <typename...> class Op, typename... Arg>
using Detector = Impl::DetectorImpl<T, void, Op, Arg...>;
template <template <typename...> class Op, typename... Arg>
constexpr bool IsDetected = Detector<None, Op, Arg...>::VALUE;
template <template <typename...> class Op, typename... Arg>
using DetectedType = typename Detector<None, Op, Arg...>::Type;
template <typename T, template <typename...> class Op, typename... Arg>
using DetectedOrType = typename Detector<T, Op, Arg...>::Type;
template <typename T, template <typename...> class Op, typename... Arg>
constexpr bool IsDetectedExactly = std::is_same<DetectedType<Op, Arg...>, T>::value;
template <typename T, template <typename...> class Op, typename... Arg>
constexpr bool IsDetectedConvertible = std::is_convertible<DetectedType<Op, Arg...>, T>::value;

}
}
}


#endif
