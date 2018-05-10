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

#if defined(CORECAT_OS_WINDOWS)
#   include "../Win32/Windows.hpp"
#elif defined(CORECAT_OS_LINUX) || defined(CORECAT_OS_MACOS)
#   include <unistd.h>
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
    
};

}
}
}


#endif
