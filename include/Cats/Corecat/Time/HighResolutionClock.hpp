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

#ifndef CATS_CORECAT_TIME_HIGHRESOLUTIONCLOCK_HPP
#define CATS_CORECAT_TIME_HIGHRESOLUTIONCLOCK_HPP


#include <chrono>

#include "../System/OS.hpp"

#if defined(CORECAT_OS_WINDOWS)
#   include "../Win32/Windows.hpp"
#elif defined(CORECAT_OS_LINUX) || defined(CORECAT_OS_MACOS)
#include <time.h>
#else
#   error Unknown OS
#endif


namespace Cats {
namespace Corecat {
inline namespace Time {

struct HighResolutionClock {
    
    using rep = double;
    using period = std::ratio<1>;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<HighResolutionClock>;
    
    static constexpr bool is_steady = true;
    
    static time_point now() noexcept {
#if defined(CORECAT_OS_WINDOWS)
        LARGE_INTEGER nf, n;
        // TODO: Run ::QueryPerformanceFrequency only once
        ::QueryPerformanceFrequency(&nf);
        ::QueryPerformanceCounter(&n);
        return time_point(duration(double(n.QuadPart) / nf.QuadPart));
#elif defined(CORECAT_OS_LINUX) || defined(CORECAT_OS_MACOS)
        timespec t;
        ::clock_gettime(CLOCK_MONOTONIC, &t);
        return time_point(duration(t.tv_sec + t.tv_nsec * 0.000000001));
#endif
    }
    
};

}
}
}


#endif
