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

#include <functional>
#include <iostream>

#include "Cats/Corecat/Text.hpp"
#include "Cats/Corecat/Util.hpp"


using namespace Cats::Corecat;


std::function<void(int)> printRange() {
    
    bool first = true;
    return [=](int x) mutable {
        if(first) first = false;
        else std::cout << ", ";
        std::cout << x;
    };
    
}

#define PRINT(...) do { std::cout << #__VA_ARGS__ << "\n    -> "; std::cout << (__VA_ARGS__) << std::endl; } while(0)
#define PRINT_RANGE(...) do { std::cout << #__VA_ARGS__ << "\n    -> "; do { __VA_ARGS__ | forEach(printRange()); } while(0); std::cout << std::endl; } while(0)

int main() {
    
    // 1, 2, 3, 4, 5
    PRINT_RANGE(int arr[] = {1, 2, 3, 4, 5}; rangeView(arr));
    // 1, 2, 3, 4, 5
    PRINT_RANGE(iota(1) | take(5));
    // 3, 4, 5, 6, 7
    PRINT_RANGE(iota(1) | skip(2) | take(5));
    // 5, 4, 3, 2, 1
    PRINT_RANGE(iota(1) | take(5) | reverse());
    // 1, 4, 9, 16, 25
    PRINT_RANGE(iota(1) | take(5) | transform([](int x) { return x * x; }));
    // 1, 3, 5, 7, 9
    PRINT_RANGE(iota(1) | take(10) | filter([](int x) { return x % 2; }));
    // 5050
    PRINT(iota(1) | take(100) | reduce());
    // 12345678
    PRINT(iota(1) | take(8) | transduce([](int x) { return x + '0'; }, String8()));
    
    return 0;
    
}
