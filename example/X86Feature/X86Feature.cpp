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

using namespace Cats::Corecat::X86;

int main() {
    
    std::cout << std::boolalpha << std::hex;
    
    std::cout << "Max Basic CPUID: 0x" << std::setw(8) << std::setfill('0') << X86Feature::MAX_BASIC_CPUID << std::endl;
    std::cout << "Max Extended CPUID: 0x" << std::setw(8) << std::setfill('0') << X86Feature::MAX_EXTENDED_CPUID << std::endl;
    std::cout << std::endl;
    
    std::cout << "Vendor: " << X86Feature::VENDOR << std::endl;
    std::cout << "Brand: " << X86Feature::BRAND << std::endl;
    std::cout << std::endl;
    
    std::cout << "MMX: " << X86Feature::MMX << std::endl;
    std::cout << "SSE: " << X86Feature::SSE << std::endl;
    std::cout << "SSE2: " << X86Feature::SSE2 << std::endl;
    std::cout << "SSE3: " << X86Feature::SSE3 << std::endl;
    std::cout << "SSSE3: " << X86Feature::SSSE3 << std::endl;
    std::cout << "SSE4.1: " << X86Feature::SSE4_1 << std::endl;
    std::cout << "SSE4.2: " << X86Feature::SSE4_2 << std::endl;
    std::cout << "AVX: " << X86Feature::AVX << std::endl;
    std::cout << "AVX2: " << X86Feature::AVX2 << std::endl;
    std::cout << std::endl;
    
    return 0;
    
}
