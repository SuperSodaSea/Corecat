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

#ifndef CATS_CORECAT_UTIL_EXCEPTIONPTR_HPP
#define CATS_CORECAT_UTIL_EXCEPTIONPTR_HPP


#include <exception>
#include <memory>
#include <type_traits>

#include "Exception.hpp"


namespace Cats {
namespace Corecat {
inline namespace Util {

class ExceptionPtr {
    
private:
    
    template <typename T>
    using EnableIfException = std::enable_if_t<std::is_base_of<std::exception, T>::value>;
    
    template <typename F>
    struct ArgumantTypeImpl;
    template <typename Class, typename Arg, typename Ret>
    struct ArgumantTypeImpl<Ret (Class::*)(Arg)> { using Type = Arg; };
    template <typename Class, typename Arg, typename Ret>
    struct ArgumantTypeImpl<Ret (Class::*)(Arg) const> { using Type = Arg; };
    template <typename F, typename T = std::decay_t<F>>
    using ArgumentType = typename ArgumantTypeImpl<decltype(&T::operator())>::Type;
    
    class HolderBase {
        
    public:
        
        virtual ~HolderBase() = default;
        
        virtual const void* get() const noexcept = 0;
        [[noreturn]] virtual void rethrow() const = 0;
        virtual const std::type_info* getType() const noexcept = 0;
        
    };
    
    template <typename T>
    class Holder1 : public HolderBase {
        
    private:
        
        T t;
        
    public:
        
        template <typename... Arg>
        Holder1(Arg&&... arg) : t(std::forward<Arg>(arg)...) {}
        virtual ~Holder1() = default;
        
        const void* get() const noexcept final { return std::addressof(t); }
        [[noreturn]] void rethrow() const final { throw t; }
        const std::type_info* getType() const noexcept final { return &typeid(T); }
        
    };
    
    class Holder2 : public HolderBase {
        
    private:
        
        std::exception_ptr ptr;
        
    public:
        
        Holder2(std::exception_ptr ptr_) : ptr(ptr_) {}
        virtual ~Holder2() = default;
        
        const void* get() const noexcept final { return nullptr; }
        [[noreturn]] void rethrow() const final { std::rethrow_exception(ptr); }
        const std::type_info* getType() const noexcept final { return nullptr; }
        
    };
    
private:
    
    std::shared_ptr<HolderBase> data;
    
    std::shared_ptr<std::exception> exception;
    const std::type_info* type = nullptr;
    void (*throwFunction)(const std::exception&) = nullptr;
    std::exception_ptr eptr = nullptr;
    
private:
    
    template <typename T>
    static void throwFunctionImpl(const std::exception& e) { throw static_cast<const T&>(e); }
    
public:
    
    ExceptionPtr() = default;
    template <typename T, typename U = std::remove_reference_t<T>, typename = EnableIfException<U>>
    ExceptionPtr(T&& t) : data(std::make_shared<Holder1<U>>(std::forward<T>(t))) {}
    ExceptionPtr(std::exception_ptr ptr) : data(std::make_shared<Holder2>(ptr)) {}
    ExceptionPtr(const ExceptionPtr& src) : data(src.data) {}
    
    template <typename T, typename U = std::remove_reference_t<T>, typename = EnableIfException<U>>
    ExceptionPtr& operator =(T&& t) { return *this = ExceptionPtr(std::forward<T>(t)); }
    ExceptionPtr& operator =(std::exception_ptr ptr) noexcept { return *this = ExceptionPtr(ptr); }
    ExceptionPtr& operator =(const ExceptionPtr& src) noexcept { data = src.data; return *this; }
    
    operator bool() const noexcept { return bool(data); }
    
    [[noreturn]] void rethrow() const {
        
        if(data) data->rethrow();
        throw InvalidArgumentException("No exception in ExceptionPtr");
        
    }
    
    template <typename F, typename T = std::remove_reference_t<ArgumentType<F>>>
    bool with(F&& f) const {
        
        if(!data) return false;
        auto type = data->getType();
        if(type && *type == typeid(T)) {
            
            f(*static_cast<const T*>(data->get()));
            return true;
            
        }
        try { data->rethrow(); }
        catch(const T& arg) { f(arg); return true; }
        catch(...) {}
        return false;
        
    }
    
public:
    
    static ExceptionPtr getCurrent() noexcept { return std::current_exception(); }
    
};

}
}
}


#endif
