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
#   include <unistd.h>
#   include <sys/ioctl.h>
#   include <sys/wait.h>
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
    
    static WString getProcessDirectory() {
        
        WString path;
        path.setLength(MAX_PATH);
        while(true) {
            
            DWORD length = ::GetModuleFileNameW(nullptr, path.getData(), DWORD(path.getLength() + 1));
            if(::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                
                path.setLength(path.getLength() * 2);
                continue;
                
            }
            if(!length)
                throw SystemException("::GetModuleFileNameW failed");
            while(length && path[length - 1] != '/' && path[length - 1] != '\\') --length;
            if(length) --length;
            path.setLength(length);
            return path;
            
        }
        
    }
    static WString getCurrentDirectory() {
        
        WString path;
        DWORD length = ::GetCurrentDirectoryW(0, nullptr);
        if(!length) return {};
        path.setLength(length - 1);
        ::GetCurrentDirectoryW(length, path.getData());
        return path;
        
    }
    static WString getSystemDirectory() {
        
        WString path;
        UINT length = ::GetSystemDirectoryW(nullptr, 0);
        if(!length) return {};
        path.setLength(length - 1);
        ::GetSystemDirectoryW(path.getData(), length);
        return path;
        
    }
    static WString getWindowsDirectory() {
        
        WString path;
        UINT length = ::GetWindowsDirectoryW(nullptr, 0);
        if(!length) return {};
        path.setLength(length - 1);
        ::GetWindowsDirectoryW(path.getData(), length);
        return path;
        
    }
    
    static bool isExist(const WString& path) {
        
        DWORD attribute = ::GetFileAttributesW(path.getData());
        return attribute != INVALID_FILE_ATTRIBUTES && !(attribute & FILE_ATTRIBUTE_DIRECTORY);
        
    }
    
    static WString enumPath(const WString& file, const Array<WString>& pathList) {
        
        for(auto&& x : pathList) {
            
            auto path = x + L'\\' + file;
            if(isExist(path)) return path;
            
        }
        return {};
        
    }
    
    static WString enumExtension(const WString& file, const Array<WString>& pathList) {
        
        WString path;
        path = enumPath(file, pathList);
        if(!path.isEmpty()) return path;
        path = enumPath(file + L".com", pathList);
        if(!path.isEmpty()) return path;
        path = enumPath(file + L".exe", pathList);
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
        Array<WString> pathList;
        pathList.push(getProcessDirectory());
        pathList.push(getCurrentDirectory());
        pathList.push(getSystemDirectory());
        pathList.push(getWindowsDirectory());
        // TODO: PATH
        auto path = enumExtension(WString(option.file), pathList);
        
        WString argument;
        if(option.argument) {
            
            for(auto p = option.argument; *p; ++p) {
                
                if(!argument.isEmpty()) argument += L' ';
                WString src(*p);
                WString dst;
                bool quote = false;
                for(std::size_t i = src.getLength() - 1; i != std::size_t(-1); --i) {
                    
                    auto c = src[i];
                    dst += c;
                    if(quote && c == L'\\') dst += L'\\';
                    else if(c == L'\"') quote = true, dst += L'\\';
                    else quote = false;
                    
                }
                argument += L'"' + dst.reverse() + L'"';
                
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
        int messagePipe[2];
        if(::pipe(messagePipe))
            throw SystemException("::pipe failed");
        
        int ret;
        do
            ret = ::ioctl(messagePipe[1], FIOCLEX);
        while(ret < 0 && errno == EINTR);
        if(ret) {
            
            ::close(messagePipe[0]);
            ::close(messagePipe[1]);
            throw SystemException("::ioctl failed");
            
        }
        
        pid = ::fork();
        if(pid < 0) {
            
            ::close(messagePipe[0]);
            ::close(messagePipe[1]);
            throw SystemException("::fork failed");
            
        }
        
        if(pid > 0) {
            
            // Parent process
            ::close(messagePipe[1]);
            
            ssize_t n;
            char data;
            do
                n = ::read(messagePipe[0], &data, 1);
            while(n < 0 && errno == EINTR);
            
            ::close(messagePipe[0]);
            if(n) {
                
                do
                    ret = ::waitpid(pid, nullptr, 0);
                while(ret < 0 && errno == EINTR);
                throw SystemException("Failed to create process");
                
            }
            
        } else {
            
            // Child process
            ::close(messagePipe[0]);
            do {
                
                if(option.environment) environ = const_cast<char**>(option.environment);
                if(option.directory && ::chdir(option.directory))
                    break;
                ::execvp(option.file, const_cast<char* const*>(option.argument));
                
            } while(false);
            
            // Failed
            ssize_t n;
            char data = 0;
            do
                n = ::write(messagePipe[1], &data, 1);
            while(n < 0 && errno == EINTR);
            ::_exit(127);
            
        }
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
        int ret, code;
        do
            ret = ::waitpid(pid, &code, 0);
        while(ret < 0 && errno == EINTR);
        if(ret < 0)
            throw SystemException("::waitpid failed");
        pid = 0;
        return code;
#endif
    }
    
};

}
}
}


#endif
