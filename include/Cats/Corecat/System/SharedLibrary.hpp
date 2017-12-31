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

#ifndef CATS_CORECAT_SYSTEM_SHAREDLIBRARY_HPP
#define CATS_CORECAT_SYSTEM_SHAREDLIBRARY_HPP


#include <stdexcept>

#include "OS.hpp"
#include "../Text/String.hpp"

#if defined(CATS_CORECAT_SYSTEM_OS_WINDOWS)
#   include "../Win32/Windows.hpp"
#elif defined(CATS_CORECAT_SYSTEM_OS_LINUX)
#   include <dlfcn.h>
#else
#   error Unknown OS
#endif


namespace Cats {
namespace Corecat {
inline namespace System {

class SharedLibrary {
    
private:
    
    using String8 = Text::String8;
    using String16 = Text::String16;
    
private:
    
#if defined(CATS_CORECAT_SYSTEM_OS_WINDOWS)
    HMODULE handle = nullptr;
#elif defined(CATS_CORECAT_SYSTEM_OS_LINUX)
    void* handle = nullptr;
#endif
    
public:
    
    SharedLibrary(const String8& path) {
        
#if defined(CATS_CORECAT_SYSTEM_OS_WINDOWS)
        handle = LoadLibraryW(reinterpret_cast<const WCHAR*>(String16(path).getData()));
#elif defined(CATS_CORECAT_SYSTEM_OS_LINUX)
        handle = dlopen(path.getData(), RTLD_NOW);
#endif
        if(!handle) throw std::runtime_error("Failed to load shared library");
        
    }
    SharedLibrary(const SharedLibrary& src) = delete;
    SharedLibrary(SharedLibrary&& src) noexcept : handle(src.handle) { src.handle = nullptr; }
    ~SharedLibrary() {
        
#if defined(CATS_CORECAT_SYSTEM_OS_WINDOWS)
        if(handle) FreeLibrary(handle);
#elif defined(CATS_CORECAT_SYSTEM_OS_LINUX)
        if(handle) dlclose(handle);
#endif
        
    }
    
    SharedLibrary& operator =(const SharedLibrary& src) = delete;
    SharedLibrary& operator =(SharedLibrary&& src) noexcept { handle = src.handle; src.handle = nullptr; return *this; }
    
    void* get(const String8& name) {
        
#if defined(CATS_CORECAT_SYSTEM_OS_WINDOWS)
        return reinterpret_cast<void*>(GetProcAddress(handle, name.getData()));
#elif defined(CATS_CORECAT_SYSTEM_OS_LINUX)
        return dlsym(handle, name.getData());
#endif
        
    }
    
};

}
}
}


#endif
