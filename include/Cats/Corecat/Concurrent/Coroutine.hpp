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

#ifndef CATS_CORECAT_CONCURRENT_COROUTINE_HPP
#define CATS_CORECAT_CONCURRENT_COROUTINE_HPP


#include "Promise.hpp"


namespace Cats {
namespace Corecat {
inline namespace Concurrent {

template <typename T>
class Coroutine : public std::enable_shared_from_this<T> {
    
protected:
    
    int _coroutineState = 0;
    ExceptionPtr _exception;
    
};


#define CORECAT_COROUTINE \
    if(_exception) _exception.rethrow(); \
    switch(_coroutineState) \
        if(false) { \
            _BREAK_COROUTINE: \
            break; \
        } else case 0:

#define CORECAT_AWAIT_IMPL(expr, n) \
    do { \
        if(true) { \
            _coroutineState = (n); \
            (expr) \
                .then([self = shared_from_this()] { (*self)(); }) \
                .fail([self = shared_from_this()](auto& e) { self->_exception = e, (*self)(); }); \
            goto _BREAK_COROUTINE; \
        } else { case (n): ; } \
    } while(0)

#define CORECAT_AWAIT(expr) CORECAT_AWAIT_IMPL(expr, __LINE__)

#define CORECAT_AWAIT_FOR_IMPL(expr, to, n) \
    do { \
        if(true) { \
            _coroutineState = (n); \
            (expr) \
                .then([&, self = shared_from_this()](auto x) { to = std::move(x); (*self)(); }) \
                .fail([self = shared_from_this()](auto& e) { self->_exception = e, (*self)(); }); \
            goto _BREAK_COROUTINE; \
        } else { case (n): ; } \
    } while(0)

#define CORECAT_AWAIT_FOR(expr, to) CORECAT_AWAIT_FOR_IMPL(expr, to, __LINE__)

}
}
}


#endif
