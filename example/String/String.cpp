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

#include <iostream>

#include "Cats/Corecat/Text.hpp"


using namespace Cats::Corecat;


#define PRINT(x) do { std::cout << #x << "\n    -> " << (x) << std::endl; } while(0)

int main() {
    
    std::cout << std::boolalpha;
    
    auto str1 = "0123456789"_s;
    PRINT(str1); // "0123456789"
    PRINT(str1.getLength()); // 10
    std::cout << std::endl;
    
    PRINT(str1 == str1); // true
    PRINT(str1 == "0123456789"); // true
    PRINT(str1 != "0123456788"); // true
    PRINT(str1 < "0"); // false
    PRINT(str1 > "0"); // true
    std::cout << std::endl;
    
    PRINT(str1.isEmpty()); // false
    PRINT(str1.startsWith("0123")); // true
    PRINT(str1.endsWith("6789")); // true
    std::cout << std::endl;
    
    PRINT(str1.find("567")); // 5
    PRINT(str1.find("567", 6)); // -1
    std::cout << std::endl;
    
    PRINT(str1.repeat(2)); // "01234567890123456789"
    PRINT(str1 * 2); // "01234567890123456789"
    std::cout << std::endl;
    
    PRINT(str1.slice(3)); // "3456789"
    PRINT(str1.slice(3, 8)); // "34567"
    PRINT(str1.slice(3, -2)); // "34567"
    PRINT(str1.slice(-7, -2)); // "34567"
    std::cout << std::endl;
    
    PRINT(str1.substr(3)); // "3456789"
    PRINT(str1.substr(3, 5)); // "34567"
    PRINT(str1.substr(-7, 5)); // "34567"
    std::cout << std::endl;
    
    PRINT("{}, {}!"_format("Hello", "world")); // Hello, world!
    PRINT("{2}, {1}, {0}"_format("A", "B", "C")); // C, B, A
    std::cout << std::endl;
    
    PRINT("{0:_<5} | {0:_^5} | {0:_>5}"_format(123)); // 123__ | _123_ | __123
    PRINT("{0:_>+5} | {0:_>-5} | {0:_> 5}"_format(123)); // _+123 | __123 |  _ 123
    PRINT("{0:b}, {0:o}, {0:d}, {0:x}, {0:X}, {0:c}"_format(90)); // 1011010, 132, 90, 5a, 5A, Z
    PRINT("{0:#b}, {0:#o}, {0:#d}, {0:#x}, {0:#X}"_format(90)); // 0b1011010, 0o132, 90, 0x5a, 0x5A
    
    return 0;
    
}
