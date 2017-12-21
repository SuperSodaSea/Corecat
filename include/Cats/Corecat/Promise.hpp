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
#include <mutex>
#include <type_traits>

#include "Util/ExceptionWrapper.hpp"


namespace Cats {
namespace Corecat {

// Inspired by https://github.com/facebook/folly/blob/master/folly/futures/Future.h

template <typename T>
class Promise;

namespace Impl {

template <typename T>
class PromiseImpl : public std::enable_shared_from_this<PromiseImpl<T>> {
    
    using Util::ExceptionWrapper;
    
private:
    
    enum class State {Pending, Resolved, Rejected};
    
private:
    
    template <typename... U>
    struct VoidTypeImpl { using Type = void; };
    template <typename... U>
    using VoidType = typename VoidTypeImpl<U...>::Type;
    
    template <typename F, typename Args, typename = void>
    struct IsCallableImpl : public std::false_type {};
    template <typename F, typename... Arg>
    struct IsCallableImpl<F, std::tuple<Arg...>, VoidType<decltype(std::declval<F>()(std::declval<Arg>()...))>> : public std::true_type {};
    template <typename F, typename... Arg>
    struct IsCallable : public IsCallableImpl<F, std::tuple<Arg...>> {};
    template <typename F>
    struct IsCallable<F, void> : public IsCallableImpl<F, std::tuple<>> {};
    
    template <typename U>
    struct IsPromise : public std::false_type {};
    template <typename U>
    struct IsPromise<Promise<U>> : public std::true_type {};
    
    template <typename F, typename A>
    using ArgumentType = typename std::conditional<IsCallable<F, A>::value, A, void>::type;
    
    template <typename F, typename Arg>
    struct ReturnTypeImpl { using Type = decltype(std::declval<F>()(std::declval<Arg>())); };
    template <typename F>
    struct ReturnTypeImpl<F, void> { using Type = decltype(std::declval<F>()()); };
    template <typename F, typename Arg>
    using ReturnType = typename ReturnTypeImpl<F, Arg>::Type;
    
    template <typename U>
    struct ResultTypeImpl { using Type = U; };
    template <typename U>
    struct ResultTypeImpl<Promise<U>> { using Type = U; };
    template <typename U>
    using ResultType = typename ResultTypeImpl<U>::Type;
    
    template <typename U>
    using EnableIfVoid = typename std::enable_if<std::is_void<U>::value>::type;
    template <typename U>
    using EnableIfNotVoid = typename std::enable_if<!std::is_void<U>::value>::type;
    template <typename U>
    using EnableIfNotPromise = typename std::enable_if<!IsPromise<U>::value>::type;
    
    using Mutex = std::mutex;
    
private:
    
    Mutex mutex;
    State state = State::Pending;
    std::deque<std::function<void()>> resolvedQueue;
    std::deque<std::function<void()>> rejectedQueue;
    typename std::conditional<std::is_void<T>::value, char, T>::type result;
    ExceptionWrapper exception;
    
private:
    
    using std::enable_shared_from_this<PromiseImpl<T>>::shared_from_this;
    
    template <typename F, typename Arg = ArgumentType<F, T>, typename Ret = ReturnType<F, Arg>, typename Res = ResultType<Ret>>
    void thenImpl(F&& resolved, const Promise<Res>& promise, EnableIfVoid<Arg>* = 0, EnableIfVoid<Ret>* = 0) {
        
        resolved(); promise.resolve();
        
    }
    template <typename F, typename Arg = ArgumentType<F, T>, typename Ret = ReturnType<F, Arg>, typename Res = ResultType<Ret>>
    void thenImpl(F&& resolved, const Promise<Res>& promise, EnableIfVoid<Arg>* = 0, EnableIfNotVoid<Ret>* = 0) {
        
        promise.resolve(resolved());
        
    }
    template <typename F, typename Arg = ArgumentType<F, T>, typename Ret = ReturnType<F, Arg>, typename Res = ResultType<Ret>>
    void thenImpl(F&& resolved, const Promise<Res>& promise, EnableIfNotVoid<Arg>* = 0, EnableIfVoid<Ret>* = 0) {
        
        resolved(result); promise.resolve();
        
    }
    template <typename F, typename Arg = ArgumentType<F, T>, typename Ret = ReturnType<F, Arg>, typename Res = ResultType<Ret>>
    void thenImpl(F&& resolved, const Promise<Res>& promise, EnableIfNotVoid<Arg>* = 0, EnableIfNotVoid<Ret>* = 0) {
        
        promise.resolve(resolved(result));
        
    }
    
