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
    
private:
    
    template <typename F, typename... Arg>
    using ResultOf = decltype(std::declval<F>()(std::declval<Arg>()...));
    
    template <typename F, typename... Arg>
    class IsCallableWith {
        
    private:
        
        template <typename U, typename = ResultOf<U, Arg...>>
        static constexpr bool f(int) { return true; }
        template <typename>
        static constexpr bool f(...) { return false; }
        
    public:
        
        static constexpr bool value = f<F>(0);
        
    };
    
    template <typename F, typename U>
    using ArgumentOf =
        typename std::conditional<IsCallableWith<F, U>::value, U,
        typename std::conditional<IsCallableWith<F, const U&>::value, const U&,
        void>::type>::type;
    
private:
    
    class Impl {
        
    private:
        
        State state = State::Pending;
        std::deque<std::function<void(const T&)>> resolvedQueue;
        std::deque<std::function<void(const ExceptionWrapper&)>> rejectedQueue;
        T result;
        ExceptionWrapper exception;
        
    public:
        
        Impl(const Impl& src) = delete;
        ~Impl() {}
        
        Impl() = default;
        
        Impl& operator =(const Impl& src) = delete;
        
        State getState() { return state; }
        
        void resolve(T&& t) {
            
            if(state == State::Pending) {
                
                state = State::Resolved;
                result = std::forward<T>(t);
                for(auto& x : resolvedQueue) x(result);
                resolvedQueue.clear();
                rejectedQueue.clear();
                
            }
            
        }
        void reject(ExceptionWrapper&& e) {
            
            if(state == State::Pending) {
                
                state = State::Rejected;
                exception = std::forward<ExceptionWrapper>(e);
                for(auto& x : rejectedQueue) x(exception);
                resolvedQueue.clear();
                rejectedQueue.clear();
                
            }
            
        }
        
        template <typename F, typename Arg = ArgumentOf<F, T>, typename Res = ResultOf<F, Arg>>
        Promise<Res> then(F&& resolved) {
            
            Promise<Res> promise;
            auto cb = [promise, resolved](const T& t) {
                
                try { promise.resolve(resolved(t)); }
                catch(...) {
                    
                    ExceptionWrapper ew;
                    ew.setCurrent();
                    promise.reject(std::move(ew));
                    
                }
                
            };
            switch(state) {
            case State::Pending: resolvedQueue.emplace_back(cb); break;
            case State::Resolved: cb(result); break;
            default: break;
            }
            return promise;
            
        }
        template <typename F, typename Arg = ArgumentOf<F, ExceptionWrapper>, typename Res = ResultOf<F, Arg>>
        Promise<Res> fail(F&& rejected) {
            
            Promise<Res> promise;
            auto cb = [promise, rejected](const ExceptionWrapper& e) {
                
                try { promise.resolve(rejected(e)); }
                catch(...) {
                    
                    ExceptionWrapper ew;
                    ew.setCurrent();
                    promise.reject(std::move(ew));
                    
                }
                
            };
            switch(state) {
            case State::Pending: rejectedQueue.emplace_back(cb); break;
            case State::Rejected: cb(exception); break;
            default: break;
            }
            return promise;
            
        }
        
    };
    
private:
    
    std::shared_ptr<Impl> impl;
    
public:
    
    Promise(const Promise& src) : impl(src.impl) {}
    
    Promise() : impl(std::make_shared<Impl>()) {}
    
    Promise& operator =(const Promise& src) { impl = src.impl; return *this; }
    
    void resolve(T&& t) const { impl->resolve(std::move(t)); }
    void reject(ExceptionWrapper&& t) const { impl->reject(std::move(t)); }
    
    template <typename F>
    Promise<ResultOf<F, ArgumentOf<F, T>>> then(F&& resolved) const { return impl->then(std::forward<F>(resolved)); }
    template <typename F>
    Promise<ResultOf<F, ArgumentOf<F, ExceptionWrapper>>> fail(F&& rejected) const { return impl->fail(std::forward<F>(rejected)); }
    
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
