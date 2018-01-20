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

#ifndef CATS_CORECAT_UTIL_FUNCTION_HPP
#define CATS_CORECAT_UTIL_FUNCTION_HPP


#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include "Sequence.hpp"

namespace Cats {
namespace Corecat {
inline namespace Util {

namespace Impl {

template <class T>
struct IsReferenceWrapper : std::false_type {};
template <class T>
struct IsReferenceWrapper<std::reference_wrapper<T>> : std::true_type {};

#define INVOKE_IMPL_EXPR std::forward<F>(f)(std::forward<Arg>(arg)...)
template <typename F, typename... Arg>
auto invokeImpl(F&& f, Arg&&... arg) noexcept(noexcept(INVOKE_IMPL_EXPR)) -> decltype(INVOKE_IMPL_EXPR) { return INVOKE_IMPL_EXPR; }
#undef INVOKE_IMPL_EXPR

#define INVOKE_IMPL_MEMBER_FUNCTION(cond, expr) \
    template <typename F, typename C, typename T, typename = typename std::enable_if<std::is_function<F>::value && (cond)>::type, typename... Arg> \
    auto invokeImpl(F C::* f, T&& t, Arg&&... arg) noexcept(noexcept(expr)) -> decltype(expr) { return expr; }
INVOKE_IMPL_MEMBER_FUNCTION(
    (std::is_base_of<C, typename std::decay<T>::type>::value),
    (std::forward<T>(t).*f)(std::forward<Arg>(arg)...));
INVOKE_IMPL_MEMBER_FUNCTION(
    (IsReferenceWrapper<typename std::decay<T>::type>::value),
    (t.get().*f)(std::forward<Arg>(arg)...));
INVOKE_IMPL_MEMBER_FUNCTION(
    (!std::is_base_of<C, typename std::decay<T>::type>::value && !IsReferenceWrapper<typename std::decay<T>::type>::value),
    ((*std::forward<T>(t)).*f)(std::forward<Arg>(arg)...));
#undef INVOKE_IMPL_MEMBER_FUNCTION

#define INVOKE_IMPL_MEMBER_OBJECT(cond, expr) \
    template <typename F, typename C, typename T, typename = typename std::enable_if<!std::is_function<F>::value && (cond)>::type> \
    auto invokeImpl(F C::* f, T&& t) noexcept(noexcept(expr)) -> decltype(expr) { return expr; }
INVOKE_IMPL_MEMBER_OBJECT(
    (std::is_base_of<C, typename std::decay<T>::type>::value),
    std::forward<T>(t).*f);
INVOKE_IMPL_MEMBER_OBJECT(
    (IsReferenceWrapper<typename std::decay<T>::type>::value),
    t.get().*f);
INVOKE_IMPL_MEMBER_OBJECT(
    (!std::is_base_of<C, typename std::decay<T>::type>::value && !IsReferenceWrapper<typename std::decay<T>::type>::value),
    (*std::forward<T>(t)).*f);
#undef INVOKE_IMPL_MEMBER_OBJECT

}

#define INVOKE_EXPR Impl::invokeImpl(std::forward<F>(f), std::forward<Arg>(arg)...)
template <typename F, typename... Arg>
auto invoke(F&& f, Arg&&... arg) noexcept(noexcept(INVOKE_EXPR)) -> decltype(INVOKE_EXPR) { return INVOKE_EXPR; }
#undef INVOKE_EXPR

namespace Impl {

template <typename, typename, typename...>
struct InvokeResultImpl {};
template <typename F, typename... Arg>
struct InvokeResultImpl<decltype(void(invoke(std::declval<F>(), std::declval<Arg>()...))), F, Arg...> {
    
    using Type = decltype(invoke(std::declval<F>(), std::declval<Arg>()...));
    
};

}
template <typename F, typename... Arg>
using InvokeResult = Impl::InvokeResultImpl<F, Arg...>;


namespace Impl {

#define APPLY_IMPL_EXPR invoke(std::forward<F>(f), std::get<I>(std::forward<T>(t))...)
template <typename F, typename T, std::size_t... I>
auto applyImpl(F&& f, T&& t, Sequence<std::size_t, I...>) noexcept(noexcept(APPLY_IMPL_EXPR)) -> decltype(APPLY_IMPL_EXPR) { return APPLY_IMPL_EXPR; }
#undef APPLY_IMPL_EXPR

}

#define APPLY_EXPR \
    Impl::applyImpl(std::forward<F>(f), std::forward<T>(t), \
        IndexSequence<std::size_t, 0, std::tuple_size<typename std::remove_reference<T>::type>::value - 1>())
template <typename F, typename T>
auto apply(F&& f, T&& t) noexcept(noexcept(APPLY_EXPR)) -> decltype(APPLY_EXPR) { return APPLY_EXPR; }
#undef APPLY_EXPR

}
}
}


#endif
