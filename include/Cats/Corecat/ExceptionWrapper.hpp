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

#ifndef CATS_CORECAT_EXCEPTIONWRAPPER_HPP
#define CATS_CORECAT_EXCEPTIONWRAPPER_HPP


#include <exception>
#include <stdexcept>


namespace Cats {
namespace Corecat {

class ExceptionWrapper {
    
private:
    
    std::exception_ptr eptr;
    
public:
    
    ExceptionWrapper(const ExceptionWrapper& src) : eptr(src.eptr) {}
    ~ExceptionWrapper() = default;
    
    ExceptionWrapper() noexcept : eptr() {}
    ExceptionWrapper(std::exception_ptr eptr_) noexcept : eptr(eptr_) {}
    
    ExceptionWrapper& operator =(const ExceptionWrapper& src) { eptr = src.eptr; }
    
    ExceptionWrapper& operator =(std::exception_ptr eptr_) { eptr = eptr_; return *this; }
    
    [[noreturn]] void rethrow() const {
        
        if(eptr) { std::rethrow_exception(eptr); }
        else throw std::runtime_error("no exception");
        
    }
    
    void setCurrent() noexcept {
        
        eptr = std::current_exception();
        
    }
    
};

}
}


#endif
