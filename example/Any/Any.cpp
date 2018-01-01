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
#include "Cats/Corecat/Util.hpp"


using namespace Cats::Corecat;


#define PRINT(x) do { std::cout << #x << " -> " << (x) << std::endl; } while(0)

int main() {
    
    Any a1 = 42;
    Any a2 = 2.33;
    Any a3 = String8("Hello");
    PRINT(a1.get<int>()); // 42
    PRINT(a2.get<double>()); // 2.33
    PRINT(a3.get<String8>()); // Hello
    std::cout << std::endl;
    
    a1 = a3;
    PRINT(a1.get<String8>()); // Hello
    std::cout << std::endl;
    
    try {
        
        Any a4 = 234;
        a4.get<double>(); // Throw exception
        
    } catch(std::exception& e) { PRINT(e.what()); }
    
    return 0;
    
}
