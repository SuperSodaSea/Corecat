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

#ifndef CATS_CORECAT_SYSTEM_PROCESS_HPP
#define CATS_CORECAT_SYSTEM_PROCESS_HPP


#include "OS.hpp"
#include "../Data/Array.hpp"
#include "../Text/String.hpp"
#include "../Util/Exception.hpp"

#if defined(CORECAT_OS_WINDOWS)
#   include "../Win32/Windows.hpp"
#elif defined(CORECAT_OS_LINUX) || defined(CORECAT_OS_MACOS)
#   include <spawn.h>
#else
#   error Unknown OS
#endif


namespace Cats {
namespace Corecat {
inline namespace System {

class Process {
    
#if defined(CORECAT_OS_WINDOWS)
private:
    
    static WString getFullPath(const WString& file) {
        
        HMODULE module = ::LoadLibraryW(file.getData());
        if(!module) return {};
        WString path;
        path.setLength(MAX_PATH);
        while(true) {
            
            DWORD pathLength = ::GetModuleFileNameW(module, path.getData(), path.getLength() + 1);
            if(::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                
                path.setLength(path.getLength() * 2);
                continue;
                
            }
            if(!pathLength) {
                
                ::FreeLibrary(module);
                return {};
                
            }
            path.setLength(pathLength);
            break;
            
        }
        ::FreeLibrary(module);
        return path;
        
    }
    
    static WString findFullPath(const WString& file) {
        
        WString path;
        path = getFullPath(file + L'.');
        if(!path.isEmpty()) return path;
        path = getFullPath(file + L".exe.");
        if(!path.isEmpty()) return path;
        throw SystemException("File not found");
        
    }
#endif
    
public:
    
    static void create(const char* file, ArrayView<const char* const> argv) {
#if defined(CORECAT_OS_WINDOWS)
        WString argument;
        for(auto&& arg : argv) {
            
            if(!argument.isEmpty()) argument += L' ';
            argument += L'"';
            for(auto&& c : WString(arg)) {
                
                if(c == L'"') argument += L"\\\"";
                else if(c == L'\\') argument += L"\\\\";
                else argument += c;
                
            }
            argument += L'"';
            
        }
        STARTUPINFOW si = {};
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi = {};
        auto path = findFullPath(WString(file));
        if(!::CreateProcessW(path.getData(), argument.getData(), nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi))
            throw SystemException("::CreateProcessW failed");
        ::CloseHandle(pi.hProcess);
        ::CloseHandle(pi.hThread);
#else
        pid_t pid;
        if(::posix_spawn(&pid, file, nullptr, nullptr, argv, nullptr))
            throw SystemException("::posix_spawn failed");
#endif
    }
    
};

}
}
}


#endif
