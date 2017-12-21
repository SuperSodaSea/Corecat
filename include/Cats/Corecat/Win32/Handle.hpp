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

#ifndef CATS_CORECAT_WIN32_HANDLE_HPP
#define CATS_CORECAT_WIN32_HANDLE_HPP


#include <cassert>

#include "Windows.hpp"


namespace Cats {
namespace Corecat {
namespace Win32 {

class Handle {
    
private:
    
    HANDLE handle = nullptr;
    
public:
    
    Handle() = default;
    Handle(HANDLE handle_) : handle(handle_) {}
    Handle(const Handle& src) = delete;
    Handle(Handle&& src) : handle(src.handle) { src.handle = nullptr; }
    ~Handle() { if(*this) close(); }
    
    Handle& operator =(HANDLE handle_) { if(*this) close(); handle = handle_; return *this; }
    Handle& operator =(const Handle& src) = delete;
    Handle& operator =(Handle&& src) { handle = src.handle; src.handle = nullptr; return *this; }
    
    operator HANDLE() const { return handle; }
    explicit operator bool() const { return handle != nullptr && handle != INVALID_HANDLE_VALUE; }
    
    void close() { assert(*this); CloseHandle(handle); handle = nullptr; }
    
};

}
}
}


#endif
