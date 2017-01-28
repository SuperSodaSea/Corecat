/*
 *
 * MIT License
 *
 * Copyright (c) 2016-2017 The Cats Project
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

#ifndef CATS_CORECAT_PROMISE_HPP
#define CATS_CORECAT_PROMISE_HPP


#include <deque>
#include <functional>
#include <memory>
#include <type_traits>

#include "ExceptionWrapper.hpp"


namespace Cats {
namespace Corecat {

// Inspired by https://github.com/facebook/folly/blob/master/folly/futures/Future.h
template <typename T>
class Promise {
    
public:
    
    enum class State { Pending, Resolved, Rejected };
    
public:
    
    struct Void {};
    template <typename U>
    using NonVoidType = typename std::conditional<std::is_void<U>::value, Void, U>::type;
    
    template <typename... U>
    struct VoidTypeImpl { using Type = void; };
    template <typename... U>
    using VoidType = typename VoidTypeImpl<U...>::Type;
    
    template <typename U>
    using EnableIfVoid = typename std::enable_if<std::is_void<U>::value>::type;
    template <typename U>
    using EnableIfNotVoid = typename std::enable_if<!std::is_void<U>::value>::type;
    
    template <typename F, typename... Arg>
    struct ResultTypeImpl { using Type = decltype(std::declval<F>()(std::declval<Arg>()...)); };
    template <typename F>
    struct ResultTypeImpl<F, void> { using Type = decltype(std::declval<F>()()); };
    template <typename F, typename... Arg>
    using ResultType = typename ResultTypeImpl<F, Arg...>::Type;
    
    template <typename F, typename Args, typename = void>
    struct IsCallableImpl : public std::false_type {};
    template <typename F, typename... Arg>
    struct IsCallableImpl<F, std::tuple<Arg...>, VoidType<decltype(std::declval<F>()(std::declval<Arg>()...))>> : public std::true_type {};
    template <typename F, typename... Arg>
    struct IsCallable : public IsCallableImpl<F, std::tuple<Arg...>> {};
    template <typename F>
    struct IsCallable<F, void> : public IsCallableImpl<F, std::tuple<>> {};
    
    template <typename F, typename U, typename V = NonVoidType<U>>
    using ArgumentType =
        typename std::conditional<std::is_void<U>::value, void,
        typename std::conditional<IsCallable<F, V>::value, V,
        typename std::conditional<IsCallable<F, const V&>::value, const V&,
        void>::type>::type>::type;
    
    template <typename U>
    struct Inner { static constexpr bool value = false; using Type = U; };
    template <typename U>
    struct Inner<Promise<U>> { static constexpr bool value = true; using Type = U; };
    
    template <typename U>
    using EnableIfNotInner = typename std::enable_if<!Inner<U>::value>::type;
    template <typename U>
    using EnableIfInner = typename std::enable_if<Inner<U>::value>::type;
    
private:
    
    class Impl {
        
    private:
        
        State state = State::Pending;
        std::deque<std::function<void()>> resolvedQueue;
        std::deque<std::function<void()>> rejectedQueue;
        NonVoidType<T> result;
        ExceptionWrapper exception;
        
    private:
        
        template <typename F, typename Arg = ArgumentType<F, T>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        void thenImpl(F&& resolved, const Promise<In>& promise, EnableIfVoid<Arg>* = 0, EnableIfVoid<Res>* = 0) {
            
            resolved(); promise.resolve();
            
        }
        template <typename F, typename Arg = ArgumentType<F, T>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        void thenImpl(F&& resolved, const Promise<In>& promise, EnableIfNotVoid<Arg>* = 0, EnableIfVoid<Res>* = 0) {
            
            resolved(result); promise.resolve();
            
        }
        template <typename F, typename Arg = ArgumentType<F, T>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        void thenImpl(F&& resolved, const Promise<In>& promise, EnableIfVoid<Arg>* = 0, EnableIfNotVoid<Res>* = 0, EnableIfNotInner<Res>* = 0) {
            
            promise.resolve(resolved());
            
        }
        template <typename F, typename Arg = ArgumentType<F, T>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        void thenImpl(F&& resolved, const Promise<In>& promise, EnableIfNotVoid<Arg>* = 0, EnableIfNotVoid<Res>* = 0, EnableIfNotInner<Res>* = 0) {
            
            promise.resolve(resolved(result));
            
        }
        template <typename F, typename Arg = ArgumentType<F, T>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        void thenImpl(F&& resolved, const Promise<In>& promise, EnableIfVoid<Arg>* = 0, EnableIfNotVoid<Res>* = 0, EnableIfInner<Res>* = 0, EnableIfVoid<In>* = 0) {
            
            Res res = resolved();
            res.then([=]() { promise.resolve(); });
            res.fail([=](const ExceptionWrapper& ew) { promise.reject(ew); });
            
        }
        template <typename F, typename Arg = ArgumentType<F, T>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        void thenImpl(F&& resolved, const Promise<In>& promise, EnableIfNotVoid<Arg>* = 0, EnableIfNotVoid<Res>* = 0, EnableIfInner<Res>* = 0, EnableIfVoid<In>* = 0) {
            
            Res res = resolved(result);
            res.then([=]() { promise.resolve(); });
            res.fail([=](const ExceptionWrapper& ew) { promise.reject(ew); });
            
        }
        template <typename F, typename Arg = ArgumentType<F, T>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        void thenImpl(F&& resolved, const Promise<In>& promise, EnableIfVoid<Arg>* = 0, EnableIfNotVoid<Res>* = 0, EnableIfInner<Res>* = 0, EnableIfNotVoid<In>* = 0) {
            
            Res res = resolved();
            res.then([=](const In& in) { promise.resolve(in); });
            res.fail([=](const ExceptionWrapper& ew) { promise.reject(ew); });
            
        }
        template <typename F, typename Arg = ArgumentType<F, T>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        void thenImpl(F&& resolved, const Promise<In>& promise, EnableIfNotVoid<Arg>* = 0, EnableIfNotVoid<Res>* = 0, EnableIfInner<Res>* = 0, EnableIfNotVoid<In>* = 0) {
            
            Res res = resolved(result);
            res.then([=](const In& in) { promise.resolve(in); });
            res.fail([=](const ExceptionWrapper& ew) { promise.reject(ew); });
            
        }
        
        template <typename F, typename Arg = ArgumentType<F, ExceptionWrapper>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        void failImpl(F&& rejected, const Promise<In>& promise, EnableIfVoid<Arg>* = 0, EnableIfVoid<Res>* = 0) {
            
            rejected(); promise.resolve();
            
        }
        template <typename F, typename Arg = ArgumentType<F, ExceptionWrapper>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        void failImpl(F&& rejected, const Promise<In>& promise, EnableIfNotVoid<Arg>* = 0, EnableIfVoid<Res>* = 0) {
            
            rejected(exception); promise.resolve();
            
        }
        template <typename F, typename Arg = ArgumentType<F, ExceptionWrapper>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        void failImpl(F&& rejected, const Promise<In>& promise, EnableIfVoid<Arg>* = 0, EnableIfNotVoid<Res>* = 0, EnableIfNotInner<Res>* = 0) {
            
            promise.resolve(rejected());
            
        }
        template <typename F, typename Arg = ArgumentType<F, ExceptionWrapper>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        void failImpl(F&& rejected, const Promise<In>& promise, EnableIfNotVoid<Arg>* = 0, EnableIfNotVoid<Res>* = 0, EnableIfNotInner<Res>* = 0) {
            
            promise.resolve(rejected(exception));
            
        }
        template <typename F, typename Arg = ArgumentType<F, ExceptionWrapper>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        void failImpl(F&& rejected, const Promise<In>& promise, EnableIfVoid<Arg>* = 0, EnableIfNotVoid<Res>* = 0, EnableIfInner<Res>* = 0, EnableIfVoid<In>* = 0) {
            
            Res res = rejected();
            res.then([=]() { promise.resolve(); });
            res.fail([=](const ExceptionWrapper& ew) { promise.reject(ew); });
            
        }
        template <typename F, typename Arg = ArgumentType<F, ExceptionWrapper>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        void failImpl(F&& rejected, const Promise<In>& promise, EnableIfNotVoid<Arg>* = 0, EnableIfNotVoid<Res>* = 0, EnableIfInner<Res>* = 0, EnableIfVoid<In>* = 0) {
            
            Res res = rejected(exception);
            res.then([=]() { promise.resolve(); });
            res.fail([=](const ExceptionWrapper& ew) { promise.reject(ew); });
            
        }
        template <typename F, typename Arg = ArgumentType<F, ExceptionWrapper>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        void failImpl(F&& rejected, const Promise<In>& promise, EnableIfVoid<Arg>* = 0, EnableIfNotVoid<Res>* = 0, EnableIfInner<Res>* = 0, EnableIfNotVoid<In>* = 0) {
            
            Res res = rejected();
            res.then([=](const In& in) { promise.resolve(in); });
            res.fail([=](const ExceptionWrapper& ew) { promise.reject(ew); });
            
        }
        template <typename F, typename Arg = ArgumentType<F, ExceptionWrapper>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        void failImpl(F&& rejected, const Promise<In>& promise, EnableIfNotVoid<Arg>* = 0, EnableIfNotVoid<Res>* = 0, EnableIfInner<Res>* = 0, EnableIfNotVoid<In>* = 0) {
            
            Res res = rejected(exception);
            res.then([=](const In& in) { promise.resolve(in); });
            res.fail([=](const ExceptionWrapper& ew) { promise.reject(ew); });
            
        }
        
    public:
        
        Impl(const Impl& src) = delete;
        ~Impl() {}
        
        Impl() = default;
        
        Impl& operator =(const Impl& src) = delete;
        
        State getState() { return state; }
        
        template <typename U = T, typename = EnableIfVoid<U>>
        void resolve() {
            
            if(state == State::Pending) {
                
                state = State::Resolved;
                for(auto& x : resolvedQueue) x();
                resolvedQueue.clear();
                rejectedQueue.clear();
                
            }
            
        }
        template <typename U = T, typename = EnableIfNotVoid<U>>
        void resolve(const U& u) {
            
            if(state == State::Pending) {
                
                state = State::Resolved;
                result = u;
                for(auto& x : resolvedQueue) x();
                resolvedQueue.clear();
                rejectedQueue.clear();
                
            }
            
        }
        void reject(const ExceptionWrapper& e) {
            
            if(state == State::Pending) {
                
                state = State::Rejected;
                exception = e;
                for(auto& x : rejectedQueue) x();
                resolvedQueue.clear();
                rejectedQueue.clear();
                
            }
            
        }
        
        template <typename F, typename Arg = ArgumentType<F, T>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        Promise<In> then(F&& resolved) {
            
            Promise<In> promise;
            auto cb = [=]() {
                
                try { thenImpl(resolved, promise); }
                catch(...) { promise.reject(ExceptionWrapper::current()); }
                
            };
            switch(state) {
            case State::Pending: resolvedQueue.emplace_back(cb); break;
            case State::Resolved: cb(); break;
            default: break;
            }
            return promise;
            
        }
        
        template <typename F, typename Arg = ArgumentType<F, ExceptionWrapper>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
        Promise<In> fail(F&& rejected) {
            
            Promise<In> promise;
            auto cb = [=]() {
                
                try { failImpl(rejected, promise); }
                catch(...) { promise.reject(ExceptionWrapper::current()); }
                
            };
            switch(state) {
            case State::Pending: rejectedQueue.emplace_back(cb); break;
            case State::Rejected: cb(); break;
            default: break;
            }
            return promise;
            
        }
        
    };
    
public:
    
    std::shared_ptr<Impl> impl;
    
public:
    
    Promise(const Promise& src) : impl(src.impl) {}
    
    Promise() : impl(std::make_shared<Impl>()) {}
    
    Promise& operator =(const Promise& src) { impl = src.impl; return *this; }
    template <typename U = T, typename = typename std::enable_if<std::is_same<U, void>::value>::type>
    void resolve() const { impl->resolve(); }
    template <typename U = T, typename = typename std::enable_if<!std::is_same<U, void>::value>::type>
    void resolve(const U& t) const { impl->resolve(std::move(t)); }
    void reject(const ExceptionWrapper& t) const { impl->reject(std::move(t)); }
    
    template <typename F, typename Arg = ArgumentType<F, T>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
    Promise<In> then(F&& resolved) const { return impl->then(std::forward<F>(resolved)); }
    template <typename F, typename Arg = ArgumentType<F, ExceptionWrapper>, typename Res = ResultType<F, Arg>, typename In = typename Inner<Res>::Type>
    Promise<In> fail(F&& rejected) const { return impl->fail(std::forward<F>(rejected)); }
    
public:
    
    template <typename F>
    static Promise create(F&& f) {
        
        Promise promise;
        f(promise);
        return promise;
        
    }
    
};

}
}


#endif
