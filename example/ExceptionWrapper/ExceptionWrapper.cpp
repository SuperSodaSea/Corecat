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

#include <iostream>

#include "Cats/Corecat/Util.hpp"

using namespace Cats::Corecat::Util;

#define PRINT(x) do { std::cout << #x << " -> " << (x) << std::endl; } while(0)

ExceptionWrapper func1() {
    
    try { throw 233; } catch(...) { return ExceptionWrapper::current(); }
    
}
ExceptionWrapper func2() {
    
    return std::runtime_error("Runtime error");
    
}

int main() {
    
    auto ew1 = func1();
    try { ew1.rethrow(); }
    catch(int e1) { PRINT(e1); }
    
    auto ew2 = func2();
    ew2.with([](const std::runtime_error& e2) { PRINT(e2.what()); });
    
    return 0;
    
}
