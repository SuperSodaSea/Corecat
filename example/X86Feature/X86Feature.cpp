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

#include <iomanip>
#include <iostream>

#include "Cats/Corecat/X86.hpp"

using namespace Cats::Corecat;

#define PRINT(x) do { std::cout << #x << " -> " << (x) << std::endl; } while(0)
#define PRINTEX(x, y) do { std::cout << #x << " -> " y << (x) << std::endl; } while(0)

int main() {
    
    std::cout << std::boolalpha << std::hex;
    
    PRINTEX(X86Feature::MAX_BASIC_CPUID, << "0x" << std::setw(8) << std::setfill('0')); 
    PRINTEX(X86Feature::MAX_EXTENDED_CPUID, << "0x" << std::setw(8) << std::setfill('0')); 
    std::cout << std::endl;
    
    PRINT(X86Feature::VENDOR);
    PRINT(X86Feature::BRAND);
    std::cout << std::endl;
    
    PRINT(X86Feature::MMX);
    PRINT(X86Feature::SSE);
    PRINT(X86Feature::SSE2);
    PRINT(X86Feature::SSE3);
    PRINT(X86Feature::SSSE3);
    PRINT(X86Feature::SSE4_1);
    PRINT(X86Feature::SSE4_2);
    PRINT(X86Feature::AVX);
    PRINT(X86Feature::AVX2);
    
    return 0;
    
}
