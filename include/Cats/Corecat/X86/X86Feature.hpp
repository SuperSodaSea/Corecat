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

#ifndef CATS_CORECAT_X86FEATURE_HPP
#define CATS_CORECAT_X86FEATURE_HPP


#include <cstdint>

#include <array>

#include "../System/Compiler.hpp"
#include "../Text/String.hpp"

#if defined(CORECAT_COMPILER_CLANG) || defined(CORECAT_COMPILER_GCC)
#   include <cpuid.h>
#elif defined(CORECAT_COMPILER_MSVC)
#   include <intrin.h>
#else
#   error Unknown compiler
#endif


namespace Cats {
namespace Corecat {
inline namespace X86 {

namespace {

struct X86Feature {
    
public:
    
    static std::array<std::uint32_t, 4> cpuid(std::uint32_t func, std::uint32_t sub) {
        
        std::array<std::uint32_t, 4> data;
#if defined(CORECAT_COMPILER_CLANG) || defined(CORECAT_COMPILER_GCC)
        // __cpuid_count is a macro, so :: is not needed
        __cpuid_count(func, sub, data[0], data[1], data[2], data[3]);
#elif defined(CORECAT_COMPILER_MSVC)
        ::__cpuidex(reinterpret_cast<int*>(data.data()), func, sub);
#endif
        return data;
        
    }
    static bool cpuidBit(std::uint32_t func, std::uint32_t sub, std::uint32_t reg, std::uint32_t bit) {
        
        return (cpuid(func, sub)[reg] >> bit) & 1;
        
    }
    
    static const std::uint32_t MAX_BASIC_CPUID, MAX_EXTENDED_CPUID;
    static const String8 VENDOR, BRAND;
    static const bool MMX, SSE, SSE2, SSE3, SSSE3, SSE4_1, SSE4_2, AVX, AVX2;
    
};

const std::uint32_t
    X86Feature::MAX_BASIC_CPUID = X86Feature::cpuid(0x00, 0)[0],
    X86Feature::MAX_EXTENDED_CPUID = X86Feature::cpuid(0x80000000, 0)[0];
const Text::String8
    X86Feature::VENDOR = [] {
        
        char str[12];
        auto data = X86Feature::cpuid(0x00, 0);
        int table[] = {1, 3, 2};
        for(int i = 0; i < 3; ++i) reinterpret_cast<std::uint32_t*>(str)[i] = data[table[i]];
        return Text::String8(str, 12);
        
    }(),
    X86Feature::BRAND = [] {
        
        char str[48];
        for(int i = 0; i < 3; ++i) {
            
            auto data = cpuid(0x80000002 + i, 0);
            for(int j = 0; j < 4; ++j) reinterpret_cast<std::uint32_t*>(str)[i * 4 + j] = data[j];
            
        }
        return Text::String8(str);
        
    }();
const bool
    X86Feature::MMX = X86Feature::cpuidBit(0x01, 0, 3, 23),
    X86Feature::SSE = X86Feature::cpuidBit(0x01, 0, 3, 25),
    X86Feature::SSE2 = X86Feature::cpuidBit(0x01, 0, 3, 26),
    X86Feature::SSE3 = X86Feature::cpuidBit(0x01, 0, 2, 0),
    X86Feature::SSSE3 = X86Feature::cpuidBit(0x01, 0, 2, 9),
    X86Feature::SSE4_1 = X86Feature::cpuidBit(0x01, 0, 2, 19),
    X86Feature::SSE4_2 = X86Feature::cpuidBit(0x01, 0, 2, 20),
    X86Feature::AVX = X86Feature::cpuidBit(0x01, 0, 2, 28),
    X86Feature::AVX2 = MAX_BASIC_CPUID >= 0x07 && X86Feature::cpuidBit(0x07, 0, 1, 5);

}

}
}
}


#endif
