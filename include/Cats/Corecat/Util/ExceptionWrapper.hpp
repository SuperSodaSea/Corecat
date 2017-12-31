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

#ifndef CATS_CORECAT_UTIL_EXCEPTIONWRAPPER_HPP
#define CATS_CORECAT_UTIL_EXCEPTIONWRAPPER_HPP


#include <exception>
#include <memory>
#include <stdexcept>
#include <type_traits>


namespace Cats {
namespace Corecat {
inline namespace Util {

// Inspired by https://github.com/facebook/folly/blob/master/folly/ExceptionWrapper.h

class ExceptionWrapper {
    
private:
    
    template <typename T>
    using EnableIfException = typename std::enable_if<std::is_base_of<std::exception, T>::value>::type;
    
    template <typename F>
    struct ArgumantTypeImpl;
    template <typename Class, typename Arg, typename Ret>
    struct ArgumantTypeImpl<Ret (Class::*)(Arg)> { using Type = Arg; };
    template <typename Class, typename Arg, typename Ret>
    struct ArgumantTypeImpl<Ret (Class::*)(Arg) const> { using Type = Arg; };
    template <typename F, typename T = typename std::decay<F>::type>
    using ArgumentType = typename ArgumantTypeImpl<decltype(&T::operator())>::Type;
    
private:
    
    std::shared_ptr<std::exception> exception;
    const std::type_info* type = nullptr;
    void (*throwFunction)(const std::exception&) = nullptr;
    std::exception_ptr eptr = nullptr;
    
private:
    
    template <typename T>
    static void throwFunctionImpl(const std::exception& e) { throw static_cast<const T&>(e); }
    
public:
    
    ExceptionWrapper(const ExceptionWrapper& src) : exception(src.exception), type(src.type), throwFunction(src.throwFunction), eptr(src.eptr) {}
    ~ExceptionWrapper() = default;
    
    ExceptionWrapper() noexcept = default;
    template <typename T, typename U = typename std::decay<T>::type, typename = EnableIfException<U>>
    ExceptionWrapper(T&& t) : exception(std::make_shared<T>(t)), type(&typeid(U)), throwFunction(throwFunctionImpl<U>) {}
    ExceptionWrapper(std::exception_ptr eptr_) noexcept : eptr(eptr_) {}
    
    ExceptionWrapper& operator =(const ExceptionWrapper& src) noexcept { exception = src.exception; type = src.type; throwFunction = src.throwFunction; eptr = src.eptr; return *this; }
    
    ExceptionWrapper& operator =(std::exception_ptr eptr_) noexcept { return *this = ExceptionWrapper(eptr_); }
    template <typename T, typename = EnableIfException<T>>
    ExceptionWrapper& operator =(T&& t) { return *this = ExceptionWrapper(std::forward<T>(t)); }
    
    operator bool() const noexcept { return exception || eptr; }
    
    [[noreturn]] void rethrow() const {
        
        if(exception) throwFunction(*exception.get());
        else if(eptr) std::rethrow_exception(eptr);
        else throw std::runtime_error("No exception in ExceptionWrapper");
        std::terminate();
        
    }
    
    template <typename F, typename Arg = typename std::decay<ArgumentType<F>>::type>
    bool with(F&& f) const {
        
        if(exception && *type == typeid(Arg)) {
            
            f(static_cast<const Arg&>(*exception));
            return true;
            
        } else if(eptr) {
            
            try { rethrow(); }
            catch(const Arg& arg) { f(arg); }
            catch(...) {}
            
        }
        return false;
        
    }
    
public:
    
    static ExceptionWrapper current() noexcept { return std::current_exception(); }
    
};

}
}
}


#endif
