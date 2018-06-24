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

#ifndef CATS_CORECAT_SYSTEM_ENVIRONMENT_HPP
#define CATS_CORECAT_SYSTEM_ENVIRONMENT_HPP


#include "OS.hpp"
#include "../Data/Array.hpp"
#include "../Text/String.hpp"
#include "../Util/Exception.hpp"

#if defined(CORECAT_OS_WINDOWS)
#   include "../Win32/Windows.hpp"
#elif defined(CORECAT_OS_LINUX) || defined(CORECAT_OS_MACOS)
#   include <limits.h>
#   include <unistd.h>
#   include <cstdlib>
#   if defined(CORECAT_OS_MACOS)
#       include <crt_externs.h>
#       define environ (*_NSGetEnviron())
#   endif
#else
#   error Unknown OS
#endif


namespace Cats {
namespace Corecat {
inline namespace System {

class Environment {
    
public:
    
    static Array<String8> getEnvironmentVariable() {
#if defined(CORECAT_OS_WINDOWS)
        std::size_t count = 0;
        auto env = ::GetEnvironmentStringsW();
        if(!env) return {};
        for(auto p = env; *p; ++count, ++p)
            while(*++p);
        Array<String8> data;
        data.reserve(count);
        for(auto p = env; *p; ++p) {
            
            data.push(WString(p));
            while(*++p);
            
        }
        ::FreeEnvironmentStringsW(env);
        return data;
#else
        std::size_t count = 0;
        for(auto p = environ; *p; ++count, ++p);
        Array<String8> data;
        data.reserve(count);
        for(auto p = environ; *p; ++p)
            data.push(WString(*p));
        return data;
#endif
    }
    
    static String8 getEnvironmentVariable(StringView8 name) {
#if defined(CORECAT_OS_WINDOWS)
        WString n(name);
        DWORD length = ::GetEnvironmentVariableW(n.getData(), nullptr, 0);
        if(!length) return {};
        WString v;
        v.setLength(length - 1);
        ::GetEnvironmentVariableW(n.getData(), v.getData(), length);
        return String8(v);
#else
        auto value = ::getenv(String8(name).getData());
        if(value) return value;
        return {};
#endif
    }
    
    static void setEnvironmentVariable(StringView8 name, StringView8 value) {
#if defined(CORECAT_OS_WINDOWS)
        WString n(name), v(value);
        if(!::SetEnvironmentVariableW(n.getData(), v.getData()))
            throw SystemException("::SetEnvironmentVariableW failed");
#else
#endif
    }
    
    static String8 getCurrentDirectory() {
#if defined(CORECAT_OS_WINDOWS)
        WString path;
        path.setLength(::GetCurrentDirectoryW(0, nullptr));
        ::GetCurrentDirectoryW(DWORD(path.getLength() + 1), path.getData());
        return String8(path);
#else
        char path[PATH_MAX];
        if(!::getcwd(path, sizeof(path)))
            throw SystemException("::getcwd failed");
        return path;
#endif
    }
    
};

}
}
}


#endif