    template <typename F, typename Arg = ArgumentType<F, ExceptionWrapper>, typename Ret = ReturnType<F, Arg>, typename Res = ResultType<Ret>>
    void failImpl(F&& rejected, const Promise<Res>& promise, EnableIfVoid<Arg>* = 0, EnableIfVoid<Ret>* = 0) {
        
        rejected(); promise.resolve();
        
    }
    template <typename F, typename Arg = ArgumentType<F, ExceptionWrapper>, typename Ret = ReturnType<F, Arg>, typename Res = ResultType<Ret>>
    void failImpl(F&& rejected, const Promise<Res>& promise, EnableIfVoid<Arg>* = 0, EnableIfNotVoid<Ret>* = 0) {
        
        promise.resolve(rejected());
        
    }
    template <typename F, typename Arg = ArgumentType<F, ExceptionWrapper>, typename Ret = ReturnType<F, Arg>, typename Res = ResultType<Ret>>
    void failImpl(F&& rejected, const Promise<Res>& promise, EnableIfNotVoid<Arg>* = 0, EnableIfVoid<Ret>* = 0) {
        
        rejected(exception); promise.resolve();
        
    }
    template <typename F, typename Arg = ArgumentType<F, ExceptionWrapper>, typename Ret = ReturnType<F, Arg>, typename Res = ResultType<Ret>>
    void failImpl(F&& rejected, const Promise<Res>& promise, EnableIfNotVoid<Arg>* = 0, EnableIfNotVoid<Ret>* = 0) {
        
        promise.resolve(rejected(exception));
        
    }
    
public:
    
    PromiseImpl() = default;
    PromiseImpl(const PromiseImpl& src) = delete;
    ~PromiseImpl() = default;
    
    PromiseImpl& operator =(const PromiseImpl& src) = delete;
    
    template <typename U = T, typename = EnableIfVoid<U>>
    void resolve() {
        
        std::unique_lock<Mutex> lock(mutex);
        if(state == State::Pending) {
            
            state = State::Resolved;
            for(auto&& x : resolvedQueue) x();
            resolvedQueue.clear();
            rejectedQueue.clear();
            
        }
        
    }
    template <typename U = T, typename = EnableIfNotVoid<U>, typename = EnableIfNotPromise<typename std::decay<U>::type>>
    void resolve(U&& u) {
        
        std::unique_lock<Mutex> lock(mutex);
        if(state == State::Pending) {
            
            state = State::Resolved;
            result = std::forward<U>(u);
            for(auto&& x : resolvedQueue) x();
            resolvedQueue.clear();
            rejectedQueue.clear();
            
        }
        
    }
    template <typename U = T>
    void resolve(const Promise<T>& promise, EnableIfVoid<U>* = 0) {
        
        auto self = shared_from_this();
        promise.then([this, self]() { resolve(); });
        promise.fail([this, self](ExceptionWrapper e) { rejected(e); });
        
    }
    template <typename U = T>
    void resolve(const Promise<T>& promise, EnableIfNotVoid<U>* = 0) {
        
        auto self = shared_from_this();
        promise.then([this, self](T t) { resolve(t); });
        promise.fail([this, self](ExceptionWrapper e) { rejected(e); });
        
    }
    void rejected(const ExceptionWrapper& e) {
        
        std::unique_lock<Mutex> lock(mutex);
        if(state == State::Pending) {
            
            state = State::Rejected;
            exception = e;
            for(auto& x : rejectedQueue) x();
            resolvedQueue.clear();
            rejectedQueue.clear();
            
        }
        
    }
    template <typename F, typename Arg = ArgumentType<F, T>, typename Ret = ReturnType<F, Arg>, typename Res = ResultType<Ret>>
    Promise<Res> then(F&& resolved) {
        
        Promise<Res> promise;
        auto cb1 = [=]() {
            
            try { thenImpl(resolved, promise); }
            catch(...) { promise.reject(ExceptionWrapper::current()); }
            
        };
        auto cb2 = [=]() { promise.reject(exception); };
        std::unique_lock<Mutex> lock(mutex);
        switch(state) {
        case State::Pending: resolvedQueue.emplace_back(std::move(cb1)); rejectedQueue.emplace_back(std::move(cb2)); break;
        case State::Resolved: cb1(); break;
        case State::Rejected: cb2(); break;
        }
        return promise;
        
    }
    template <typename F, typename Arg = ArgumentType<F, ExceptionWrapper>, typename Ret = ReturnType<F, Arg>, typename Res = ResultType<Ret>>
    Promise<Res> fail(F&& rejected) {
        
        Promise<Res> promise;
        auto cb = [=]() {
            
            try { failImpl(rejected, promise); }
            catch(...) { promise.reject(ExceptionWrapper::current()); }
            
        };
        std::unique_lock<Mutex> lock(mutex);
        switch(state) {
        case State::Pending: rejectedQueue.emplace_back(std::move(cb)); break;
        case State::Resolved: break;
        case State::Rejected: cb(); break;
        }
        return promise;
        
    }
    
};

}

template <typename T = void>
class Promise {
    
public:
    
    using Type = T;
    
private:
    
    std::shared_ptr<Impl::PromiseImpl<T>> impl = std::make_shared<Impl::PromiseImpl<T>>();
    
public:
    
    Promise() = default;
    Promise(const Promise& src) = default;
    ~Promise() = default;
    
    Promise& operator =(const Promise& src) = default;
    
    template <typename... Arg>
    void resolve(Arg&&... arg) const { impl->resolve(std::forward<Arg>(arg)...); }
    void reject(const ExceptionWrapper& e) const { impl->rejected(e); }
    template <typename F>
    auto then(F&& resolved) const -> decltype(impl->then(std::forward<F>(resolved))) { return impl->then(std::forward<F>(resolved)); }
    template <typename F>
    auto fail(F&& rejected) const -> decltype(impl->fail(std::forward<F>(rejected))) { return impl->fail(std::forward<F>(rejected)); }
    
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
