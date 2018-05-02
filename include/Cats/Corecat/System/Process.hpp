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
#   include "../Win32/Handle.hpp"
#elif defined(CORECAT_OS_LINUX) || defined(CORECAT_OS_MACOS)
#   include <spawn.h>
#   include <sys/wait.h>
#   if defined(CORECAT_OS_MACOS)
#       include <crt_externs.h>
#       define environ (*_NSGetEnviron())
#   else
#       include <unistd.h>
#   endif
#else
#   error Unknown OS
#endif


namespace Cats {
namespace Corecat {
inline namespace System {

struct ProcessOption {
    
    const char* file = nullptr;
    const char* const* argument = nullptr;
    const char* const* environment = nullptr;
    const char* directory = nullptr;
    
    ProcessOption(const char* file_, const char* const* argument_ = nullptr, const char* const* environment_ = nullptr, const char* directory_ = nullptr) :
        file(file_), argument(argument_), environment(environment_), directory(directory_) {}
    
};

class Process {
    
private:
    
    template <typename T>
    static Array<T> toNullTerminated(ArrayView<const T> arr) {
        
        Array<T> ret(arr.getSize() + 1);
        std::copy(arr.begin(), arr.end(), ret.begin());
        ret[arr.getSize()] = T();
        return ret;
        
    }
    
#if defined(CORECAT_OS_WINDOWS)
private:
    
    static WString getFullPath(const WString& file) {
        
        HMODULE module = ::LoadLibraryW(file.getData());
        if(!module) return {};
        WString path;
        path.setLength(MAX_PATH);
        while(true) {
            
            DWORD pathLength = ::GetModuleFileNameW(module, path.getData(), DWORD(path.getLength() + 1));
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
    
private:
#if defined(CORECAT_OS_WINDOWS)
    Handle handle;
#else
    pid_t pid;
#endif
    
public:
    
    Process(const ProcessOption& option) {
#if defined(CORECAT_OS_WINDOWS)
        WString argument;
        if(option.argument) {
            
            for(auto p = option.argument; *p; ++p) {
                
                if(!argument.isEmpty()) argument += L' ';
                argument += L'"';
                for(auto&& c : WString(*p)) {
                    
                    if(c == L'"') argument += L"\\\"";
                    else if(c == L'\\') argument += L"\\\\";
                    else argument += c;
                    
                }
                argument += L'"';
                
            }
            
        }
        WString environment;
        if(option.environment) {
            
            for(auto p = option.environment; *p; ++p) {
                
                environment += WString(*p);
                environment += wchar_t();
                
            }
            
        }
        STARTUPINFOW si = {};
        si.cb = sizeof(si);
        si.wShowWindow = SW_SHOWDEFAULT;
        PROCESS_INFORMATION pi = {};
        auto path = findFullPath(WString(option.file));
        if(!::CreateProcessW(
            path.getData(),
            option.argument ? argument.getData() : nullptr,
            nullptr,
            nullptr,
            true,
            CREATE_UNICODE_ENVIRONMENT,
            option.environment ? environment.getData() : nullptr,
            option.directory ? WString(option.directory).getData() : nullptr,
            &si,
            &pi))
            throw SystemException("::CreateProcessW failed");
        ::CloseHandle(pi.hThread);
        handle = pi.hProcess;
#else
        if(::posix_spawnp(
            &pid,
            option.file,
            nullptr,
            nullptr,
            const_cast<char* const*>(option.argument),
            option.environment ? const_cast<char* const*>(option.environment) : environ))
            throw SystemException("::posix_spawn failed");
#endif
    }
    Process(const char* file, const char* const* argument = nullptr, const char* const* environment = nullptr, const char* directory = nullptr) :
        Process(ProcessOption(file, argument, environment, directory)) {}
    Process(const char* file, const char* directory) :
        Process(file, nullptr, nullptr, directory) {}
    Process(const char* file, ArrayView<const char* const> argument, const char* directory = nullptr) :
        Process(file, toNullTerminated(argument).getData(), nullptr, directory) {}
    Process(const char* file, ArrayView<const char* const> argument, ArrayView<const char* const> environment, const char* directory = nullptr) :
        Process(file, toNullTerminated(argument).getData(), toNullTerminated(environment).getData(), directory) {}
    Process(const Process& src) = delete;
    ~Process() = default;
    
    Process& operator =(const Process& src) = delete;
    
    std::int64_t wait() {
#if defined(CORECAT_OS_WINDOWS)
        if(::WaitForSingleObject(handle, INFINITE) != WAIT_OBJECT_0)
            throw SystemException("::WaitForSingleObject failed");
        DWORD ret;
        if(!::GetExitCodeProcess(handle, &ret))
            throw SystemException("::GetExitCodeProcess failed");
        handle.close();
        return ret;
#else
        int ret;
        if(::waitpid(pid, &ret, 0) < 0)
            throw SystemException("::waitpid failed");
        pid = 0;
        return ret;
#endif
    }
    
};

}
}
}


#endif
