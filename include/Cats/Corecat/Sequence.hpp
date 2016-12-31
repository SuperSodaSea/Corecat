/*
 *
 * MIT License
 *
 * Copyright (c) 2016 The Cats Project
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

#ifndef CATS_CORECAT_SEQUENCE_HPP
#define CATS_CORECAT_SEQUENCE_HPP


#include <cstdlib>


namespace Cats {
namespace Corecat {
namespace Sequence {

template <typename T, T... V>
struct Base {
    
    static constexpr T table[] = { V... };
    
    static constexpr std::size_t size() { return sizeof...(V); }
    static constexpr T get(std::size_t index) { return table[index]; }
    
};

namespace Impl {

template <typename S, std::size_t I1, std::size_t I2, typename = void>
struct Contain;
template <typename T, T... V, std::size_t I1, std::size_t I2>
struct Contain<Base<T, V...>, I1, I2, typename std::enable_if<sizeof...(V) == 0>::type> {
    
    static constexpr bool get(T /*t*/) { return false; }
    
};
template <typename T, T... V, std::size_t I1, std::size_t I2>
struct Contain<Base<T, V...>, I1, I2, typename std::enable_if<I1 == I2>::type> {
    
    static constexpr bool get(T t) { return t == Base<T, V...>::get(I1); }
    
};
template <typename T, T... V, std::size_t I1, std::size_t I2>
struct Contain<Base<T, V...>, I1, I2, typename std::enable_if<(I1 < I2)>::type> {
    
    static constexpr bool get(T t) {
        
        return Contain<Base<T, V...>, I1, (I1 + I2) / 2>::get(t) || Contain<Base<T, V...>, (I1 + I2) / 2 + 1, I2>::get(t);
        
    }
    
};

}
template <typename S>
using Contain = typename Impl::Contain<S, 0, S::size() - 1>;

namespace Impl {

template <typename S1, typename S2>
struct Concat;
template <typename T, T... V1, T... V2>
struct Concat<Base<T, V1...>, Base<T, V2...>> { using Type = Base<T, V1..., V2...>; };

}
template <typename S1, typename S2>
using Concat = typename Impl::Concat<S1, S2>::Type;

namespace Impl {

template <typename T, T Begin, T End, typename = void>
struct Index;
template <typename T, T Begin, T End>
struct Index<T, Begin, End, typename std::enable_if<Begin == End>::type> {
    
    using Type = Base<T, Begin>;
    
};
template <typename T, T Begin, T End>
struct Index<T, Begin, End, typename std::enable_if<(Begin < End)>::type> {
    
    using Type = typename Concat<typename Index<T, Begin, (Begin + End) / 2>::Type,
        typename Index<T, (Begin + End) / 2 + 1, End>::Type>::Type;
    
};

}
template <typename T, T Begin, T End>
using Index = typename Impl::Index<T, Begin, End>::Type;

namespace Impl {

template <typename F, typename S>
struct Mapper;
template <typename F, typename T, T... V>
struct Mapper<F, Base<T, V...>> {
    
    using Type = Base<typename std::result_of<decltype(&F::get)(T)>::type, F::get(V)...>;
    
};

}
template <typename F, typename S>
using Mapper = typename Impl::Mapper<F, S>::Type;

template <typename S>
struct Table;
template <typename T, T... V>
struct Table<Base<T, V...>> {
    
    static const T table[];
    static T get(std::size_t t) { return table[t]; }
    
};
template <typename T, T... V>
const T Table<Base<T, V...>>::table[] = { V... };

}
}
}


#endif
