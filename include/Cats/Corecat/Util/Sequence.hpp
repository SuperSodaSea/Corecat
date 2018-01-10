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

#ifndef CATS_CORECAT_UTIL_SEQUENCE_HPP
#define CATS_CORECAT_UTIL_SEQUENCE_HPP


#include <cstdlib>


namespace Cats {
namespace Corecat {
inline namespace Util {

template <typename T, T... V>
struct Sequence {
    
    static constexpr std::size_t size() { return sizeof...(V); }
    
};


template <typename S>
struct GetSequence;
template <typename T, T... V>
struct GetSequence<Sequence<T, V...>> {
    
private:
    static constexpr T TABLE[] = {V...};
public:
    static constexpr std::size_t get(std::size_t index) { return TABLE[index]; }
    
};


namespace Impl {

template <typename S, std::size_t I1, std::size_t I2, typename = void>
struct ContainSequenceImpl;
template <typename T, T... V, std::size_t I1, std::size_t I2>
struct ContainSequenceImpl<Sequence<T, V...>, I1, I2, typename std::enable_if<sizeof...(V) == 0>::type> {
    
    static constexpr bool get(T /*t*/) { return false; }
    
};
template <typename T, T... V, std::size_t I1, std::size_t I2>
struct ContainSequenceImpl<Sequence<T, V...>, I1, I2, typename std::enable_if<I1 == I2>::type> {
    
    static constexpr bool get(T t) { return t == GetSequence<Sequence<T, V...>>::get(I1); }
    
};
template <typename T, T... V, std::size_t I1, std::size_t I2>
struct ContainSequenceImpl<Sequence<T, V...>, I1, I2, typename std::enable_if<(I1 < I2)>::type> {
    
    static constexpr bool get(T t) {
        
        return ContainSequenceImpl<Sequence<T, V...>, I1, (I1 + I2) / 2>::get(t)
            || ContainSequenceImpl<Sequence<T, V...>, (I1 + I2) / 2 + 1, I2>::get(t);
        
    }
    
};

}
template <typename S>
using ContainSequence = typename Impl::ContainSequenceImpl<S, 0, S::size() - 1>;


namespace Impl {

template <typename S1, typename S2>
struct ConcatSequenceImpl;
template <typename T, T... V1, T... V2>
struct ConcatSequenceImpl<Sequence<T, V1...>, Sequence<T, V2...>> { using Type = Sequence<T, V1..., V2...>; };

}
template <typename S1, typename S2>
using ConcatSequence = typename Impl::ConcatSequenceImpl<S1, S2>::Type;


namespace Impl {

template <typename T, T Begin, T End, typename = void>
struct IndexSequenceImpl;
template <typename T, T Begin, T End>
struct IndexSequenceImpl<T, Begin, End, typename std::enable_if<Begin == End>::type> {
    
    using Type = Sequence<T, Begin>;
    
};
template <typename T, T Begin, T End>
struct IndexSequenceImpl<T, Begin, End, typename std::enable_if<(Begin < End)>::type> {
    
    using Type = ConcatSequence<typename IndexSequenceImpl<T, Begin, (Begin + End) / 2>::Type,
        typename IndexSequenceImpl<T, (Begin + End) / 2 + 1, End>::Type>;
    
};

}
template <typename T, T Begin, T End>
using IndexSequence = typename Impl::IndexSequenceImpl<T, Begin, End>::Type;


namespace Impl {
    
template <typename F, typename S>
struct MapperSequenceImpl;
template <typename F, typename T, T... V>
struct MapperSequenceImpl<F, Sequence<T, V...>> {
    
    using Type = Sequence<typename std::result_of<decltype(&F::get)(T)>::type, F::get(V)...>;
    
};

}
template <typename F, typename S>
using MapperSequence = typename Impl::MapperSequenceImpl<F, S>::Type;


namespace {

template <typename S>
struct SequenceTable;
template <typename T, T... V>
struct SequenceTable<Sequence<T, V...>> {
    
    static const T TABLE[];
    static T get(std::size_t t) { return TABLE[t]; }
    
};
template <typename T, T... V>
const T SequenceTable<Sequence<T, V...>>::TABLE[] = { V... };

}

}
}
}


#endif
